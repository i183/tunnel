//
// Created by maqian on 2018/6/15.
//

#ifndef LEARN_C_ARRAY_H
#define LEARN_C_ARRAY_H

#define ARRAY_DEFAULT_CAPACITY 10
#define ARRAY_EXPAND_INTENSITY 1.6

typedef struct ArrayStructure {

    void *data;

    int typeSize;

    int size;

    int capacity;

} *Array;

/**
 * 创建Array
 * @param typeSize
 * @param initialCapacity
 * @return
 */
Array newArray(int typeSize, int initialCapacity);

/**
 * 创建Array 默认大小
 * @param typeSize
 * @return
 */
Array newArrayDefault(int typeSize);

/**
 * 添加元素
 * @param arr
 * @param item
 */
void addArray(Array arr, void *item);

/**
 * 插入元素到指定位置
 * @param arr
 * @param index
 * @param item
 */
void insertArray(Array arr, int index, void *item);

/**
 * 删除元素
 * @param arr
 * @param index
 */
void removeArray(Array arr, int index);

/**
 * 获取元素
 * @param arr
 * @param index
 * @param dst
 * @return
 */
void *getArray(Array arr, int index, void *dst);

/**
 * 置空
 * @param arr
 */
void resetArrayEmpty(Array arr);

/**
 * 销毁Array
 * @param arr
 */
void freeArray(Array arr);

/**
 * 插入Int
 * @param arr
 * @param item
 */
void addArrayForInt(Array arr, int item);

/**
 * 插入Long
 * @param arr
 * @param item
 */
void addArrayForLong(Array arr, long item);

/**
 * 插入Double
 * @param arr
 * @param item
 */
void addArrayForDouble(Array arr, double item);

/**
 * 插入Char
 * @param arr
 * @param item
 */
void addArrayForChar(Array arr, char item);

/**
 * 指定位置插入Int
 * @param arr
 * @param item
 */
void insertArrayForInt(Array arr, int index, int item);

/**
 * 指定位置插入Long
 * @param arr
 * @param item
 */
void insertArrayForLong(Array arr, int index, long item);

/**
 * 指定位置插入Double
 * @param arr
 * @param item
 */
void insertArrayForDouble(Array arr, int index, double item);

/**
 * 指定位置插入Char
 * @param arr
 * @param item
 */
void insertArrayForChar(Array arr, int index, char item);

/**
 * 获取Int数据
 * @param arr
 * @param index
 */
int getArrayForInt(Array arr, int index);

/**
 * 获取Long数据
 * @param arr
 * @param index
 */
long getArrayForLong(Array arr, int index);

/**
 * 获取Double数据
 * @param arr
 * @param index
 */
double getArrayForDouble(Array arr, int index);

/**
 * 获取Char数据
 * @param arr
 * @param index
 */
char getArrayForChar(Array arr, int index);

#endif //LEARN_C_ARRAY_H
