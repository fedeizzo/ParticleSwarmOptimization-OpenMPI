#include "pso.h"
#include "../../include/config.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void updateGlobal(ArrayList particles, Solution globalBest);
void printSolution(Solution sol);

void printSolution(Solution sol) { printArrayList(sol->pos, printDouble); }

void updateGlobal(ArrayList particles, Solution globalBest) {
  int i;
  for (i = 0; i < getNumberElements(particles); i++) { // n particle
    // Update the solution
    if (((Particle)getDataAtIndex(particles, i))->fitness >
        globalBest->fitness) {
      copyDoubleArray(((Particle)getDataAtIndex(particles, i))->pos,
                      globalBest->pos, free);
      globalBest->fitness = ((Particle)getDataAtIndex(particles, i))->fitness;
    }
  }

  for (i = 0; i < getNumberElements(particles); i++) { // n particle
    copyDoubleArray(globalBest->pos,
                    ((Particle)getDataAtIndex(particles, i))->socialBest, free);
    ((Particle)getDataAtIndex(particles, i))->currentBestSocialFitness =
        globalBest->fitness;
  }
  printf("\nCurrent Global best: [fitness: %f] \n", globalBest->fitness);
  printSolution(globalBest);
  printf("\n");
}

bool initParticles(ArrayList particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(ArrayList)) {
  int i;
  int rc;
  Particle particle;
  for (i = 0; i < n; i++) {
    particle = newParticle(dim, max, min, v_max, v_min, fitnessFunction);
    rc = checkAllocationError(particle);
    if (rc == FAILURE) {
      return false;
    }
    push_back(particles, particle);
  }
  return true;
}

void particleSwarmOptimization(ArrayList particles, int n_iterations, double w,
                               double phi_1, double phi_2,
                               double (*fitnessFunction)(ArrayList)) {
  printf("Initial population\n");
  printArrayList(particles, printParticle);
  int _;
  int i;
  Solution globalBestSolution;
  globalBestSolution = (Solution)malloc(sizeof(struct solution_t));
  globalBestSolution->fitness =
      ((Particle)getDataAtIndex(particles, 0))->fitness;
  globalBestSolution->pos = newArrayList();
  copyDoubleArray(((Particle)getDataAtIndex(particles, 0))->pos,
                  globalBestSolution->pos, free);
  for (_ = 0; _ < n_iterations; _++) { // n iterations
    printf("\nIteration [%d]:\n", _);
    updateGlobal(particles, globalBestSolution);
    for (i = 0; i < getNumberElements(particles); i++) { // n particles
      updateVelocity(((Particle)getDataAtIndex(particles, i)), w, phi_1, phi_2);
      updatePosition(((Particle)getDataAtIndex(particles, i)), fitnessFunction);
    }
    printf("Current Population\n");
    printArrayList(particles, printParticle);
    printf("Best fitness: %f\n", globalBestSolution->fitness);
  }
}
