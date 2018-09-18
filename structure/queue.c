#include "queue.h"
#include "../common/global.h"
#include <stdlib.h>
#include <string.h>

static void resetNull(void *dst, int size) {
    char *b = dst;
    for (int i = 0; i < size; ++i) {
        b[i] = 0;
    }
}

Queue newQueue(int typeSize) {
    Queue queue = malloc(sizeof(struct QueueStructure));
    queue->typeSize = typeSize;
    queue->head = null;
    queue->rear = null;
    queue->size = 0;
    return queue;
}

void inQueue(Queue q, void *item) {
    QueueDataPtr qdp = malloc(sizeof(QueueData));
    qdp->data = memcpy(malloc(q->typeSize), item, q->typeSize);
    qdp->next = null;
    if (isQueueEmpty(q)) {
        q->head = qdp;
    } else {
        q->rear->next = qdp;
    }
    q->rear = qdp;
    q->size++;
}

void *outQueue(Queue q, void *dst) {
    if (isQueueEmpty(q)) {
        error("Queue is empty!");
    }

    memcpy(dst, q->head->data, q->typeSize);
    QueueDataPtr temp = q->head;
    q->head = q->head->next;
    free(temp->data);
    free(temp);
    q->size--;
    return dst;
}

int isQueueEmpty(Queue q) {
    return q->size < 1;
}

void resetQueueEmpty(Queue q) {
    while (q->head) {
        QueueDataPtr cur = q->head;
        q->head = q->head->next;
        free(cur->data);
        free(cur);
    }
    q->size = 0;
}

void freeQueue(Queue q) {
    resetQueueEmpty(q);
    free(q);
}


void inQueueForInt(Queue q, int item) {
    inQueue(q, &item);
}

void inQueueForLong(Queue q, long item) {
    inQueue(q, &item);
}

void inQueueForDouble(Queue q, double item) {
    inQueue(q, &item);
}

void inQueueForChar(Queue q, char item) {
    inQueue(q, &item);
}

void inQueueForPointer(Queue q, void *item) {
    inQueue(q, &item);
}


int outQueueForInt(Queue q) {
    int item;
    outQueue(q, &item);
    return item;
}

long outQueueForLong(Queue q) {
    long item;
    outQueue(q, &item);
    return item;
}

double outQueueForDouble(Queue q) {
    double item;
    outQueue(q, &item);
    return item;
}

char outQueueForChar(Queue q) {
    char item;
    outQueue(q, &item);
    return item;
}

void *outQueueForPointer(Queue q) {
    void *item;
    outQueue(q, &item);
    return item;
}