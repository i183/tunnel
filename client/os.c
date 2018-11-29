#include <fcntl.h>
#include <stdio.h>
#include "os.h"
#include "client.h"
#include "../common/global.h"

#ifdef _WIN32
void _socket_start(void) {
    WSACleanup();
}
#endif

void socket_start() {
#ifdef _WIN32
    WSADATA wsad;
    WSAStartup(MAKEWORD(2, 2), &wsad);
    atexit(_socket_start);
#endif
    IGNORE_SIGPIPE();
}

socket_t socket_stream() {
    return socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

sockaddr_t create_sockaddr(char *ip, int port) {
#ifdef _WIN32
    // Windows create sockaddr
#else
    sockaddr_t addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    return addr;
#endif
}

int socket_close(socket_t s) {
#ifdef _WIN32
    return closesocket(s);
#else
    return close(s);
#endif
}

int socket_set_block(socket_t s) {
#ifdef _WIN32
    u_long mode = 0;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int mode = fcntl(s, F_GETFL, 0);
    if (mode == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (mode & O_NONBLOCK)
        return fcntl(s, F_SETFL, mode & ~O_NONBLOCK);
    return 0;
#endif
}

int socket_set_nonblock(socket_t s) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int mode = fcntl(s, F_GETFL, 0);
    if (mode == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (mode & O_NONBLOCK)
        return 0;
    return fcntl(s, F_SETFL, mode | O_NONBLOCK);
#endif
}

int socket_connect(socket_t s, const sockaddr_t *addr) {
    return connect(s, (const struct sockaddr *) addr, sizeof(*addr));
}

int socket_addr(const char *ip, uint16_t port, sockaddr_t *addr) {
    if (!ip || !*ip || !addr) {
        fprintf(stderr, "check empty ip = %s, port = %hu, addr = %p.\n", ip, port, addr);
        return -1;
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
    if (addr->sin_addr.s_addr == INADDR_NONE) {
        struct hostent *host = gethostbyname(ip);
        if (!host || !host->h_addr) {
            fprintf(stderr, "check ip is error = %s.\n", ip);
            return -1;
        }
        // 尝试一种, 默认ipv4
        memcpy(&addr->sin_addr, host->h_addr, host->h_length);
    }
    memset(addr->sin_zero, 0, sizeof addr->sin_zero);

    return 0;
}

void fd_set_to_fd_list(fd_list *fl, fd_set *fs) {
    if (fl && fl->li && fs) {
        fl->num = 0;
        Array li = fl->li;
        for (int i = 0; i < li->size; i++) {
            struct connection *conn = getArrayForPointer(li, i);
            if (FD_ISSET(conn->fd, fs)) {
                if (fl->num != i) {
                    struct connection *temp = getArrayForPointer(li, fl->num);
                    setArrayForPointer(li, fl->num, conn);
                    setArrayForPointer(li, i, temp);
                }
                fl->num++;
            }
        }

    }
}

int get_max_fd(fd_list *fl) {
    int max = 0;
    if (fl && fl->li) {
        Array li = fl->li;
        for (int i = 0; i < li->size; i++) {
            struct connection *conn = getArrayForPointer(li, i);
            if (conn->fd > max) {
                max = conn->fd;
            }
        }
    }
    return max;
}

void fd_list_to_fd_set(fd_list *fl, fd_set *fs) {
    FD_ZERO(fs);
    if (fl && fl->li) {
        Array li = fl->li;
        for (int i = 0; i < li->size; i++) {
            struct connection *conn = getArrayForPointer(li, i);
            FD_SET(conn->fd, fs);
        }
    }
}

int select_linux(fd_list *rl, fd_list *wl, fd_list *el, int timeout_ms) {
    int max_fd = get_max_fd(rl), temp;
    if ((temp = get_max_fd(wl)) > max_fd) {
        max_fd = temp;
    }
    if ((temp = get_max_fd(el)) > max_fd) {
        max_fd = temp;
    }

    struct timeval tv;
    struct timeval *timeout;
    if (timeout_ms < 0) {
        timeout = null;
    } else {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 100;
        timeout = &tv;
    }

    fd_set rs, ws, es;
    fd_list_to_fd_set(rl, &rs);
    fd_list_to_fd_set(wl, &ws);
    fd_list_to_fd_set(el, &es);

    int result = select(max_fd + 1, &rs, &ws, &es, timeout);
    if (result > 0) {
        fd_set_to_fd_list(rl, &rs);
        fd_set_to_fd_list(wl, &ws);
        fd_set_to_fd_list(el, &es);
    }
    return result;
}

int select_os(fd_list *rl, fd_list *wl, fd_list *el, int timeout_ms) {
    return select_linux(rl, wl, el, timeout_ms);
}