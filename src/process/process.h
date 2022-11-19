#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "../database/database.h"
#include "../log/log.h"
#include "../message/message.h"
#include "../particle/particle.h"
#include "../pso/pso.h"
#include "../sort/sort.h"
#include <math.h>
#include <omp.h>
#include <stdlib.h>

typedef struct process_particle_t {
  int totalNumberOfParticles;
  Particle particle;
  broadcastMessage_t *inputBuffer;
  broadcastMessage_t outputBuffer;
  bool isMessageReady;
  omp_lock_t inputBufferLock;
  omp_lock_t outputBufferLock;
} process_particle_t;

typedef process_particle_t *ProcessParticle;

void processRoutine(const int numberOfProcesses, const int numberOfThreads,
                    const int processId, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData, const char *databasePath);
#endif
