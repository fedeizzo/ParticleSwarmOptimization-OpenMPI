#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib.h>
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
bool copyDoubleArray(GArray *source, GArray *destination);

#endif
