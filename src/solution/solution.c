#include "solution.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

Solution newSolution() {
  Solution solution = (Solution)malloc(sizeof(struct solution_t));
  solution->pos = newArrayList();
  return solution;
}

void destroySolution(void *ptr) {
  Solution solution = (Solution)ptr;
  destroyArrayList(solution->pos, free);
  free(solution);
}

Solution cloneSolution(Solution solution) {
  ArrayList pos = solution->pos;
  double fitness = solution->fitness;

  Solution new = (Solution)malloc(sizeof(struct solution_t));
  new->pos = newArrayList();
  for (int i = 0; i < pos->used; i++) {
    double *original = (double *)getDataAtIndex(pos, i);
    double coord = *original;
    double *data = (double *)malloc(sizeof(double));
    *data = coord;
    push_back(new->pos, data);
  }

  new->fitness = fitness;
  return new;
}

void printSolution(Solution solution) {
  printf("\tPos:");
  printArrayList(solution->pos, printDoubleInLine);
  printf("\n\tFitness value: %f\n", solution->fitness);
}

void jsonSolution(Solution solution, FILE *fp) {
  fprintf(fp, "{\n");
  fprintf(fp, "\"pos\": [");
  int numberElements = getNumberElements(solution->pos);
  for (int i = 0; i < numberElements; i++)
    if (i != numberElements - 1)
      fprintf(fp, "%f, ", *((double *)getDataAtIndex(solution->pos, i)));
    else
      fprintf(fp, "%f", *((double *)getDataAtIndex(solution->pos, i)));
  fprintf(fp, "],\n");
  fprintf(fp, "\"fitness\": %f\n", solution->fitness);
  fprintf(fp, "}");
}
