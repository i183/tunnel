//
// Created by maqian on 2018/9/7.
//

#include <sys/epoll.h>
#include "../common/global.h"
#include "../structure/queue.h"

#ifndef TUNNEL_SERVER_H
#define TUNNEL_SERVER_H

#define S_LISTEN_CLIENT 1
#define S_LISTEN_USER 2
#define S_UNKNOWN 3
#define S_TUNNEL 4
#define S_CLIENT 5
#define S_USER 6

struct connection {
    int fd;
    char type; //1.监听客户端请求的Socket 2.监听用户请求的Socket 3.未知 4.与客户端建立的隧道 5.客户端处理请求连接 6.用户的请求连接
    void *write_buf; //等待写入的数据
    size_t len; //等待写入数据的长度
    void *ptr; //额外数据指针
};

struct listen_user {
    struct connection *tunnel_conn;
    int port; //监听的端口号
    Queue queue;
};

struct tunnel {
    struct connection *listen_user_conn;
    char *token; //客户端处理请求连接时需要验证Token
};

struct client_conn {
    struct connection *user_conn;
};

struct user_conn {
    struct connection *client_conn;
};

int start(int port, char *password);

#endif //TUNNEL_SERVER_H
