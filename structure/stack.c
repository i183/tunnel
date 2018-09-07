#include "stack.h"
#include "../common/global.h"
#include <stdlib.h>
#include <string.h>

static void resetNull(void *dst, int size) {
    char *b = dst;
    for (int i = 0; i < size; ++i) {
        b[i] = 0;
    }
}

Stack newStack(int typeSize) {
    Stack stack = malloc(sizeof(struct StackStructure));
    stack->typeSize = typeSize;
    stack->head = null;
    stack->size = 0;
    return stack;
}

void inStack(Stack sp, void *item) {
    StackDataPtr sdp = malloc(sizeof(StackData));
    sdp->data = memcpy(malloc(sp->typeSize), item, sp->typeSize);
    sdp->next = sp->head;
    sp->head = sdp;
    sp->size++;
}

void *outStack(Stack sp, void *dst) {
    if (isStackEmpty(sp)) {
        error("Stack is empty!");
    }

    memcpy(dst, sp->head->data, sp->typeSize);
    StackDataPtr temp = sp->head;
    sp->head = sp->head->next;
    free(temp->data);
    free(temp);
    sp->size--;
    return dst;
}

int isStackEmpty(Stack sp) {
    return sp->size < 1;
}

void resetStackEmpty(Stack sp) {
    while (sp->head) {
        StackDataPtr cur = sp->head;
        sp->head = sp->head->next;
        free(cur->data);
        free(cur);
    }
    sp->size = 0;
}

void freeStack(Stack s) {
    resetStackEmpty(s);
    free(s);
}


void inStackForInt(Stack sp, int item) {
    inStack(sp, &item);
}

void inStackForLong(Stack sp, long item) {
    inStack(sp, &item);
}

void inStackForDouble(Stack sp, double item) {
    inStack(sp, &item);
}

void inStackForChar(Stack sp, char item) {
    inStack(sp, &item);
}


int outStackForInt(Stack s) {
    int item;
    outStack(s, &item);
    return item;
}

long outStackForLong(Stack s) {
    long item;
    outStack(s, &item);
    return item;
}

double outStackForDouble(Stack s) {
    double item;
    outStack(s, &item);
    return item;
}

char outStackForChar(Stack s) {
    char item;
    outStack(s, &item);
    return item;
}