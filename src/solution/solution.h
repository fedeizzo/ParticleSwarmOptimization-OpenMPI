#ifndef __SOLUTION_H__
#define __SOLUTION_H__
#include "../arraylist/arraylist.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct solution_t {
  int dimension;
  double fitness;
  double *pos;
} solution_t;

typedef solution_t *Solution;

Solution newSolution();
void destroySolution();
Solution cloneSolution(Solution solution);
void printSolution(Solution solution);

#endif
