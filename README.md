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
-[sockaddr和sockaddr_in详解](https://blog.csdn.net/qingzhuyuxian/article/details/79736821)
-[设计模式](https://blog.csdn.net/u014376961/category_9315271.html)
-[socket](https://blog.csdn.net/u014376961/article/details/88614029)
-[c++17环境配置](https://blog.csdn.net/fighting_Kitty/article/details/124774798)