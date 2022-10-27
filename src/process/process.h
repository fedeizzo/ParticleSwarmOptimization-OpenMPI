#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "../particle/particle.h"
#include "../message/message.h"
#include "../log/log.h"
#include <math.h>
#include <omp.h>
#include <stdlib.h>

typedef struct process_particle_t {
  int totalNumberOfParticles;
  Particle particle;
  BroadcastMessage **inputBuffer;
  BroadcastMessage outputBuffer;
  bool isMessageReady;
  omp_lock_t inputBufferLock;
} process_particle_t;

typedef process_particle_t *ProcessParticle;
#endif
