#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array.h"
#include "../common/global.h"

boolean isOutRange(Array arr, int index) {
    if (index > arr->size - 1 || index < 0) {
        return true;
    }

    return false;
}

boolean nonOutRange(Array arr, int index) {
    return !isOutRange(arr, index);
}

void rangeCheck(Array arr, int index) {
    if (isOutRange(arr, index)) {
        error("Array index out range!");
    }
}

boolean isFullCapacity(Array arr) {
    return arr->size == arr->capacity;
}

void expand(Array arr) {
    int newCapacity = arr->capacity * ARRAY_EXPAND_INTENSITY + 1;

    void *newData = malloc(newCapacity * arr->typeSize);
    memcpy(newData, arr->data, arr->size * arr->typeSize);
    free(arr->data);
    arr->data = newData;
    arr->capacity = newCapacity;
}

Array newArray(int typeSize, int initialCapacity) {
    if (initialCapacity <= 0) {
        error("initialCapacity must be greater than 0!");
    }
    Array arr = malloc(sizeof(struct ArrayStructure));
    arr->data = malloc(initialCapacity * typeSize);
    arr->typeSize = typeSize;
    arr->capacity = initialCapacity;
    arr->size = 0;
    return arr;
}

Array newArrayDefault(int typeSize) {
    return newArray(typeSize, ARRAY_DEFAULT_CAPACITY);
}

void addArray(Array arr, void *item) {

    if (isFullCapacity(arr)) {
        expand(arr);
    }

    memcpy(arr->data + arr->size * arr->typeSize, item, arr->typeSize);
    arr->size++;
}

void insertArray(Array arr, int index, void *item) {
    rangeCheck(arr, index);

    if (isFullCapacity(arr)) {
        expand(arr);
    }

    for (int i = arr->size; i > index; i--) {
        memcpy(arr->data + i * arr->typeSize, arr->data + (i - 1) * arr->typeSize, arr->typeSize);
    }
    memcpy(arr->data + index * arr->typeSize, item, arr->typeSize);
    arr->size++;
}

void removeArray(Array arr, int index) {
    rangeCheck(arr, index);

    for (int i = index + 1; i < arr->size; i++) {
        memcpy(arr->data + (i - 1) * arr->typeSize, arr->data + i * arr->typeSize, arr->typeSize);
    }
    arr->size--;
}

void *getArray(Array arr, int index, void *dst) {
    rangeCheck(arr, index);

    memcpy(dst, arr->data + index * arr->typeSize, arr->typeSize);

    return dst;
}

void resetArrayEmpty(Array arr) {
    arr->size = 0;
}

void freeArray(Array arr) {
    free(arr->data);
    free(arr);
}

void addArrayForInt(Array arr, int item) {
    addArray(arr, &item);
}

void addArrayForLong(Array arr, long item) {
    addArray(arr, &item);
}

void addArrayForDouble(Array arr, double item) {
    addArray(arr, &item);
}

void addArrayForChar(Array arr, char item) {
    addArray(arr, &item);
}

void addArrayForPointer(Array arr, void *item) {
    addArray(arr, &item);
}

void insertArrayForInt(Array arr, int index, int item) {
    insertArray(arr, index, &item);
}

void insertArrayForLong(Array arr, int index, long item) {
    insertArray(arr, index, &item);
}

void insertArrayForDouble(Array arr, int index, double item) {
    insertArray(arr, index, &item);
}

void insertArrayForChar(Array arr, int index, char item) {
    insertArray(arr, index, &item);
}

void insertArrayForPointer(Array arr, int index, void *item) {
    insertArray(arr, index, &item);
}

int getArrayForInt(Array arr, int index) {
    int item;
    getArray(arr, index, &item);
    return item;
}

long getArrayForLong(Array arr, int index) {
    long item;
    getArray(arr, index, &item);
    return item;
}

double getArrayForDouble(Array arr, int index) {
    double item;
    getArray(arr, index, &item);
    return item;
}

char getArrayForChar(Array arr, int index) {
    char item;
    getArray(arr, index, &item);
    return item;
}

void *getArrayForPointer(Array arr, int index) {
    void *item;
    getArray(arr, index, &item);
    return item;
}