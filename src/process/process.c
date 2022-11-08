#include "./process.h"

#define SLEEP_TIME 100
/* #define SEND_SLEEP_TIME 45000 */
#define SEND_SLEEP_TIME 100000
/* #define SLEEP_TIME 100000 */

void updateInnerInputBuffer(ProcessParticle *particles, ProcessParticle src,
                            const int processId, const int iteration,
                            const int numberOfParticles);
ProcessParticle newProcessParticle(const int particleId, const int processId,
                                   const int totalNumberOfParticles,
                                   const int problemDimension, const double max,
                                   const double min, const double v_max,
                                   const double v_min,
                                   double (*fitnessFunction)(double *, int)) {
  ProcessParticle processParticle =
      (ProcessParticle)malloc(sizeof(process_particle_t));
  processParticle->totalNumberOfParticles = totalNumberOfParticles;
  processParticle->particle = newParticle(particleId, problemDimension, max,
                                          min, v_max, v_min, fitnessFunction);
  processParticle->inputBuffer = (BroadcastMessage *)malloc(
      totalNumberOfParticles * sizeof(BroadcastMessage));
  for (int i = 0; i < totalNumberOfParticles; i++)
    processParticle->inputBuffer[i] = NULL;
  processParticle->outputBuffer =
      (BroadcastMessage)malloc(sizeof(BroadcastMessage));

  omp_init_lock(&processParticle->inputBufferLock);
  omp_init_lock(&processParticle->outputBufferLock);

  omp_set_lock(&processParticle->outputBufferLock);
  processParticle->outputBuffer = newBroadcastMessage();
  initalizeBroacastMessage(processParticle->outputBuffer, processId, 0,
                           particleId, processParticle->particle->current);
  omp_unset_lock(&processParticle->outputBufferLock);
  return processParticle;
}

void destroyProcessParticle(ProcessParticle particle) {
  for (int i = 0; i < particle->totalNumberOfParticles; i++)
    free(particle->inputBuffer[i]);
  free(particle->inputBuffer);
  free(particle->outputBuffer);
  destroyParticle(particle->particle);
  omp_destroy_lock(&particle->inputBufferLock);
  omp_destroy_lock(&particle->outputBufferLock);
  free(particle);
}

void processLog(const char *operation, const int iteration, const int process,
                const int thread, const char *step) {
  log_info("%-10s :: iteration %d process %d thread %d :: %s", operation,
           iteration, process, thread, step);
}

void processLogDebug(const char *operation, const int iteration,
                     const int process, const int thread, const char *step) {
  log_debug("%-10s :: iteration %d process %d thread %d :: %s", operation,
            iteration, process, thread, step);
}

typedef struct {
  BroadcastMessage msg;
  int particleId;
} temp_t;
typedef temp_t *Temp;

void saveNewMessage(ProcessParticle *particles, const int numberOfParticles,
                    BroadcastMessage msg, const int processId) {
  List waitingMsg = newList();
  for (int i = 0; i < numberOfParticles; i++) {
    ProcessParticle particle = particles[i];
    BroadcastMessage particleMsg = newBroadcastMessage();
    particleMsg = cloneMessage(msg);
    omp_set_lock(&particle->inputBufferLock);
    if (particle->inputBuffer[msg->particleId] == NULL)
      particle->inputBuffer[msg->particleId] = particleMsg;
    else {
      Temp tmp = (Temp)malloc(sizeof(temp_t));
      tmp->particleId = i;
      tmp->msg = particleMsg;
      enqueue(waitingMsg, tmp);
    }
    omp_unset_lock(&particle->inputBufferLock);
  }

  while (!isEmpty(waitingMsg)) {
    Element curr = waitingMsg->head;
    if (curr->next != NULL)
      waitingMsg->head = waitingMsg->head->next;
    else
      waitingMsg->head = NULL;
    Temp tmp = (Temp)curr->data;
    ProcessParticle particle = particles[tmp->particleId];
    BroadcastMessage particleMsg = tmp->msg;
    omp_set_lock(&particle->inputBufferLock);
    if (particle->inputBuffer[msg->particleId] == NULL) {
      particle->inputBuffer[msg->particleId] = particleMsg;
      free(curr);
    } else
      enqueue(waitingMsg, tmp);
    omp_unset_lock(&particle->inputBufferLock);
    sleep(SLEEP_TIME);
  }
}

