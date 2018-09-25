#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structure/queue.h"
#include "common/global.h"
#include "structure/array.h"

int main() {
    void *p = malloc(10);
    free(p);
    free(p);
    return 0;
}