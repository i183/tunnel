#include <stdio.h>
#include <stdlib.h>
#include "common/global.h"
#include "client/client.h"

#define BUF_SIZE 100

int main() {
    char *buf = malloc(BUF_SIZE);
    int r_port, l_port;
    char ip[BUF_SIZE], password[BUF_SIZE];
    printf("Enter server IP:");
    scanf("%s", ip);
    clear_stdin();

    printf("Enter server port(default 8877):");
    enter(buf, BUF_SIZE, "8877");
    r_port = atoi(buf);

    printf("Enter connection password(default maqian.cc):");
    enter(password, BUF_SIZE, "maqian.cc");

    printf("Enter local port:");
    scanf("%s", buf);
    clear_stdin();
    l_port = atoi(buf);
    free(buf);
    printf("Server IP:%s, Server Port:%d, Password:%s, Local Port:%d\n", ip, r_port, password, l_port);
    return create_tunnel(ip, r_port, l_port, password);
}