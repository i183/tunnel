#include "server.h"

#ifndef TUNNEL_CMD_H
#define TUNNEL_CMD_H

int tunnel_cmd(int epfd, struct connection *conn, const char *cmd, const char *pw);

int pull_cmd(int epfd, struct connection *conn, const char *cmd);

#endif //TUNNEL_CMD_H
