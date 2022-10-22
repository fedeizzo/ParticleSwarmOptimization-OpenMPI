#include "particle.h"
#include "../../include/config.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdlib.h>

/*
 * HEADERS
 */
double randfrom(double min, double max);
bool randomArrayInitialization(GArray *array, int n, double min, double max);

/**
 * IMPLEMENTATIONS
 */

double randfrom(double min, double max) {
  double range = (max - min);
  double div = RAND_MAX / range;
  return min + (rand() / div);
}

bool randomArrayInitialization(GArray *array, int n, double min, double max) {
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
    g_array_append_val(array, data);
  }
  return true;
}

/**
 *
 */
Particle newParticle(int n, double max, double min, double v_max, double v_min,
                     double (*fitnessFunction)(GArray *)) {
  Particle particle = NULL;
  particle = (Particle)malloc(sizeof(struct Particle));
  int rc = checkAllocationError(particle);
  if (rc == SUCCESS) {
    particle->pos = g_array_new(false, false, sizeof(double));
    randomArrayInitialization(particle->pos, n, min, max);

    particle->personalBest = g_array_new(false, false, sizeof(double));
    copyDoubleArray(particle->pos, particle->personalBest);

    particle->socialBest = g_array_new(false, false, sizeof(double));
    copyDoubleArray(particle->pos, particle->socialBest);

    particle->velocity = g_array_new(false, false, sizeof(double));
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
  for (i = 0; i < particle->velocity->len; i++) {
    particle->velocity->data[i] =
        particle->velocity->data[i] * w; // Times inertia
    particle->velocity->data[i] += phi_1 * randfrom(0.0, 1.0) *
                                   (particle->personalBest->data[i] -
                                    particle->pos->data[i]); // Social rate
    particle->velocity->data[i] +=
        phi_2 * randfrom(0.0, 1.0) *
        (particle->socialBest->data[i] - particle->pos->data[i]); // Global rate
  }
}

void updatePosition(Particle particle, double (*fitnessFunction)(GArray *)) {
  int i;
  double oldFitness;
  for (i = 0; i < particle->pos->len; i++) {
    particle->pos->data[i] += particle->velocity->data[i];
  }
  // Update personal best
  oldFitness = particle->fitness;
  updateFitness(particle, fitnessFunction);
  if (particle->fitness > oldFitness) {
    particle->currentBestPersonalFitness = particle->fitness;
    copyDoubleArray(particle->pos, particle->personalBest);
  }
}

void updateFitness(Particle particle,
                   double (*fitnessFunction)(GArray *value)) {
  particle->fitness = fitnessFunction(particle->pos);
}

void destroyParticle(Particle particle) {
  g_array_free(particle->pos, true);
  g_array_free(particle->velocity, true);
  g_array_free(particle->personalBest, true);
  free(particle);
}

double getFitness(Particle particle) { return particle->fitness; }
GArray *getPosition(Particle particle) { return particle->pos; }
GArray *getPersonalBest(Particle particle) { return particle->personalBest; }
GArray *getSocialBest(Particle particle) { return particle->socialBest; }
GArray *getVelocity(Particle particle) { return particle->velocity; }
double getPersonalBestFitness(Particle particle) {
  return particle->currentBestPersonalFitness;
}
double getSocialBestFitness(Particle particle) {
  return particle->currentBestSocialFitness;
}
