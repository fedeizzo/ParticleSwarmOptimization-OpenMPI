/**
 * @file      particle.h
 * @brief     Particle definition
 * @date      Fri Nov  4 12:32:35 2022
 * @author    Federico Izzo, Samuele Bortolotti
 * @copyright MIT
 */

#ifndef __PARTICLE_H_
#define __PARTICLE_H_

#include "../solution/solution.h"
#include <stdio.h>

/**
 * @struct particle_t
 * Contains all information associated to a particle for a specific iteration
 */
typedef struct particle_t {
  int id;                /**< id of the particle */
  int dimension;         /**< dimension of the problem */
  Solution current;      /**< current solution */
  Solution personalBest; /**< personal best solution */
  Solution socialBest;   /**< social best solution */
  double *velocity;      /**< velocity of the particle */
} particle_t;

typedef particle_t *Particle;

/**
 * @brief Generates a new particle
 * @param[in] id id of the particle
 * @param[in] n dimension of the problem
 * @param[in] max max init value for random position geneneration
 * @param[in] min min init vlaue for random position generation
 * @param[in] v_max max init value for random velocity generation
 * @param[in] v_min min init vlaue for random velocity generation
 * @param[in] fitnessFunction A pointer to function
 * @return returns the created particle
 */
Particle newParticle(int id, int n, double max, double min, double v_max,
                     double v_min, double (*fitnessFunction)(double *, int));

/**
 * @brief Updates velocity of the given particle
 * @param[in] particle particle to update
 * @param[in] w inertia used to weight the impact of the velocity
 * @param[in] phi_1 hyperparameter used to weight the impact of the personal best
 * @param[in] phi_2 hyperparameter used to weight the impact of the social best
 */
void updateVelocity(Particle particle, double w, double phi_1, double phi_2);

/**
 * @brief Updates position of the given particle
 * @param[in] particle particle to update
 * @param[in] fitnessFunction function used to compute the fitnesss
 * @param[in] fitnessChecker function used to decide if a better fitness is found
 */
void updatePosition(Particle particle, double (*fitnessFunction)(double *, int),
                    bool (*fitnessChecker)(double, double));

/**
 * @brief Destroy a particle
 * @param[in] ptr the particle to destroy
 */
void destroyParticle(void *ptr);
#endif
