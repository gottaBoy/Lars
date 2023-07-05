#include "event_loop.h"
#include <assert.h>
#include <sys/event.h>

//构造，初始化epoll堆
event_loop::event_loop() 
{
    //flag=0 等价于epll_craete
    // _epfd = epoll_create1(0);
    _epfd = kqueue();
    if (_epfd == -1) {
        fprintf(stderr, "epoll_create error\n");
        exit(1);
    }
}


//阻塞循环处理事件
void event_loop::event_process()
{
    while (true) {
        io_event_map_it ev_it;

        // int nfds = epoll_wait(_epfd, _fired_evs, MAXEVENTS, 10);
        int nfds = kevent(_epfd, NULL, 0, _fired_evs, MAXEVENTS, NULL);
        for (int i = 0; i < nfds; i++) {
            //通过触发的fd找到对应的绑定事件
            ev_it = _io_evs.find(_fired_evs[i].ident);
            assert(ev_it != _io_evs.end());

            io_event *ev = &(ev_it->second);

            // if (_fired_evs[i].events & EPOLLIN) {
            if (_fired_evs[i].filter & EVFILT_READ) {
                //读事件，掉读回调函数
                void *args = ev->rcb_args;
                ev->read_callback(this, _fired_evs[i].ident, args);
            }
            // else if (_fired_evs[i].events & EPOLLOUT) {
            else if (_fired_evs[i].filter & EVFILT_WRITE) {
                //写事件，掉写回调函数
                void *args = ev->wcb_args; 
                ev->write_callback(this, _fired_evs[i].ident, args);
            }
            // else if (_fired_evs[i].events &(EPOLLHUP|EPOLLERR)) {
            else if (_fired_evs[i].filter & EVFILT_EXCEPT) {
                //水平触发未处理，可能会出现HUP事件，正常处理读写，没有则清空
                if (ev->read_callback != NULL) {
                    void *args = ev->rcb_args;
                    ev->read_callback(this, _fired_evs[i].ident, args);
                }
                else if (ev->write_callback != NULL) {
                    void *args = ev->wcb_args;
                    ev->write_callback(this, _fired_evs[i].ident, args);
                }
                else {
                    //删除
                    fprintf(stderr, "fd %lu get error, delete it from epoll\n", _fired_evs[i].ident);
                    this->del_io_event(_fired_evs[i].ident);
                }
            }

        }
    }
}

/*
 * 这里我们处理的事件机制是
 * 如果EPOLLIN 在mask中， EPOLLOUT就不允许在mask中
 * 如果EPOLLOUT 在mask中， EPOLLIN就不允许在mask中
 * 如果想注册EPOLLIN|EPOLLOUT的事件， 那么就调用add_io_event() 方法两次来注册。
 * */

//添加一个io事件到loop中
void event_loop::add_io_event(int fd, io_callback *proc, int mask, void *args)
{
    int final_mask;
    int op;

    //1 找到当前fd是否已经有事件
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        //2 如果没有操作动作就是ADD
        //没有找到
        final_mask = mask;    
        // op = EPOLL_CTL_ADD;
        op = EV_ADD;
    }
    else {
        //3 如果有操作董酒是MOD
        //添加事件标识位
        final_mask = it->second.mask | mask;
        // op = EPOLL_CTL_MOD;
        op = EV_ADD;
    }

    //4 注册回调函数
    // if (mask & EPOLLIN) {
    if (mask & EVFILT_READ) {
        //读事件回调函数注册
        _io_evs[fd].read_callback = proc;
        _io_evs[fd].rcb_args = args;
    }
    // else if (mask & EPOLLOUT) {
    else if (mask & EVFILT_WRITE) {
        _io_evs[fd].write_callback = proc;
        _io_evs[fd].wcb_args = args;
    }
    
    //5 epoll_ctl添加到epoll堆里
    _io_evs[fd].mask = final_mask;
    //创建原生epoll事件
    // struct epoll_event event;
    // event.events = final_mask;
    // event.data.fd = fd;
    struct kevent event;
    // event.filter = final_mask;
    // event.ident = fd;
    // EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);  // 赋值
    EV_SET(&event, fd, final_mask, EV_ADD, 0, 0, NULL);  // 赋值
    _io_evs[fd].kev = event;
    kevent(_epfd, &event, op, NULL, 0, NULL);    // 添加
    // if (epoll_ctl(_epfd, op, fd, &event) == -1) {
    //     fprintf(stderr, "epoll ctl %d error\n", fd);
    //     return;
    // }

    //6 将fd添加到监听集合中
    listen_fds.insert(fd);
}

//删除一个io事件从loop中
void event_loop::del_io_event(int fd)
{
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        return ;
    }
    // struct io_event io_ev= it->second;
    struct kevent event = _io_evs[fd].kev;
    //将事件从_io_evs删除
    _io_evs.erase(fd);

    //将fd从监听集合中删除
    listen_fds.erase(fd);

    //将fd从epoll堆删除
    // epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
    EV_SET(&event, fd, NULL, EV_DELETE, 0, 0, NULL);  // 赋值
    kevent(_epfd, &event, EV_DELETE, NULL, 0, NULL);    // 添加
}

//删除一个io事件的EPOLLIN/EPOLLOUT
void event_loop::del_io_event(int fd, int mask)
{
    //如果没有该事件，直接返回
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        return ;
    }

    int &o_mask = it->second.mask;
    //修正mask
    o_mask = o_mask & (~mask);
    
    if (o_mask == 0) {
        //如果修正之后 mask为0，则删除
        this->del_io_event(fd);
    }
    // else {
    //     //如果修正之后，mask非0，则修改
    //     struct epoll_event event;
    //     event.events = o_mask;
    //     event.data.fd = fd;
    //     epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event);
    // }
}