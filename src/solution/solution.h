#ifndef __SOLUTION_H__
#define __SOLUTION_H__
#include "../arraylist/arraylist.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct solution_t {
  double fitness;
  ArrayList pos;
} solution_t;

typedef solution_t *Solution;

Solution newSolution();
void destroySolution(void *ptr);
Solution cloneSolution(Solution solution);
void printSolution(Solution solution);
void jsonSolution(Solution solution, FILE *fp);

#endif
