#ifndef __PSO_H__
#define __PSO_H__

#include "../particle/particle.h"
#include <stdbool.h>

typedef struct solution_t {
  double fitness;
  ArrayList pos;
} solution_t;

typedef solution_t *Solution;

void particleSwarmOptimization(ArrayList particles, int n_iterations, double w,
                               double phi_1, double phi_2,
                               double (*fitnessFunction)(ArrayList));

bool initParticles(ArrayList particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(ArrayList));
#endif
