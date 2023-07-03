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