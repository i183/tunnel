//
// Created by maqian on 2018/6/15.
//

#ifndef LEARN_C_STACK_H
#define LEARN_C_STACK_H

typedef struct StackNode {

    void *data;

    struct StackNode *next;

} StackData, *StackDataPtr;

typedef struct StackStructure{

    StackData *head;//栈数据头指针

    int typeSize;//类型占用字节数

    int size;//元素个数

} *Stack;

/**
 * 创建栈
 * @param typeSize
 * @return
 */
Stack newStack(int typeSize);

/**
 * 栈插入
 * @param sp
 * @param item
 */
void inStack(Stack s, void *item);

/**
 * 栈弹出
 * @param sp
 * @param dst
 * @return
 */
void *outStack(Stack s, void *dst);

/**
 * 栈是否为空
 * @param sp
 * @return
 */
int isStackEmpty(Stack s);

/**
 * 重置为空栈
 * @param sp
 */
void resetStackEmpty(Stack s);

/**
 * 销毁栈
 * @param s
 */
void freeStack(Stack s);


/**
 * 插入int
 * @param sp
 * @param item
 */
void inStackForInt(Stack s, int item);

/**
 * 插入long
 * @param sp
 * @param item
 */
void inStackForLong(Stack s, long item);

/**
 * 插入double
 * @param sp
 * @param item
 */
void inStackForDouble(Stack s, double item);

/**
 * 插入char
 * @param sp
 * @param item
 */
void inStackForChar(Stack s, char item);


/**
 * 出栈int
 * @param sp
 * @param item
 */
int outStackForInt(Stack s);

/**
 * 出栈long
 * @param sp
 * @param item
 */
long outStackForLong(Stack s);

/**
 * 出栈double
 * @param sp
 * @param item
 */
double outStackForDouble(Stack s);

/**
 * 出栈char
 * @param sp
 * @param item
 */
char outStackForChar(Stack s);

#endif //LEARN_C_STACK_H
