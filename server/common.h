#ifndef TUNNEL_COMMON_H
#define TUNNEL_COMMON_H

struct connection *create_conn(int fd, int type, void *ptr);

int close_conn(struct connection *conn);

int write_data(struct connection *conn, const void *buf, size_t len);

#endif //TUNNEL_COMMON_H
