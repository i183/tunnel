#include <stdio.h>
#include <string.h>
#include "client.h"
#include "common.h"
#include "../common/command.h"
#include "os.h"

#define READ_BUF_LEN 1024

fd_list rl, wl, el;
Array tag;
int rfd;
char token[50];
char rip[30];
int rport, lport;

void init() {
    rl.li = newArrayDefault(sizeof(struct connection *));
    wl.li = newArrayDefault(sizeof(struct connection *));
    el.li = newArrayDefault(sizeof(struct connection *));
    tag = newArrayDefault(sizeof(struct connection *));
}

void add_fd_to_rel(struct connection *conn) {
    addArrayForPointer(rl.li, conn);
    addArrayForPointer(el.li, conn);
}

void add_fd_to_wl(struct connection *conn) {
    addArrayForPointer(wl.li, conn);
}

/**
 * 事件分发
 * @param conn
 * @return
 */
int dist(struct connection *conn);

int handler_1(struct connection *conn);

int handler_2(struct connection *conn);

int handler_3(struct connection *conn);

int create_tunnel(char *ip, int r_port, int l_port, char *password) {
    strcpy(rip, ip);
    rport = r_port;
    lport = l_port;

    socket_start();
    init();

    socket_t sock = socket_stream();
    if (sock == INVALID_SOCKET) {
        perror("invalid socket");
        return -1;
    }

    sockaddr_t dest_addr = create_sockaddr(ip, r_port);

    if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
        perror("connect error");
        socket_close(sock);
        return -1;
    }

    struct connection *tunnel = create_conn(sock, C_TUNNEL, false, null);

    char msg[256];
    sprintf(msg, "%s %s\n", TUNNEL, password);
    if (write_data(tunnel, msg, strlen(msg)) == -1) {
        return -1;
    }

    add_fd_to_rel(tunnel);

    while (true) {
        int count = select_os(&rl, &wl, &el, -1);
        printf("count: %d\n", count);
        for (int i = 0; i < el.num; ++i) {
            tag_close_conn(getArrayForPointer(el.li, i), tag);
        }
        verify_asyn_conn(&rl, tag);
        verify_asyn_conn(&wl, tag);
        for (int i = 0; i < rl.num; ++i) {
            struct connection *conn = getArrayForPointer(rl.li, i);
            dist(conn);
        }
        for (int i = 0; i < wl.num; ++i) {
            struct connection *conn = getArrayForPointer(wl.li, i);
            dist(conn);
        }
        close_conn_arr(tag);
        resetArrayEmpty(tag);
    }

    return 0;
}

int dist(struct connection *conn) {
    if (conn->tag_close == true) {
        return 1;
    }

    if (conn->type == 1) {
        handler_1(conn);
    } else if (conn->type == 2) {
        handler_2(conn);
    } else if (conn->type == 3) {
        handler_3(conn);
    }
    return 0;
}

int connecto(struct connection *conn, const sockaddr_t *addr) {
    if (conn->is_asyn) {
        int r = socket_connect(conn->fd, addr);
        //非阻塞connect 返回 -1 并且 errno == ECONNECTED 表示正在建立链接
        if (r == -1 && errno == ECONNECTED) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return socket_connect(conn->fd, addr);
    }
}

int request() {
    socket_t rs = socket_stream();
    socket_t ls = socket_stream();
    struct connection *rc = create_conn(rs, C_R_SERVER, true, null);
    struct connection *lc = create_conn(ls, C_L_SERVER, true, null);
    struct r_server_conn *rcp = malloc(sizeof(struct r_server_conn));
    struct l_server_conn *lcp = malloc(sizeof(struct l_server_conn));
    rcp->l_server_conn = lc;
    lcp->r_server_conn = rc;
    rc->ptr = rcp;
    lc->ptr = lcp;

    sockaddr_t raddr = create_sockaddr(rip, rport);
    sockaddr_t laddr = create_sockaddr("127.0.0.1", lport);

    if (connecto(rc, &raddr) == -1 || connecto(lc, &laddr) == -1) {
        close_conn(rc);
        close_conn(lc);
        return -1;
    }

    add_fd_to_rel(rc);
    add_fd_to_rel(lc);
    add_fd_to_wl(rc);
    add_fd_to_wl(lc);
}

int handler_1(struct connection *conn) {
    boolean done = false;
    while (true) {
        if (conn->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        printf("handler_1 len: %ld\n", len);
        if (len == -1) {
            if (EAGAIN != errno) {
                perror("Read data");
                done = true;
            }
            break;
        } else if (len == 0) {
            done = true;
            break;
        }
        buf[len] = 0;

        line_to_zero(buf);
        printf("handler_1 Read the content: %s\n", buf);

        char command[30] = "";
        sscanf(buf, "%s", command);
        if (strcmp(command, SUCCESS) == 0) {
            int a_port;
            sscanf(buf, "success %d %d %s", &rfd, &a_port, token);
            printf("success fd: %d, port: %d, token: %s\n", rfd, a_port, token);
        } else if (strcmp(command, REQUEST) == 0) {
            printf("REQUEST command\n");
        } else {
            done = true;
            printf("Unknown command: %s\n", command);
            break;
        }
    }

    if (done) {
        tag_close_conn(conn, tag);
    }
    return 0;
}

int handler_2(struct connection *conn) {
    return 0;
}

int handler_3(struct connection *conn) {
    return 0;
}