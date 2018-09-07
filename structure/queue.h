//
// Created by maqian on 2018/6/15.
//

#ifndef LEARN_C_QUEUE_H
#define LEARN_C_QUEUE_H
typedef struct QueueNode {

    void *data;

    struct QueueNode *next;

} QueueData, *QueueDataPtr;

typedef struct QueueStructure{

    QueueData *head;//队列头指针

    QueueData *rear;//队列尾指针

    int typeSize;//类型占用字节数

    int size;//元素个数

} *Queue;

/**
 * 创建队列
 * @param typeSize
 * @return
 */
Queue newQueue(int typeSize);

/**
 * 队列插入
 * @param sp
 * @param item
 */
void inQueue(Queue q, void *item);

/**
 * 队列弹出
 * @param sp
 * @param dst
 * @return
 */
void *outQueue(Queue q, void *dst);

/**
 * 队列是否为空
 * @param sp
 * @return
 */
int isQueueEmpty(Queue q);

/**
 * 重置为空队列
 * @param sp
 */
void resetQueueEmpty(Queue q);

/**
 * 销毁队列
 * @param s
 */
void freeQueue(Queue q);


/**
 * 插入int
 * @param sp
 * @param item
 */
void inQueueForInt(Queue q, int item);

/**
 * 插入long
 * @param sp
 * @param item
 */
void inQueueForLong(Queue q, long item);

/**
 * 插入double
 * @param sp
 * @param item
 */
void inQueueForDouble(Queue q, double item);

/**
 * 插入char
 * @param sp
 * @param item
 */
void inQueueForChar(Queue q, char item);


/**
 * 出队列int
 * @param sp
 * @param item
 */
int outQueueForInt(Queue q);

/**
 * 出队列long
 * @param sp
 * @param item
 */
long outQueueForLong(Queue q);

/**
 * 出队列double
 * @param sp
 * @param item
 */
double outQueueForDouble(Queue q);

/**
 * 出队列char
 * @param sp
 * @param item
 */
char outQueueForChar(Queue q);
#endif //LEARN_C_QUEUE_H
