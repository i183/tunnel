#ifndef TUNNEL_COMMON_H
#define TUNNEL_COMMON_H

#include "../structure/array.h"

struct connection *create_conn(int fd, int type, void *ptr);

void tag_close_conn(struct connection *conn, Array arr);

int close_conn(struct connection *conn);

void close_conn_arr(Array arr);

int write_data(struct connection *conn, const void *buf, size_t len);

int make_socket_non_blocking(int fd);

#endif //TUNNEL_COMMON_H
