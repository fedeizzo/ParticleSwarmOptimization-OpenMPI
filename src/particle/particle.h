#ifndef __PARTICLE_H_
#define __PARTICLE_H_

#include "../solution/solution.h"
#include <stdio.h>
/**
 *
 */
typedef struct particle_t {
  int id;
  int dimension;
  Solution current;
  Solution personalBest;
  Solution socialBest;
  double *velocity;
} particle_t;

typedef particle_t *Particle;

/**
 *
 */
Particle newParticle(int id, int n, double max, double min, double v_max, double v_min,
                     double (*fitnessFunction)(double*, int));

/**
 * Update velocity function
 */
void updateVelocity(Particle particle, double w, double phi_1, double phi_2);

void updatePosition(Particle particle, double (*fitnessFunction)(double*, int));

void updateFitness(Particle particle, double (*fitnessFunction)(double*, int));

double getFitness(Particle particle);

void destroyParticle(void *ptr);
#endif
