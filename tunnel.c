#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structure/queue.h"
#include "common/global.h"
#include "structure/array.h"
#include "client/client.h"

int main() {
    char *ip = "47.74.188.43";
    int r_port = 8877;
    int l_port = 8088;
    char *password = "0101001";
    return create_tunnel(ip, r_port, l_port, password);
}