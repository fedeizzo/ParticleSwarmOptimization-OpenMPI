#include "particle.h"
#include "../../include/config.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void randomArrayInitialization(double *array, int n, double min, double max);
void updateFitness(Particle particle, double (*fitnessFunction)(double *, int));

void randomArrayInitialization(double *array, int n, double min, double max) {
  for (int i = 0; i < n; i++)
    array[i] = randfrom(min, max);
}

void destroyParticle(void *ptr) {
  Particle particle = (Particle)ptr;
  destroySolution(particle->current);
  destroySolution(particle->personalBest);
  destroySolution(particle->socialBest);
  free(particle->velocity);
  free(particle);
}

Particle newParticle(int id, int problemDimension, double max, double min,
                     double v_max, double v_min,
                     double (*fitnessFunction)(double *, int)) {
  Particle particle = NULL;
  particle = (Particle)malloc(sizeof(struct particle_t));
  particle->id = id;
  particle->dimension = problemDimension;
  particle->current = newSolution(problemDimension);
  randomArrayInitialization(particle->current->pos, problemDimension, min, max);
  particle->velocity = (double *)malloc(problemDimension * sizeof(double));
  randomArrayInitialization(particle->velocity, problemDimension, v_min / 3,
                            v_max / 3);
  updateFitness(particle, fitnessFunction);

  particle->personalBest = cloneSolution(particle->current);
  particle->socialBest = cloneSolution(particle->current);
  return particle;
}

void updateVelocity(Particle particle, double w, double phi_1, double phi_2) {
/* #pragma omp parallel for */
  for (int i = 0; i < particle->dimension; i++) {
    double v = particle->velocity[i];
    double pbp = particle->personalBest->pos[i];
    double sbp = particle->socialBest->pos[i];
    double cp = particle->current->pos[i];

    v = v * w;
    v += phi_1 * randfrom(0.0, 1.0) * (pbp - cp);
    v += phi_2 * randfrom(0.0, 1.0) * (sbp - cp);
    particle->velocity[i] = v;
  }
}

void updatePosition(Particle particle, double (*fitnessFunction)(double *, int),
                    bool (*fitnessChecker)(double, double)) {
  double oldFitness;
/* #pragma omp parallel for */
  for (int i = 0; i < particle->dimension; i++)
    particle->current->pos[i] += particle->velocity[i];

  oldFitness = particle->personalBest->fitness;
  updateFitness(particle, fitnessFunction);
  if (fitnessChecker(particle->current->fitness, oldFitness)) {
    destroySolution(particle->personalBest);
    particle->personalBest = cloneSolution(particle->current);
  }
}

void updateFitness(Particle particle,
                   double (*fitnessFunction)(double *, int)) {
  particle->current->fitness =
      fitnessFunction(particle->current->pos, particle->current->dimension);
}
