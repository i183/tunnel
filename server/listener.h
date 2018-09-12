#ifndef TUNNEL_LISTENER_H
#define TUNNEL_LISTENER_H

#include "../common/global.h"

/**
 * 创建监听器
 * @param port
 * @return
 */
int create_listener(int port, int backlog, boolean reuse_addr, boolean non_blocking);

#endif //TUNNEL_LISTENER_H
