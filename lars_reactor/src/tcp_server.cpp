#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <signal.h>
#include <sys/event.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "tcp_server.h"
#include "tcp_conn.h"
#include "reactor_buf.h"

// ==== 链接资源管理   ====
//全部已经在线的连接信息
tcp_conn ** tcp_server::conns = NULL;

//最大容量链接个数;
int tcp_server::_max_conns = 0;      

//当前链接刻度
int tcp_server::_curr_conns = 0;

//保护_curr_conns刻度修改的锁
pthread_mutex_t tcp_server::_conns_mutex = PTHREAD_MUTEX_INITIALIZER;

// ==== 消息分发路由   ===
msg_router tcp_server::router;

//创建链接之后的回调函数
conn_callback tcp_server::conn_start_cb = NULL;
void * tcp_server::conn_start_cb_args = NULL;

//销毁链接之前的回调函数
conn_callback tcp_server::conn_close_cb = NULL;
void * tcp_server::conn_close_cb_args = NULL;

//临时的收发消息
// struct message{
//     char data[m4K];
//     char len;
// };
// struct message msg;

// void server_rd_callback(event_loop *loop, int fd, void *args);
// void server_wt_callback(event_loop *loop, int fd, void *args);

//listen fd 客户端有新链接请求过来的回调函数
void accept_callback(event_loop *loop, int fd, void *args)
{
    tcp_server *server = (tcp_server*)args;
    server->do_accept();
}

//server的构造函数
tcp_server::tcp_server(event_loop *loop, const char *ip, uint16_t port)
{
    bzero(&_connaddr, sizeof(_connaddr));
    
    //忽略一些信号 SIGHUP, SIGPIPE
    //SIGPIPE:如果客户端关闭，服务端再次write就会产生
    //SIGHUP:如果terminal关闭，会给当前进程发送该信号
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "signal ignore SIGHUP\n");
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "signal ignore SIGPIPE\n");
    }

    //1. 创建socket
    _sockfd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK| SOCK_CLOEXEC */ , IPPROTO_TCP);
    if (_sockfd == -1) {
        fprintf(stderr, "tcp_server::socket()\n");
        exit(1);
    }

    //2 初始化地址
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    //2-1可以多次监听，设置REUSE属性
    int op = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        fprintf(stderr, "setsocketopt SO_REUSEADDR\n");
    }

    //3 绑定端口
    if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "bind error\n");
        exit(1);
    }

    //4 监听ip端口
    if (listen(_sockfd, 500) == -1) {
        fprintf(stderr, "listen error\n");
        exit(1);
    }

    //5 将_sockfd添加到event_loop中
    _loop = loop;

    //6 =============  创建链接管理 ===============
    _max_conns = MAX_CONNS;
    //创建链接信息数组
    conns = new tcp_conn*[_max_conns+3];//3是因为stdin,stdout,stderr 已经被占用，再新开fd一定是从3开始,所以不加3就会栈溢出
    if (conns == NULL) {
        fprintf(stderr, "new conns[%d] error\n", _max_conns);
        exit(1);
    }
    //===========================================

    //7 注册_socket读事件-->accept处理
    _loop->add_io_event(_sockfd, accept_callback, EVFILT_READ, this);
}

//新增一个新建的连接
void tcp_server::increase_conn(int connfd, tcp_conn *conn)
{
    pthread_mutex_lock(&_conns_mutex);
    conns[connfd] = conn;
    _curr_conns++;
    pthread_mutex_unlock(&_conns_mutex);
}

//减少一个断开的连接
void tcp_server::decrease_conn(int connfd)
{
    pthread_mutex_lock(&_conns_mutex);
    conns[connfd] = NULL;
    _curr_conns--;
    pthread_mutex_unlock(&_conns_mutex);
}

//得到当前链接的刻度
void tcp_server::get_conn_num(int *curr_conn)
{
    pthread_mutex_lock(&_conns_mutex);
    *curr_conn = _curr_conns;
    pthread_mutex_unlock(&_conns_mutex);
}

//server read_callback
// void server_rd_callback(event_loop *loop, int fd, void *args)
// {
//     int ret = 0;

//     struct message *msg = (struct message*)args;
//     input_buf ibuf;

//     ret = ibuf.read_data(fd);
//     if (ret == -1) {
//         fprintf(stderr, "ibuf read_data error\n");
//         //删除事件
//         loop->del_io_event(fd);
        
//         //对端关闭
//         close(fd);

//         return;
//     }
//     if (ret == 0) {
//         //删除事件
//         loop->del_io_event(fd);
        
//         //对端关闭
//         close(fd);
//         return ;
//     }

//     printf("ibuf.length() = %d\n", ibuf.length());
    
//     //将读到的数据放在msg中
//     msg->len = ibuf.length();
//     bzero(msg->data, msg->len);
//     memcpy(msg->data, ibuf.data(), msg->len);

//     ibuf.pop(msg->len);
//     ibuf.adjust();

//     printf("recv data = %s\n", msg->data);

    
//     //删除读事件，添加写事件
//     loop->del_io_event(fd, EVFILT_READ);
//     loop->add_io_event(fd, server_wt_callback, EVFILT_WRITE, msg);
// }

//server write_callback
// void server_wt_callback(event_loop *loop, int fd, void *args)
// {
//     struct message *msg = (struct message*)args;
//     output_buf obuf;

//     //回显数据
//     obuf.send_data(msg->data, msg->len);
//     while(obuf.length()) {
//         int write_ret = obuf.write2fd(fd);
//         if (write_ret == -1) {
//             fprintf(stderr, "write connfd error\n");
//             return;
//         }
//         else if(write_ret == 0) {
//             //不是错误，表示此时不可写
//             break;
//         }
//     }

//     //删除写事件，添加读事件
//     loop->del_io_event(fd, EVFILT_WRITE);
//     loop->add_io_event(fd, server_rd_callback, EVFILT_READ, msg);
// }

//开始提供创建链接服务
void tcp_server::do_accept()
{
    int connfd;    
    while(true) {
        //accept与客户端创建链接
        printf("begin accept\n");
        connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
        if (connfd == -1) {
            if (errno == EINTR) {
                fprintf(stderr, "accept errno=EINTR\n");
                continue;
            }
            else if (errno == EMFILE) {
                //建立链接过多，资源不够
                fprintf(stderr, "accept errno=EMFILE\n");
            }
            else if (errno == EAGAIN) {
                fprintf(stderr, "accept errno=EAGAIN\n");
                break;
            }
            else {
                fprintf(stderr, "accept error");
                exit(1);
            }
        }
        else {
            //accept succ!
            //TODO 添加心跳机制
            
            //TODO 消息队列机制

            int cur_conns;
            get_conn_num(&cur_conns);

            //1 判断链接数量
            if (cur_conns >= _max_conns) {
                fprintf(stderr, "so many connections, max = %d\n", _max_conns);
                close(connfd);
            }
            else {
                tcp_conn *conn = new tcp_conn(connfd, _loop);
                if (conn == NULL) {
                    fprintf(stderr, "new tcp_conn error\n");
                    exit(1);
                }
                printf("get new connection succ!\n");
            }
            break;
            
        }
    }
}

//链接对象释放的析构
tcp_server::~tcp_server()
{
    close(_sockfd);
}