void receiveMessage(ProcessParticle *particles, const int iterationsNumber,
                    const int numberOfParticles, const int numberOfProcesses,
                    const int processId,
                    const int *processToNumberOfParticles) {
  if (numberOfProcesses == 1)
    return;
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(particles[0]->particle->dimension);
  for (int iteration = 0; iteration < iterationsNumber; iteration++) {
    processLog("READ", iteration, processId, omp_get_thread_num(), "start");
    for (int pid = 0; pid < numberOfProcesses; pid++) {
      if (pid != processId) {
        printf("sto facendo pid %d\n", pid);
        BroadcastMessage messages[processToNumberOfParticles[pid]];
        for (int i = 0; i < processToNumberOfParticles[pid]; i++) 
	  messages[i] = newBroadcastMessage();
        MPI_Bcast(messages, processToNumberOfParticles[pid],
                  DT_BROADCAST_MESSAGE, pid, MPI_COMM_WORLD);
        printf("Ricevuto\n");
        for (int i = 0; i < processToNumberOfParticles[pid]; i++) {
          BroadcastMessage msg = messages[i];
          printf("Message particella %d\n", msg->particleId);
          /* saveNewMessage(particles, numberOfParticles, msg, processId); */
          /* destroyBroadcastMessage(msg); */
        }
        printf("Salvato\n");
        processLog("READ", iteration, processId, omp_get_thread_num(), "done");
        /* #if DEBUG_CHECKS == 1 */
        /*           if (msg->iteration != iteration) */
        /*             log_error( */
        /*                 "A mismatch in the iteration within receive
         * function occured"); */
        /* #endif */
        /* MPI_Barrier(MPI_COMM_WORLD); */
      }
    }
  }
  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}

bool checkIterationConsistency(BroadcastMessage *inputBuffer,
                               const int numberOfParticles) {
  const int iteration = inputBuffer[0]->iteration;
  for (int i = 0; i < numberOfParticles; i++)
    if (inputBuffer[i]->iteration != iteration)
      return false;
  return true;
}

void waitInputBuffer(ProcessParticle particle) {
  int isBufferFull;
  do {
    isBufferFull = 0;
    omp_set_lock(&particle->inputBufferLock);
#pragma omp parallel for reduction(+ : isBufferFull)
    for (int i = 0; i < particle->totalNumberOfParticles; i++)
      isBufferFull += particle->inputBuffer[i] != NULL ? 1 : 0;
    omp_unset_lock(&particle->inputBufferLock);
    usleep(SLEEP_TIME);
  } while (isBufferFull != particle->totalNumberOfParticles);
#if DEBUG_CHECKS == 1
  omp_set_lock(&particle->inputBufferLock);
  if (!checkIterationConsistency(particle->inputBuffer,
                                 particle->totalNumberOfParticles))
    log_error(
        "A mismatch in the iteration contained in the input buffer occured");
  omp_unset_lock(&particle->inputBufferLock);
#endif
}

void waitOutputBuffer(ProcessParticle particle) {
  bool isBufferFull = true;
  do {
    omp_set_lock(&particle->outputBufferLock);
    isBufferFull = particle->outputBuffer == NULL ? false : true;
    omp_unset_lock(&particle->outputBufferLock);
    usleep(SLEEP_TIME);
  } while (isBufferFull);
}

void computeDistances(ProcessParticle particle, int *indexes, double *distances,
                      PSOData psoData) {
  omp_set_lock(&particle->inputBufferLock);
#pragma omp parallel for
  for (int i = 0; i < particle->totalNumberOfParticles; i++) {
    indexes[i] = i;
    distances[i] =
        psoData->distanceFunction(particle->particle->current->pos,
                                  particle->inputBuffer[i]->solution.pos,
                                  particle->particle->current->dimension);
  }
  omp_unset_lock(&particle->inputBufferLock);
}

void obtainBestSocialFitness(ProcessParticle particle, int *indexes,
                             double *bestFitness, int *indexBestFitness,
                             PSOData psoData) {
  omp_set_lock(&particle->inputBufferLock);
  *bestFitness = particle->inputBuffer[indexes[0]]->solution.fitness;
  *indexBestFitness = 0;
  for (int i = 0; i < psoData->neighborhoodPopulation; i++) {
    double fitness = particle->inputBuffer[indexes[i]]->solution.fitness;
    if (psoData->fitnessChecker(fitness, *bestFitness)) {
      *bestFitness = fitness;
      *indexBestFitness = i;
    }
  }
  omp_unset_lock(&particle->inputBufferLock);
}

void eraseInputBuffer(ProcessParticle particle) {
  omp_set_lock(&particle->inputBufferLock);
#pragma omp parallel for
  for (int i = 0; i < particle->totalNumberOfParticles; i++) {
    destroyBroadcastMessage(particle->inputBuffer[i]);
    particle->inputBuffer[i] = NULL;
  }
  omp_unset_lock(&particle->inputBufferLock);
}

