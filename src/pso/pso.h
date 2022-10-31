#ifndef __PSO_H__
#define __PSO_H__

#include "../particle/particle.h"
#include "../solution/solution.h"
#include <stdbool.h>

typedef struct {
  int problemDimension;
  int particlesNumber;
  int iterationsNumber;
  int neighborhoodPopulation;
  double w;
  double phi_1;
  double phi_2;
  double initMaxPosition;
  double initMinPosition;
  double initMaxVelocity;
  double initMinVelocity;
  double (*fitnessFunction)(double *, int);
  double (*distanceFunction)(double *, double *, int);
  double (*fitnessChecker)(double, double);
} pso_data_t;

typedef pso_data_t *PSOData;

PSOData newPSOData(const int problemDimension, const int particlesNumber,
                   const int iterationsNumber, const int neighborhoodPopulation,
                   const double w, const double phi_1, const double phi_2,
                   const double initMaxPosition, const double initMinPosition,
                   const double initMaxVelocity, const double initMinVelocity,
                   double (*fitnessFunction)(double *, int),
                   double (*distanceFunction)(double *, double *, int),
                   double (*fitnessChecker)(double, double));

void destroyPSOData(PSOData psoData);

void particleSwarmOptimization(ArrayList particles, int dimension,
                               int n_iterations, double w, double phi_1,
                               double phi_2,
                               double (*fitnessFunction)(double*, int));

bool initParticles(ArrayList particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(double*, int));
#endif
