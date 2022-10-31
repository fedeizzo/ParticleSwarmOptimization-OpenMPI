#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "../log/log.h"
#include "../message/message.h"
#include "../particle/particle.h"
#include "../pso/pso.h"
#include "../list/list.h"
#include <math.h>
#include <omp.h>
#include <stdlib.h>

typedef struct process_particle_t {
  int totalNumberOfParticles;
  Particle particle;
  BroadcastMessage *inputBuffer;
  BroadcastMessage outputBuffer;
  bool isMessageReady;
  omp_lock_t inputBufferLock;
  omp_lock_t outputBufferLock;
} process_particle_t;

typedef process_particle_t *ProcessParticle;

void processRoutine(const int numberOfProcesses, const int numberOfThreads,
                    const int processId, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData);
#endif
