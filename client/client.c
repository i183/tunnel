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
    if(indexOfItemInArray(rl.li, &conn) == -1) {
        addArrayForPointer(rl.li, conn);
    }
    if(indexOfItemInArray(el.li, &conn) == -1) {
        addArrayForPointer(el.li, conn);
    }
}

void add_fd_to_wl(struct connection *conn) {
    if(indexOfItemInArray(wl.li, &conn) == -1) {
        addArrayForPointer(wl.li, conn);
    }
}

void remove_rel_by_conn(struct connection *conn) {
    removeArrayByItem(rl.li, &conn);
    removeArrayByItem(el.li, &conn);
}

void remove_wl_by_conn(struct connection *conn) {
    removeArrayByItem(wl.li, &conn);
}

void remove_fd_by_tag() {
    for (int i = 0; i < tag->size; i++) {
        struct connection *conn = getArrayForPointer(tag, i);
        removeArrayByItem(rl.li, &conn);
        removeArrayByItem(wl.li, &conn);
        removeArrayByItem(el.li, &conn);
    }
}

/**
 * 读取事件分发
 * @param conn
 * @return
 */
int dist_read(struct connection *conn);

/**
 * 写入事件分发
 * @param conn
 * @return
 */
int dist_write(struct connection *conn);

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

    struct connection *tunnel = create_conn(sock, C_TUNNEL, true, null);
    tunnel->asyn_conn = true;

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
            struct connection *conn = getArrayForPointer(el.li, i);
            if (conn->type == C_TUNNEL) {
                error("Tunnel connection is exception");
                return -1;
            }
            tag_close_conn(conn, tag);
        }
        verify_asyn_conn(&rl, tag);
        verify_asyn_conn(&wl, tag);
        for (int i = 0; i < rl.num; ++i) {
            struct connection *conn = getArrayForPointer(rl.li, i);
            dist_read(conn);
        }
        for (int i = 0; i < wl.num; ++i) {
            struct connection *conn = getArrayForPointer(wl.li, i);
            dist_write(conn);
        }
        close_conn_arr(tag);
        remove_fd_by_tag();
        resetArrayEmpty(tag);
    }

    return 0;
}

int dist_read(struct connection *conn) {
    if (conn->tag_close == true) {
        return -1;
    }

    if (conn->type == C_TUNNEL) {
        handler_1(conn);
    } else if (conn->type == C_R_SERVER) {
        handler_2(conn);
    } else if (conn->type == C_L_SERVER) {
        handler_3(conn);
    }
    return 0;
}

int dist_write(struct connection *conn) {
    if (conn->tag_close == true) {
        return -1;
    }

    int r = handler_write(conn);
    remove_wl_by_conn(conn);
    if (r == -1) {
        tag_close_conn(conn, tag);
        return -1;
    }

    if (conn->type == C_R_SERVER) {
        struct r_server_conn *rs = conn->ptr;
        handler_3(rs->l_server_conn);
    } else if (conn->type == C_L_SERVER) {
        struct l_server_conn *ls = conn->ptr;
        handler_2(ls->r_server_conn);
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

    //发送拉请求（由于是异步连接，先写入待写数据区，连接成功后写入）
    char msg[256];
    sprintf(msg, "%s %d %s\n", PULL, rfd, token);
    wait_data(rc, msg, strlen(msg));

    add_fd_to_rel(rc);
    add_fd_to_rel(lc);
    add_fd_to_wl(rc);
    add_fd_to_wl(lc);

    return 0;
}

int read_write(struct connection *read_conn, struct connection *write_conn) {
    while (true) {
        if (write_conn->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN];
        ssize_t len = read(read_conn->fd, buf, READ_BUF_LEN);
        printf("read_write len: %ld\n", len);
        if (len == -1) {
            if (EAGAIN != errno) {
                perror("read_write Read data");
                return -1;
            }
            break;
        } else if (len == 0) {
            return -1;
        }

        int flag = write_data(write_conn, buf, len);
        if (flag == -1) {
            return -1;
        }
    }
    return 0;
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
                perror("handler_1 Read data");
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
            printf("success fd: %d, port: %d, token: %s address: %s:%d\n", rfd, a_port, token, rip, a_port);
        } else if (strcmp(command, REQUEST) == 0) {
            printf("REQUEST command\n");
            request();
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
    struct connection *lc = ((struct r_server_conn *) conn->ptr)->l_server_conn;
    int r = read_write(conn, lc);
    if (r == -1) {
        tag_close_conn(conn, tag);
    }
    return r;
}

int handler_3(struct connection *conn) {
    struct connection *rc = ((struct l_server_conn *) conn->ptr)->r_server_conn;
    int r = read_write(conn, rc);
    if (r == -1) {
        tag_close_conn(conn, tag);
    }
    return r;
}