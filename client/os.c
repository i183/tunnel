#include "os.h"
#include "../common/global.h"

int sock_init() {
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1,1), &wsa_data);
#else
    return 0;
#endif
}

int sock_quit() {
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}

int closesocket(SOCKET sock) {
    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) { status = closesocket(sock); }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) { status = close(sock); }
#endif

    return status;

}

SOCKET getArrayForSOCKET(Array arr, int index) {
    SOCKET item;
    getArray(arr, index, &item);
    return item;
}

void setArrayForSOCKET(Array arr, int index, SOCKET sock) {
    setArray(arr, index, &sock);
}

void fd_set_to_fd_list(fd_list *fl, fd_set *fs) {
    if (fl && fl->li && fs) {
        fl->num = 0;
        Array li = fl->li;
        for (int i = 0; i < li->size; i++) {
            SOCKET sock = getArrayForSOCKET(li, i);
            if (FD_ISSET(sock, fs)) {
                if (fl->num != i) {
                    SOCKET temp = getArrayForSOCKET(li, fl->num);
                    setArrayForSOCKET(li, fl->num, sock);
                    setArrayForSOCKET(li, i, temp);
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
            if (getArrayForInt(li, i) > max) {
                max = getArrayForInt(li, i);
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
            SOCKET sock = getArrayForSOCKET(li, i);
            FD_SET(sock, fs);
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