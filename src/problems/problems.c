#include "./problems.h"

double sphere(double *array, int dimensionNumber) {
  double squared_sum = 0;
  int i;
  for (i = 0; i < dimensionNumber; i++)
    squared_sum += array[i] * array[i];

  return squared_sum;
}

double euclideanDistance(double *p1, double *p2, int dimensionNumber) {
  double res = 0;
  for (int dim = 0; dim < dimensionNumber; dim++)
    res += (p1[dim] - p2[dim]) * (p1[dim] - p2[dim]);
  return sqrt(res);
}

double maximize(double d1, double d2) { return d1 > d2; }

double minimize(double d1, double d2) { return d1 < d2; }
