//
// Created by maqian on 2018/9/11.
//
#include "server.h"

#ifndef TUNNEL_TUNNEL_H
#define TUNNEL_TUNNEL_H

/**
 * 初始化Tunnel散列
 * @param size 散列大小
 */
void initTunnelMap(int size);

/**
 * 创建 tunnel
 * @param epfd
 * @param conn
 * @return
 */
int create_tunnel(int epfd, struct connection *conn);

struct connection *get_tunnel(int fd);

boolean remove_tunnel(int fd);

#endif //TUNNEL_TUNNEL_H
