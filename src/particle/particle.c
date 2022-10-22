#include "particle.h"
#include "../../include/config.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * HEADERS
 */
double randfrom(double min, double max);
bool randomArrayInitialization(ArrayList array, int n, double min, double max);

/**
 * IMPLEMENTATIONS
 */

void printParticle(void *data) {
  Particle particle = (Particle)data;
  printf("Particle: \n");
  printArrayList(particle->pos, printDouble);
}

double randfrom(double min, double max) {
  double range = (max - min);
  double div = RAND_MAX / range;
  return min + (rand() / div);
}

bool randomArrayInitialization(ArrayList array, int n, double min, double max) {
  int i;
  double *data;
  int rc;
  for (i = 0; i < n; i++) {
    data = (double *)malloc(sizeof(double));
    rc = checkAllocationError(data);
    if (rc == FAILURE) {
      return false;
    }
    *data = randfrom(min, max);
    push_back(array, data);
  }
  return true;
}

void destroyParticle(void *ptr) {
  Particle particle = (Particle)ptr;
  destroyArrayList(particle->pos, free);
  destroyArrayList(particle->personalBest, free);
  destroyArrayList(particle->socialBest, free);
  destroyArrayList(particle->velocity, free);
  free(particle);
}

/**
 *
 */
Particle newParticle(int n, double max, double min, double v_max, double v_min,
                     double (*fitnessFunction)(ArrayList)) {
  Particle particle = NULL;
  particle = (Particle)malloc(sizeof(struct particle_t));
  int rc = checkAllocationError(particle);
  if (rc == SUCCESS) {
    particle->pos = newArrayList();
    randomArrayInitialization(particle->pos, n, min, max);

    particle->personalBest = newArrayList();
    copyDoubleArray(particle->pos, particle->personalBest, free);

    particle->socialBest = newArrayList();
    copyDoubleArray(particle->pos, particle->socialBest, free);

    particle->velocity = newArrayList();
    randomArrayInitialization(particle->velocity, n, v_min / 3, v_max / 3);

    updateFitness(particle, fitnessFunction);
    particle->currentBestPersonalFitness = particle->fitness;
    particle->currentBestSocialFitness = particle->fitness;
  }
  return particle;
}

/**
 * Update velocity function
 */
void updateVelocity(Particle particle, double w, double phi_1, double phi_2) {
  int i;
  double *data;
  for (i = 0; i < getNumberElements(particle->velocity); i++) {
    data = (double *)getDataAtIndex(particle->velocity, i);
    *data = (*data) * w; // Times inertia
    (*data) += phi_1 * randfrom(0.0, 1.0) *
                   (*(double *)getDataAtIndex(particle->personalBest, i)) -
               (*(double *)getDataAtIndex(particle->pos, i)); // Social rate
    *(data) += phi_2 * randfrom(0.0, 1.0) *
                   (*(double *)getDataAtIndex(particle->socialBest, i)) -
               (*(double *)getDataAtIndex(particle->pos, i)); // Global rate
  }
}

void updatePosition(Particle particle, double (*fitnessFunction)(ArrayList)) {
  int i;
  double oldFitness;
  double *data;
  for (i = 0; i < getNumberElements(particle->pos); i++) {
    data = (double *)getDataAtIndex(particle->pos, i);
    *(data) += (*(double *)getDataAtIndex(particle->velocity, i));
  }
  // Update personal best
  oldFitness = particle->fitness;
  updateFitness(particle, fitnessFunction);
  if (particle->fitness > oldFitness) {
    particle->currentBestPersonalFitness = particle->fitness;
    copyDoubleArray(particle->pos, particle->personalBest, free);
  }
}

void updateFitness(Particle particle,
                   double (*fitnessFunction)(ArrayList value)) {
  particle->fitness = fitnessFunction(particle->pos);
}

double getFitness(Particle particle) { return particle->fitness; }
ArrayList getPosition(Particle particle) { return particle->pos; }
ArrayList getPersonalBest(Particle particle) { return particle->personalBest; }
ArrayList getSocialBest(Particle particle) { return particle->socialBest; }
ArrayList getVelocity(Particle particle) { return particle->velocity; }
double getPersonalBestFitness(Particle particle) {
  return particle->currentBestPersonalFitness;
}
double getSocialBestFitness(Particle particle) {
  return particle->currentBestSocialFitness;
}
