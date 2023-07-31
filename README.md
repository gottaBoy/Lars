# c go
https://seisman.github.io/how-to-write-makefile/introduction.html
https://github.com/aceld/golang
https://www.kancloud.cn/aceld/lars/2084910e

### 构建 make
```shell
g++ -g -O2 -Wall -fPIC -Wno-deprecated -c -o src/tcp_server.o src/tcp_server.cpp -I./include
mkdir -p lib
ar cqs lib/liblreactor.a src/tcp_server.o
```

### 客户端
```
nc 127.0.0.1 7777
```


### protobuf环境安装
A) protobuf 编译工具安装
https://github.com/protocolbuffers/protobuf/blob/main/src/README.md

1、下载 protoBuf：
```shell
sudo wget https://github.com/protocolbuffers/protobuf/releases/download/v3.19.4/protobuf-all-3.19.4.tar.gz
sudo tar -zxvf protobuf-all-3.19.4.tar.gz


cd $GOPATH/src/
git clone https://github.com/protocolbuffers/protobuf.git
```
2、或者直接将压缩包拖入后解压
```shell
unzip protobuf.zip 
```
3、安装依赖库
```shell
sudo apt-get install autoconf  automake  libtool curl make  g++  unzip libffi-dev -y
```
4、进入目录
```shell
cd protobuf/ 
```
5、自动生成configure配置文件：
```shell
# git submodule update --init --recursive
./autogen.sh 
```
6、配置环境：
```shell
# ./configure
./configure --prefix=/usr/local/protobuf
```
7、编译源代码(时间比较长)：
```shell
make 
```
8、安装
```shell
sudo make install
```
9、刷新共享库 （很重要的一步啊）
```shell
sudo ldconfig 

yum install libprotobuf-dev protobuf-compiler -y
yum update libprotobuf-dev protobuf-compiler -y
#设置环境变量
# Libraries have been installed in:
#    /usr/local/protobuf/lib

# If you ever happen to want to link against installed libraries
# in a given directory, LIBDIR, you must either use libtool, and
# specify the full pathname of the library, or use the `-LLIBDIR'
# flag during linking and do at least one of the following:
#    - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
#      during execution
#    - add LIBDIR to the `LD_RUN_PATH' environment variable
#      during linking
#    - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
#    - have your system administrator add LIBDIR to `/etc/ld.so.conf'
# (动态库搜索路径) 程序加载运行期间查找动态链接库时指定除了系统默认路径之外的其他路径
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/protobuf/lib
# (静态库搜索路径) 程序编译期间查找动态链接库时指定查找共享库的路径
export LIBRAR


sudo vim /etc/profile
# 添加内容如下：
#(动态库搜索路径) 程序加载运⾏期间查找动态链接库时指定除了系统默认路径之外的其他路径
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/protobuf/lib/
#(静态库搜索路径) 程序编译期间查找动态链接库时指定查找共享库的路径
export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/protobuf/lib/
#执⾏程序搜索路径
export PATH=$PATH:/usr/local/protobuf/bin/
#c程序头⽂件搜索路径
export C_INCLUDE_PATH=$C_INCLUDE_PATH:/usr/local/protobuf/include/
#c++程序头⽂件搜索路径
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/protobuf/include/
#pkg-config 路径
export PKG_CONFIG_PATH=/usr/local/protobuf/lib/pkgconfig/

source /etc/profile
```
10、成功后需要使用命令测试
```shell
protoc -h  
```

# 链接
- [图解 | 深入揭秘 epoll 是如何实现 IO 多路复用的](https://www.ithome.com/0/644/835.htm)
- [C++ workflow](https://zhuanlan.zhihu.com/p/484293077)
- [sockaddr和sockaddr_in详解](https://blog.csdn.net/qingzhuyuxian/article/details/79736821)
- [设计模式](https://blog.csdn.net/u014376961/category_9315271.html)
- [socket](https://blog.csdn.net/u014376961/article/details/88614029)
- [c++17环境配置](https://blog.csdn.net/fighting_Kitty/article/details/124774798)
- [Buffer Pool 核心原理](https://blog.csdn.net/weixin_35794878/article/details/125737083)
- [C/C++基础](https://blog.csdn.net/kingofmyheart/category_8847220.html)
- [C++中如何定义指向类成员的指针](https://blog.csdn.net/KingOfMyHeart/article/details/124655014)
- [C++ 单例模式总结](https://juejin.cn/post/6844903928497176584)
- [C++ STL教程之 string 常用成员函数](https://blog.csdn.net/sjc_0910/article/details/103533388)
- [C++】const详解](https://blog.csdn.net/HelloZEX/article/details/83861533)
- [三种结构体初始化方法](https://www.cnblogs.com/wlw-x/p/11566191.html)
- [kqueue用法简介](https://blog.csdn.net/wangquan1992/article/details/121268253)
- [epoll](https://www.cnblogs.com/lojunren/p/3856290.html)
- [用 kqueue 实现一个简单的 TCP Server](https://h1z3y3.me/posts/writing-a-tcp-server-using-kqueue/)
- [freebsd](https://www.cnblogs.com/fengyv/archive/2012/07/30/2614783.html)
- [C++下基本数据类型总结 (占位符 输入符 输出符)](https://blog.csdn.net/whorus1/article/details/108868432)
- [pthread_mutex](https://blog.csdn.net/l_z_y_000/article/details/128365134)
- [](https://blog.csdn.net/NEFUT/article/details/130945050)