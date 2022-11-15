#include "./problems.h"
#define PI 3.141592654
#define E 2.71828

double sphere(double *array, int dimensionNumber) {
  double squared_sum = 0;
  int i;
  for (i = 0; i < dimensionNumber; i++)
    squared_sum += array[i] * array[i];

  return squared_sum;
}

double wave(double *array, int dimensionNumber) {
  double squared_sum = 0;
  int i;
  for (i = 0; i < dimensionNumber; i++)
    if (i == 0)
      squared_sum += array[i] * array[i] * array[i];
    else
      squared_sum += array[i] * array[i];

  return squared_sum;
}

double Easom(double *array, int dimensionNumber) {
  double x = array[0];
  double y = array[1];
  return -cos(x) * cos(y) * exp(-((x - PI) * (x - PI) + (y - PI) * (y - PI)));
}

double Ackley(double *array, int dimensionNumber) {
  double x = array[0];
  double y = array[1];
  return -20.0 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) -
         exp(0.5 * (cos(2 * PI * x) + cos(2 * PI * y))) + E + 20;
}

double Himmelblau(double *array, int dimensionNumber) {
  double x = array[0];
  double y = array[1];
  return (x * x + y - 11) * (x * x + y - 11) +
         (x + y * y - 7) * (x + y * y - 7);
}

double HolderTable(double *array, int dimensionNumber) {
  double x = array[0];
  double y = array[1];
  return -fabs(sin(x) * cos(y) * exp(fabs(1 - (sqrt(x * x + y * y) / PI))));
}

double euclideanDistance(double *p1, double *p2, int dimensionNumber) {
  double res = 0;
  for (int dim = 0; dim < dimensionNumber; dim++)
    res += (p1[dim] - p2[dim]) * (p1[dim] - p2[dim]);
  return sqrt(res);
}

bool maximize(double d1, double d2) { return d1 > d2; }

bool minimize(double d1, double d2) { return d1 < d2; }
