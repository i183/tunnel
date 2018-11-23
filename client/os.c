#include <fcntl.h>
#include <stdio.h>
#include "os.h"
#include "client.h"
#include "../common/global.h"

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