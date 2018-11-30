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

typedef unsigned char boolean;

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

void line_to_zero(char *s);

/**
 * 获取系统类型
 * @return 1.linux 2.os x 3.windows 4.其他
 */
int get_system_type();

/**
 * 比较一段内存存储的数据是否相同
 * @param p1
 * @param p2
 * @param n 内存段字节数
 * @return
 */
boolean memeq(void *p1, void *p2, int n);

#endif //LEARN_C_COMMON_H
