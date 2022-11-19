/**
 * @file      sort.h
 * @brief     Quicksort defintion
 * @date      Sat Nov 19 15:07:09 2022
 * @author    Federico Izzo, Sameuele Bortolotti
 * @copyright MIT
 */

#ifndef __SORT_H_
#define __SORT_H_

/**
 * @brief Finds the pivot and places it in the correct position
 * @param[inout] neighborhoodIndex array to sort
 * @param[inout] distances array to sort
 * @param[in] p left and right offset
 * @param[in] r pivot position
 * @return next iteration window
 */
int partition(int *neighborhoodIndex, double *distances, int p, int r);

/**
 * @brief Sorts the passed array using quicksort algorithm
 * @param[inout] neighborhoodIndex array to sort
 * @param[inout] distances array to sort
 * @param[in] p left and right offset
 * @param[in] r pivot position
 */
void quicksort(int *neighborhoodIndex, double *distances, int p, int r);

#endif
