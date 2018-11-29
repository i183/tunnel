#ifndef TUNNEL_CLIENT_H
#define TUNNEL_CLIENT_H

#include "os.h"
#include "../common/global.h"

#define C_TUNNEL 1
#define C_R_SERVER 2
#define C_L_SERVER 3

struct connection {
    socket_t fd;
    char type; //1.与服务器建立的隧道 2.远程服务器连接 3.本地服务器连接
    void *write_buf; //等待写入的数据
    int len; //等待写入数据的长度
    boolean tag_close; //标记已关闭
    boolean is_asyn; //是否为异步连接
    boolean asyn_conn; //异步连接是否成功
    void *ptr; //额外数据指针
};

/**
 * 远程服务器连接
 */
struct r_server_conn {
    struct connection *l_server_conn;
};

/**
 * 本地服务器连接
 */
struct l_server_conn {
    struct connection *r_server_conn;
};

/**
 * 与服务器建立隧道
 * @param ip 远程服务器IP
 * @param r_port 远程服务器端口
 * @param l_port 本地服务器端口
 * @param password
 * @return
 */
int create_tunnel(char *ip, int r_port, int l_port, char *password);

#endif //TUNNEL_CLIENT_H
