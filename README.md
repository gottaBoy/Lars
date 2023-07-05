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

# 链接
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
