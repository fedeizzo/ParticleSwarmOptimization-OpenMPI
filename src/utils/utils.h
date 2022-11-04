#ifndef __UTILS_H__
#define __UTILS_H__

#include "../../include/config.h"
#include "../log/log.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>


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
void printDouble(void *ptr);
void printDoubleInLine(void *ptr);
void die(char *msg);
double randfrom(double min, double max);
#endif
