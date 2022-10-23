#ifndef __UTILS_H__
#define __UTILS_H__

#include "../arraylist/arraylist.h"
#include <stdbool.h>

/**
 * Wrap fucntion to check correct allocation with malloc
 *
 * args:
 *    void *ptr: pointer to check
 *
 * returns:
 *    0 in case of success, otherwise -1
 */
int checkAllocationError(void *ptr);
bool copyDoubleArray(ArrayList source, ArrayList destination,
                     void deleteData(void *));
void printDouble(void *ptr);
void printDoubleInLine(void *ptr);

#endif
