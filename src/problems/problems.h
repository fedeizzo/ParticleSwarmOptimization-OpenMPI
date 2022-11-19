/**
 * @file      problems.h
 * @brief     List of problems definition
 * @date      Sat Nov 19 14:43:17 2022
 * @author    Federico Izzo, Samuele Bortolotti
 * @copyright MIT
 */

#ifndef __PROBLEMS_H__
#define __PROBLEMS_H__

#include <math.h>
#include <stdbool.h>

double sphere(double *array, int dimensionNumber);
double wave(double *array, int dimensionNumber);
double euclideanDistance(double *p1, double *p2, int dimensionNumber);
double Easom(double *array, int dimensionNumber);
double Ackley(double *array, int dimensionNumber);
double Himmelblau(double *array, int dimensionNumber);
double HolderTable(double *array, int dimensionNumber);
bool maximize(double d1, double d2);
bool minimize(double d1, double d2);
#endif
