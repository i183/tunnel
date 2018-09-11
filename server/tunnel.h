//
// Created by maqian on 2018/9/11.
//
#include "server.h"

#ifndef TUNNEL_TUNNEL_H
#define TUNNEL_TUNNEL_H

/**
 * 创建 tunnel
 * @param epfd
 * @param conn
 * @return
 */
int create_tunnel(int epfd, struct connection *conn);

#endif //TUNNEL_TUNNEL_H
