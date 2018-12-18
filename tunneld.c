#include <stdio.h>
#include <stdlib.h>
#include "server/server.h"
#include "common/global.h"

#define BUF_SIZE 100

int main() {
    char *buf = malloc(BUF_SIZE);
    int port;
    char password[BUF_SIZE];
    printf("Enter listening port(default 8877):");
    enter(buf, BUF_SIZE, "8877");
    port = atoi(buf);
    printf("Enter connection password(default maqian.cc):");
    enter(password, BUF_SIZE, "maqian.cc");
    free(buf);
    printf("Port:%d, Password:%s\n", port, password);
    return start(port, password);
}