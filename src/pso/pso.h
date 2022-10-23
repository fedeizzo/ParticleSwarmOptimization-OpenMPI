#ifndef __PSO_H__
#define __PSO_H__

#include "../particle/particle.h"
#include "../solution/solution.h"
#include <stdbool.h>

void particleSwarmOptimization(ArrayList particles, int dimension, int n_iterations, double w,
                               double phi_1, double phi_2,
                               double (*fitnessFunction)(ArrayList));

bool initParticles(ArrayList particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(ArrayList));
#endif
