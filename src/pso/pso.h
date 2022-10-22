#ifndef __PSO_H__
#define __PSO_H__

#include "../particle/particle.h"
#include <stdbool.h>

typedef struct Solution {
  double fitness;
  GArray *pos;
} * Solution;

bool initParticles(GArray *particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(GArray *));

void particleSwarmOptimization(GArray *particles, int n_iterations, double w,
                               double phi_1, double phi_2,
                               double (*fitnessFunction)(GArray *));

#endif
