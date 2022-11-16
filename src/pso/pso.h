#ifndef __PSO_H__
#define __PSO_H__

#include "../../include/config.h"
#include "../ini/ini.h"
#include "../log/log.h"
#include "../particle/particle.h"
#include "../problems/problems.h"
#include "../solution/solution.h"
#include "../utils/utils.h"
#include "../sort/sort.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
  char *fitnessFunctionName;
  char *distanceFunctionName;
  char *fitnessCheckerName;
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
PSOData newPSODataFromFile(const char *path);
void destroyPSOData(PSOData psoData);
void particleSwarmOptimization(Particle *particles, PSOData psoData, const char *databasePath);
void initParticles(Particle *particles, const int particlesNumber, PSOData psoData, const int startingId);
#endif
