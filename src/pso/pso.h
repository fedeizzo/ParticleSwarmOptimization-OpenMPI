/**
 * @file      pso.h
 * @brief     Serial Particle Swarm Optimization definition
 * @date      Sat Nov 19 14:48:08 2022
 * @author    Federico Izzo, Samuele Bortolotti
 * @copyright MIT
 */

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

/**
 * @struct pso_data_t
 * Contains PSO data
 */
typedef struct {
  int problemDimension;                                /**< problem dimension */
  int particlesNumber;                                 /**< number of particles */
  int iterationsNumber;                                /**< number of iterations */
  int neighborhoodPopulation;                          /**< neighboord population */
  double w;                                            /**< inertia for update velocity */
  double phi_1;                                        /**< weight of the personal best for the update velocity */
  double phi_2;                                        /**< weight of the social best for the update velocity */
  double initMaxPosition;                              /**< initial max position for random generator */
  double initMinPosition;                              /**< initial min position for random generator */
  double initMaxVelocity;                              /**< initial max velocity for random generator */
  double initMinVelocity;                              /**< initial max velocity for random generator */
  char *fitnessFunctionName;                           /**< fitness function name */
  char *distanceFunctionName;                          /**< distance function name */
  char *fitnessCheckerName;                            /**< fitness goal name */
  double (*fitnessFunction)(double *, int);            /**< pointer to the fitness function */
  double (*distanceFunction)(double *, double *, int); /**< pointer to the distance function */
  bool (*fitnessChecker)(double, double);              /**< pointher to the fitnss goal function */
} pso_data_t;

typedef pso_data_t *PSOData;

/**
 * @brief Creates a new PSO problem
 * @param[in] problemDimension problem dimension
 * @param[in] particlesNumber number of particles
 * @param[in] iterationsNumber number of iterations
 * @param[in] neighborhoodPopulation neighboord population
 * @param[in] w inertia for update velocity
 * @param[in] phi_1 weight of the personal best for the update velocity
 * @param[in] phi_2 weight of the social best for the update velocity
 * @param[in] initMaxPosition initial max position for random generator
 * @param[in] initMinPosition initial min position for random generator
 * @param[in] initMaxVelocity initial max velocity for random generator
 * @param[in] initMinVelocity initial max velocity for random generator
 * @param[in] fitnessFunction fitness function
 * @param[in] distanceFunction distance function
 * @param[in] fitnessChecker fitness goal
 * @return returns the created PSOData
 */
PSOData newPSOData(const int problemDimension, const int particlesNumber,
                   const int iterationsNumber, const int neighborhoodPopulation,
                   const double w, const double phi_1, const double phi_2,
                   const double initMaxPosition, const double initMinPosition,
                   const double initMaxVelocity, const double initMinVelocity,
                   double (*fitnessFunction)(double *, int),
                   double (*distanceFunction)(double *, double *, int),
                   bool (*fitnessChecker)(double, double));

/**
 * @brief Loads PSO data form file
 * @param[in] path filepath containing the PSO definition
 * @return returns the created PSOData
 */
PSOData newPSODataFromFile(const char *path);

/**
 * @brief Destories the passed PSO data
 * @param[in] psoData PSOData to destroy
 */
void destroyPSOData(PSOData psoData);

/**
 * @brief Particle Swarm Optimization algorithm
 * @param[inout] particles array of particles
 * @param[in] psoData algorithm data
 * @param[in] databasePath path to the database (optional)
 */
void particleSwarmOptimization(Particle *particles, PSOData psoData, const char *databasePath);

/**
 * @brief Initializes an array of particles to solve the PSO problem
 * @param[inout] particles array of particles
 * @param[in] particlesNumber number of particles
 * @param[in] psoData PSO data
 * @param[in] startingId particle id
 */
void initParticles(Particle *particles, const int particlesNumber, PSOData psoData, const int startingId);
#endif
