#include "solution.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

Solution newSolution(int problemDimension) {
  Solution solution = (Solution)malloc(sizeof(struct solution_t));
  solution->dimension = problemDimension;
  return solution;
}

void destroySolution(Solution solution) { free(solution); }

Solution cloneSolution(Solution solution) {
  Solution new = (Solution)malloc(sizeof(struct solution_t));
  for (int i = 0; i < solution->dimension; i++)
    new->pos[i] = solution->pos[i];

  new->fitness = solution->fitness;
  new->dimension = solution->dimension;
  return new;
}

void printSolution(Solution solution) {
  printf("\tPos: [");
  for (int i = 0; i < solution->dimension; i++) {
    if (i == solution->dimension - 1) {
      printf("%f]\n", solution->pos[i]);
    } else {
      printf("%f, ", solution->pos[i]);
    }
  }
  printf("\n\tFitness value: %f\n", solution->fitness);
}
