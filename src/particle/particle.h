#ifndef __PARTICLE_H_
#define __PARTICLE_H_

#include "../arraylist/arraylist.h"
#include "../solution/solution.h"
#include <stdio.h>
/**
 *
 */
typedef struct particle_t {
  int id;
  Solution current;
  Solution personalBest;
  Solution socialBest;
  ArrayList velocity;
} particle_t;

typedef particle_t *Particle;

/**
 *
 */
Particle newParticle(int id, int n, double max, double min, double v_max, double v_min,
                     double (*fitnessFunction)(ArrayList));

/**
 * Update velocity function
 */
void updateVelocity(Particle particle, double w, double phi_1, double phi_2);

void updatePosition(Particle particle, double (*fitnessFunction)(ArrayList));

void updateFitness(Particle particle, double (*fitnessFunction)(ArrayList));

double getFitness(Particle particle);

void destroyParticle(void *ptr);

ArrayList getPosition(Particle particle);

ArrayList getPersonalBest(Particle particle);

ArrayList getSocialBest(Particle particle);

ArrayList getVelocity(Particle particle);

double getPersonalBestFitness(Particle particle);

double getSocialBestFitness(Particle particle);

void printParticle(void *data);

void jsonParticle(Particle particle, FILE *fp);
#endif
