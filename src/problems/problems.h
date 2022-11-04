#ifndef __PROBLEMS_H__
#define __PROBLEMS_H__

#include <math.h>
#include <stdbool.h>

double sphere(double *array, int dimensionNumber);
double wave(double *array, int dimensionNumber);
double euclideanDistance(double *p1, double *p2, int dimensionNumber);
bool maximize(double d1, double d2);
bool minimize(double d1, double d2);
#endif
