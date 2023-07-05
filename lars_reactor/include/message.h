#pragma once

#include "net_connection.h"
#include <ext/hash_map>

//解决tcp粘包问题的消息头
struct msg_head
{
    int msgid;
    int msglen;
};

//消息头的二进制长度，固定数
#define MESSAGE_HEAD_LEN 8

//消息头+消息体的最大长度限制
#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)

//msg 业务回调函数原型
typedef void msg_callback(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data);