#ifndef __PARTICLE_H_
#define __PARTICLE_H_

#include <glib.h>

/**
 *
 */
typedef struct Particle {
  GArray *pos;
  GArray *velocity;
  GArray *personalBest;
  GArray *socialBest;
  double fitness;
  double currentBestPersonalFitness;
  double currentBestSocialFitness;
} * Particle;

/**
 *
 */
Particle newParticle(int n, double max, double min, double v_max, double v_min,
                     double (*fitnessFunction)(GArray *));

/**
 * Update velocity function
 */
void updateVelocity(Particle particle, double w, double phi_1, double phi_2);

void updatePosition(Particle particle, double (*fitnessFunction)(GArray *));

void updateFitness(Particle particle, double (*fitnessFunction)(GArray *));

double getFitness(Particle particle);

void destroyParticle(Particle particle);

GArray *getPosition(Particle particle);

GArray *getPersonalBest(Particle particle);

GArray *getSocialBest(Particle particle);

GArray *getVelocity(Particle particle);

double getPersonalBestFitness(Particle particle);

double getSocialBestFitness(Particle particle);

#endif
