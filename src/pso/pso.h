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
  bool (*fitnessChecker)(double, double);
} pso_data_t;

typedef pso_data_t *PSOData;

PSOData newPSOData(const int problemDimension, const int particlesNumber,
                   const int iterationsNumber, const int neighborhoodPopulation,
                   const double w, const double phi_1, const double phi_2,
                   const double initMaxPosition, const double initMinPosition,
                   const double initMaxVelocity, const double initMinVelocity,
                   double (*fitnessFunction)(double *, int),
                   double (*distanceFunction)(double *, double *, int),
                   bool (*fitnessChecker)(double, double));

void destroyPSOData(PSOData psoData);
void particleSwarmOptimization(Particle *particles, PSOData psoData);
bool initParticles(Particle *particles, PSOData psoData);
#endif
