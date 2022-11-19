/**
 * @file      utils.h
 * @brief     Util functions definition
 * @date      Sat Nov 19 15:13:05 2022
 * @author    Federico Izzo, Sameuele Bortolotti
 * @copyright MIT
 */

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
 * @brief Random double generator
 * @param[in] min minimum value
 * @param[in] max maximum value
 * @return returns the double generated number
 */
double randfrom(double min, double max);
#endif
