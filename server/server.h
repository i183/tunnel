//
// Created by maqian on 2018/9/7.
//

#include "../common/global.h"

#ifndef TUNNEL_SERVER_H
#define TUNNEL_SERVER_H

struct Connection {
    int fd;
    char type; //1.与客户端建立的隧道 2.客户端处理请求连接 3.用户的请求连接
    void *dataPtr; //额外数据指针
};

struct Tunnel {
    char *token; //客户端处理请求连接时需要验证Token
};

struct ClientConn {
    boolean isAuth; //身份是否验证
    int userConnFD; //用户请求连接的描述文件
};

struct UserConn {
    int clientConnFD; //客户端处理请求连接的描述文件
};

#endif //TUNNEL_SERVER_H
