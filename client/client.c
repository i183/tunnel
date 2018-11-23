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

    if (socket_set_nonblock(sock) == -1) {
        perror("Accept make socket non blocking");
        return -1;
    }

    struct connection *tunnel = create_conn(sock, C_TUNNEL, null);

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

void request() {
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
        sscanf(buf, "%s ", command);
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