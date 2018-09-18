//
// Created by maqian on 2018/6/15.
//

#ifndef LEARN_C_COMMON_H
#define LEARN_C_COMMON_H

#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef null
#define null ((void *)0)
#endif

typedef int boolean;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

void error(char *s);

int make_socket_non_blocking(int fd);

void line_to_zero(char *s);

/**
 * 获取系统类型
 * @return 1.linux 2.os x 3.windows 4.其他
 */
int get_system_type();

#endif //LEARN_C_COMMON_H