void computeNewPosition(ProcessParticle particle, const int iteration,
                        const int processId, PSOData psoData,
                        ProcessParticle *particles, const int numberOfParticles,
                        const int numberOfProcesses) {
  /* printf("Current openMP level %d\n", omp_get_level()); */
  // list of functions that have a openMP depth = 3
  //   - waitInputBuffer
  //   - computeDistances
  //   - eraseInputBuffer
  //   - updateVelocity
  //   - updatePosition
  char logMessage[50];
  sprintf(logMessage, "start particle %d", particle->particle->id);
  processLog("COMPUTING", iteration, processId, omp_get_thread_num(),
             logMessage);
  sprintf(logMessage, "waiting input buffer particle %d",
          particle->particle->id);
  processLogDebug("COMPUTING", iteration, processId, omp_get_thread_num(),
                  logMessage);
  waitInputBuffer(particle);
  sprintf(logMessage, "waiting output buffer particle %d",
          particle->particle->id);
  processLogDebug("COMPUTING", iteration, processId, omp_get_thread_num(),
                  logMessage);
  if (numberOfProcesses > 1)
    waitOutputBuffer(particle);
  sprintf(logMessage, "doing particle %d", particle->particle->id);
  processLog("COMPUTING", iteration, processId, omp_get_thread_num(),
             logMessage);
  double distances[psoData->particlesNumber];
  int indexes[psoData->particlesNumber];
  computeDistances(particle, indexes, distances, psoData);
  // sort
  double bestFitness;
  int indexBestFitness;
  obtainBestSocialFitness(particle, indexes, &bestFitness, &indexBestFitness,
                          psoData);

  omp_set_lock(&particle->inputBufferLock);
  particle->particle->socialBest =
      cloneSolution(&particle->inputBuffer[indexBestFitness]->solution);
  omp_unset_lock(&particle->inputBufferLock);
  eraseInputBuffer(particle);

  updateVelocity(particle->particle, psoData->w, psoData->phi_1,
                 psoData->phi_2);
  updatePosition(particle->particle, psoData->fitnessFunction,
                 psoData->fitnessChecker);
  BroadcastMessage outMsg = newBroadcastMessage();
  initalizeBroacastMessage(outMsg, processId, iteration + 1,
                           particle->particle->id, particle->particle->current);
  if (numberOfProcesses > 1) {
    omp_set_lock(&particle->outputBufferLock);
    particle->outputBuffer = outMsg;
    omp_unset_lock(&particle->outputBufferLock);
  }

  sprintf(logMessage, "done particle %d", particle->particle->id);
  processLog("COMPUTING", iteration, processId, omp_get_thread_num(),
             logMessage);
}

bool checkParticlesReady(const bool *isParticleSent,
                         const int numberOfParticles) {
  for (int i = 0; i < numberOfParticles; i++)
    if (!isParticleSent[i])
      return false;
  return true;
}
void sendMessage(ProcessParticle *particles, const int iterationsNumber,
                 const int numberOfParticles, const int processId,
                 const int numberOfProcesses) {
  if (numberOfProcesses == 1)
    return;
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(particles[0]->particle->dimension);

  bool isParticleReady[numberOfParticles];
  broadcastMessage_t messages[numberOfParticles];

  for (int iteration = 0; iteration < iterationsNumber; iteration++) {
    processLog("SEND", iteration, processId, omp_get_thread_num(), "start");
    for (int i = 0; i < numberOfParticles; i++)
      isParticleReady[i] = false;

    do {
      for (int i = 0; i < numberOfParticles; i++)
        if (particles[i]->outputBuffer != NULL)
          isParticleReady[i] = true;
      usleep(SLEEP_TIME);
    } while (!checkParticlesReady(isParticleReady, numberOfParticles));

    for (int particleId = 0; particleId < numberOfParticles; particleId++) {
      ProcessParticle particle = particles[particleId];
      omp_set_lock(&particle->outputBufferLock);
      /* messages[particleId] = cloneMessage(particle->outputBuffer); */
      messages[particleId] = cloneMessageStructToStruct(*particle->outputBuffer);
      destroyBroadcastMessage(particle->outputBuffer);
      particle->outputBuffer = NULL;
      omp_unset_lock(&particle->outputBufferLock);
    }
    processLog("SEND", iteration, processId, omp_get_thread_num(), "doing");
    for (int particleId = 0; particleId < numberOfParticles; particleId++) {
      printf("sto per inviare particella %d\n", messages[particleId].particleId);
    }
    MPI_Bcast(messages, numberOfParticles, DT_BROADCAST_MESSAGE, processId,
              MPI_COMM_WORLD);
    processLog("SEND", iteration, processId, omp_get_thread_num(), "done");
    /* #if DEBUG_CHECKS == 1 */
    /*           if (particle->outputBuffer->iteration != iteration) */
    /*             log_error( */
    /*                 "A mismatch in the iteration within send function
     * occured"); */
    /* #endif */
  }

  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}

