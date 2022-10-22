#include "pso.h"
#include "../../include/config.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>

void updateGlobal(GArray *particles, Solution globalBest);

void updateGlobal(GArray *particles, Solution globalBest) {
  int i;
  for (i = 0; i < particles->len; i++) { // n particle
    // Update the solution
    if (particles->data[i]->fitness > globalBest->fitness) {
      copyDoubleArray(particles->data[i]->pos, globalBest->pos);
    }
  }

  for (i = 0; i < particles->len; i++) { // n particle
    copyDoubleArray(globalBest->pos, particles->data[i]->socialBest);
    particles->data[i]->currentBestSocialFitness = globalBest->fitness;
  }
}

bool initParticles(GArray *particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(GArray *)) {
  int i;
  int rc;
  Particle particle;
  for (i = 0; i < n; i++) {
    particle = newParticle(dim, max, min, v_max, v_min, fitnessFunction);
    rc = checkAllocationError(particle);
    if (rc == FAILURE) {
      return false;
    }
    g_array_append_val(particles, particle);
  }
  return true;
}

void particleSwarmOptimization(GArray *particles, int n_iterations, double w,
                               double phi_1, double phi_2,
                               double (*fitnessFunction)(GArray *)) {
  int _;
  int i;
  Solution globalBestSolution;
  globalBestSolution = (Solution)malloc(sizeof(struct Solution));
  globalBestSolution->fitness = 100.0; // particles->data[0]->fitness;
  globalBestSolution->pos = g_array_new(false, false, sizeof(double));
  copyDoubleArray(particles->data[0]->pos, globalBestSolution->pos);
  for (_ = 0; _ < n_iterations; _++) { // n iterations
    updateGlobal(particles, globalBestSolution);
    for (i = 0; i < particles->len; i++) { // n particles
      updateVelocity(particles->data[i], w, phi_1, phi_2);
      updatePosition(particles->data[i], fitnessFunction);
    }
    printf("%f\n", globalBestSolution->fitness);
  }
}
