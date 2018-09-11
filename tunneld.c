#include <stdio.h>
#include "server/server.h"

int main() {
    int port = 8877;
    char *password = "0101001";
    return start(port, password);
}