void updateInnerInputBuffer(ProcessParticle *particles, ProcessParticle src,
                            const int processId, const int iteration,
                            const int numberOfParticles) {
  /* printf("Update inner buffer called during iteration %d by process %d\n",
   * iteration, processId); */
  for (int i = 0; i < numberOfParticles; i++) {
    ProcessParticle dst = particles[i];
    BroadcastMessage msg = newBroadcastMessage();
    initalizeBroacastMessage(msg, processId, iteration, src->particle->id,
                             src->particle->current);
    bool isWaiting = 0;
    do {
      omp_set_lock(&dst->inputBufferLock);
      isWaiting = dst->inputBuffer[src->particle->id] != NULL ? true : false;
      omp_unset_lock(&dst->inputBufferLock);
      usleep(SLEEP_TIME);
    } while (isWaiting);
    omp_set_lock(&dst->inputBufferLock);
    // TODO: see if it is possible to use atomic pragma for the same result
    dst->inputBuffer[src->particle->id] = msg;
    omp_unset_lock(&dst->inputBufferLock);
  }
}

void initThread(const int numberOfThreads, const int processId,
                const int numberOfParticles) {
  int actualNumberOfThreads = numberOfThreads;
  if (3 > numberOfThreads)
    actualNumberOfThreads = 3;
  log_info("Using %d thread for process number %d with %d particles",
           actualNumberOfThreads, processId, numberOfParticles);
  omp_set_num_threads(actualNumberOfThreads);
  omp_set_max_active_levels(2);
}

ProcessParticle *initProcessParticles(const int processId,
                                      const int numberOfParticles,
                                      const int startingId, PSOData psoData) {
  ProcessParticle *particles =
      (ProcessParticle *)malloc(numberOfParticles * sizeof(particle_t));
#pragma omp parallel for
  for (int i = 0; i < numberOfParticles; i++)
    particles[i] =
        newProcessParticle(i + startingId, processId, psoData->particlesNumber,
                           psoData->problemDimension, psoData->initMaxPosition,
                           psoData->initMinPosition, psoData->initMaxVelocity,
                           psoData->initMinVelocity, psoData->fitnessFunction);
  return particles;
}

void processRoutine(const int numberOfProcesses, const int numberOfThreads,
                    const int processId, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData) {
  int numberOfParticles = processToNumberOfParticles[processId];

  ProcessParticle *particles =
      initProcessParticles(processId, numberOfParticles, startingId, psoData);
  initThread(numberOfThreads, processId, numberOfParticles);

#pragma omp parallel for
  for (int i = 0; i < numberOfParticles; i++)
    updateInnerInputBuffer(particles, particles[i], processId, 0,
                           numberOfParticles);
    /* for (int i = 0; i < numberOfParticles; i++) { */
    /*   printf("particle %d: ", i); */
    /*   for (int j = 0; j < psoData->particlesNumber; j++) { */
    /*     printf(particles[i]->inputBuffer[j] == NULL ? "Y" : "N"); */
    /*   } */
    /*   printf("\n"); */
    /* } */

    // openMP depth = 1
#pragma omp parallel sections
  {
#pragma omp section
    sendMessage(particles, psoData->iterationsNumber, numberOfParticles,
                processId, numberOfProcesses);
#pragma omp section
    receiveMessage(particles, psoData->iterationsNumber, numberOfParticles,
                   numberOfProcesses, processId, processToNumberOfParticles);
#pragma omp section
    for (int iteration = 0; iteration < psoData->iterationsNumber;
         iteration++) {
#pragma omp parallel for
      for (int i = 0; i < numberOfParticles; i++) {
        computeNewPosition(particles[i], iteration, processId, psoData,
                           particles, numberOfParticles, numberOfProcesses);
      }
#pragma omp parallel for
      for (int i = 0; i < numberOfParticles; i++)
        updateInnerInputBuffer(particles, particles[i], processId,
                               iteration + 1, numberOfParticles);
    }
  }
  double bestFitness = particles[0]->particle->personalBest->fitness;
  for (int i = 0; i < numberOfParticles; i++)
    bestFitness =
        fmax(bestFitness, particles[i]->particle->personalBest->fitness);
  printf("Best fitness for process %d %f\n", processId, bestFitness);

  for (int i = 0; i < numberOfParticles; i++)
    destroyProcessParticle(particles[i]);
}
