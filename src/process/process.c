#include "./process.h"

#define SLEEP_TIME 1000

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
  processParticle->outputBuffer->iteration = 0;
  processParticle->outputBuffer->particleId = particleId;
  processParticle->outputBuffer->mpi_process = processId;
  gettimeofday(&processParticle->outputBuffer->timestamp, NULL);
  includeSolution(&processParticle->outputBuffer->solution,
                  processParticle->particle->current);
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

void createMPIRequests(List *messages, List *requests,
                       const int numberOfProcesses, const int processId,
                       const int *processToNumberOfParticles,
                       MPI_Datatype *mpi_datatype) {
#pragma omp parallel for
  for (int pid = 0; pid < numberOfProcesses; pid++)
    if (pid != processId)
      for (int parId = 0; parId < processToNumberOfParticles[pid]; parId++) {
        BroadcastMessage msg = newBroadcastMessage();
        MPI_Request *req = (MPI_Request *)malloc(sizeof(MPI_Request));
        enqueue(messages[pid], msg);
        enqueue(requests[pid], req);
        /* printf("I'm %d and I'm waiting %d %p %p\n", processId, pid, msg, req); */
        MPI_Ibcast(msg, 1, *mpi_datatype, pid, MPI_COMM_WORLD, req);
      }
}

void waitMPIRequests(ProcessParticle particle, const int iteration,
                     List *messages, List *requests,
                     const int numberOfProcesses, const int processId,
                     const int *processToNumberOfParticles,
                     MPI_Datatype *mpi_datatype) {

  MPI_Status status;
  int processesDone;
  do {
    processesDone = 0;
#pragma omp parallel for reduction(+ : processesDone)
    for (int pid = 0; pid < numberOfProcesses; pid++) {
      processesDone += isEmpty(requests[pid]) == true ? 1 : 0;
      Element currRequest = requests[pid]->head;
      Element currMessage = messages[pid]->head;
      while (currRequest != NULL) {
        int isCompleted = 0;
        MPI_Test((MPI_Request *)currRequest->data, &isCompleted, &status);
        /* printf("leggo %p %p con status %d %d %d %d\n", currMessage->data, */
        /*        currRequest->data, status.MPI_ERROR, status.MPI_SOURCE, */
        /*        status.MPI_TAG, status._cancelled); */
        if (isCompleted == 1) {
          processLog("READ", iteration, processId, omp_get_thread_num(),
                     "done");
          if (currRequest->prev == NULL) {
            requests[pid]->head = currRequest->next;
            messages[pid]->head = currMessage->next;
          } else {
            currRequest->prev->next = currRequest->next;
            currMessage->prev->next = currMessage->next;
          }
          if (currRequest->next != NULL) {
            currRequest->next->prev = currRequest->prev;
            currMessage->next->prev = currMessage->prev;
          }
          BroadcastMessage msg = currMessage->data;
          int particleId = msg->particleId;
          omp_set_lock(&particle->inputBufferLock);
          if (particle->inputBuffer[particleId] == NULL)
            particle->inputBuffer[particleId] = msg;
          else
            log_error("Particle %d, process %d input buffer is full",
                      particleId, processId);
          omp_unset_lock(&particle->inputBufferLock);
          Element lastRequest = currRequest;
          Element lastMessage = currMessage;
          free(lastRequest);
          free(lastMessage);
        }
        currRequest = currRequest->next;
        currMessage = currMessage->next;
      }
    }
    usleep(SLEEP_TIME);
  } while (processesDone != numberOfProcesses);
}

void receiveMessage(ProcessParticle particle, const int iterationsNumber,
                    const int numberOfProcesses, const int processId,
                    const int *processToNumberOfParticles) {
  if (numberOfProcesses == 1)
    return;
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(particle->particle->dimension);
  List messages[numberOfProcesses];
  List requests[numberOfProcesses];
#pragma omp parallel for
  for (int pid = 0; pid < numberOfProcesses; pid++) {
    messages[pid] = newList();
    requests[pid] = newList();
  }

  for (int iteration = 0; iteration < iterationsNumber; iteration++) {
    processLog("READ", iteration, processId, omp_get_thread_num(), "start");
    createMPIRequests(messages, requests, numberOfProcesses, processId,
                      processToNumberOfParticles, &DT_BROADCAST_MESSAGE);
    waitMPIRequests(particle, iteration, messages, requests, numberOfProcesses,
                    processId, processToNumberOfParticles,
                    &DT_BROADCAST_MESSAGE);
  }

#pragma omp parallel for
  for (int pid = 0; pid < numberOfProcesses; pid++) {
    destroyList(messages[pid], (void *)destroyBroadcastMessage);
    destroyList(requests[pid], (void *)MPI_Request_free);
  }
  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}

void waitInputBuffer(ProcessParticle particle) {
  int isBufferFull;
  do {
    printf("CICLO\n");
    isBufferFull = 0;
    omp_set_lock(&particle->inputBufferLock);
#pragma omp parallel for reduction(+ : isBufferFull)
    for (int i = 0; i < particle->totalNumberOfParticles; i++)
      isBufferFull += particle->inputBuffer[i] != NULL ? 1 : 0;
    omp_unset_lock(&particle->inputBufferLock);
    usleep(SLEEP_TIME);
  } while (isBufferFull != particle->totalNumberOfParticles);
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
#pragma omp for
  for (int i = 0; i < particle->totalNumberOfParticles; i++) {
    destroyBroadcastMessage(particle->inputBuffer[i]);
    particle->inputBuffer[i] = NULL;
  }
  /* printf("particle %d [", particle->particle->id); */
  /* for (int i = 0; i < particle->totalNumberOfParticles; i++) */
  /*   printf(particle->inputBuffer[i] == NULL ? "NULLO " : "NON_NULLO "); */
  /* printf("]\n"); */
  omp_unset_lock(&particle->inputBufferLock);
}

void computeNewPosition(ProcessParticle particle, const int iterationsNumber,
                        const int processId, PSOData psoData,
                        ProcessParticle *particles,
                        const int numberOfParticles) {
  printf("we are %d mode\n", omp_in_parallel());
  char *logMessage;
  logMessage = (char *)malloc(sizeof(char) * 20);
  for (int iteration = 0; iteration < iterationsNumber; iteration++) {
    sprintf(logMessage, "start particle %d", particle->particle->id);
    processLog("COMPUTING", iteration, processId, omp_get_thread_num(),
               logMessage);
    waitInputBuffer(particle);
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
    updatePosition(particle->particle, psoData->fitnessFunction);

    updateInnerInputBuffer(particles, particle, processId, iteration,
                             numberOfParticles);

    sprintf(logMessage, "done particle %d", particle->particle->id);
    processLog("COMPUTING", iteration, processId, omp_get_thread_num(),
               logMessage);
  }
  free(logMessage);
}
void sendMessage(ProcessParticle particle, const int iterationsNumber,
                 const int processId, const int numberOfProcesses) {
  if (numberOfProcesses == 1)
    return;
  MPI_Request request;
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(particle->particle->dimension);

  for (int iteration = 0; iteration < iterationsNumber; iteration++) {
    processLog("SEND", iteration, processId, omp_get_thread_num(), "start");
    int isSent = 0;
    do {
      omp_set_lock(&particle->outputBufferLock);
      if (particle->outputBuffer != NULL) {
        isSent = 1;
        processLog("SEND", iteration, processId, omp_get_thread_num(), "doing");
        MPI_Ibcast(particle->outputBuffer, 1, DT_BROADCAST_MESSAGE, processId,
                   MPI_COMM_WORLD, &request);
        destroyBroadcastMessage(particle->outputBuffer);
        particle->outputBuffer = NULL;
        processLog("SEND", iteration, processId, omp_get_thread_num(), "done");
      }
      omp_unset_lock(&particle->outputBufferLock);
      usleep(SLEEP_TIME);
    } while (isSent != 1);
  }

  MPI_Request_free(&request);
  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}

void updateInnerInputBuffer(ProcessParticle *particles, ProcessParticle src,
                            const int processId, const int iteration,
                            const int numberOfParticles) {
  #pragma omp for
  for (int i = 0; i < numberOfParticles; i++) {
    ProcessParticle dst = particles[i];
    BroadcastMessage msg = newBroadcastMessage();
    initalizeBroacastMessage(msg, processId, iteration, src->particle->id,
                             src->particle->current);
    bool isWaiting = 0;
    do {
      omp_set_lock(&dst->inputBufferLock);
      isWaiting = dst->inputBuffer[src->particle->id] != NULL ? true : false;
      if (isWaiting == true) {
        /* printf("the pointer is: %p %d\n",
         * dst->inputBuffer[src->particle->id], dst->particle->id); */
        /* printSolution(&(dst->inputBuffer[src->particle->id]->solution)); */
      }
      omp_unset_lock(&dst->inputBufferLock);
      usleep(SLEEP_TIME);
    } while (isWaiting);
    omp_set_lock(&dst->inputBufferLock);
    // TODO: see if it is possible to use atomic pragma for the same result
    dst->inputBuffer[src->particle->id] = msg;
    omp_unset_lock(&dst->inputBufferLock);
  }
}

void processRoutine(const int numberOfProcesses, const int numberOfThreads,
                    const int processId, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData) {
  int numberOfParticles = processToNumberOfParticles[processId];
  ProcessParticle *particles =
      (ProcessParticle *)malloc(numberOfParticles * sizeof(particle_t));
  for (int i = 0; i < numberOfParticles; i++)
    particles[i] =
        newProcessParticle(i + startingId, processId, psoData->particlesNumber,
                           psoData->problemDimension, psoData->initMaxPosition,
                           psoData->initMinPosition, psoData->initMaxVelocity,
                           psoData->initMinVelocity, psoData->fitnessFunction);

  int actualNumberOfThreads = numberOfThreads;
  if (3 > numberOfThreads)
    actualNumberOfThreads = 3;
  log_info("Using %d thread for process number %d with %d particles",
           actualNumberOfThreads, processId, numberOfParticles);
  omp_set_num_threads(actualNumberOfThreads);
  omp_set_nested(1);

#pragma omp parallel for
  for (int i = 0; i < numberOfParticles; i++)
    updateInnerInputBuffer(particles, particles[i], processId, 0,
                           numberOfParticles);
/* #pragma omp parallel */
/*   { */
/* #pragma omp single nowait */
/*     for (int i = 0; i < numberOfParticles; i++) */
/*    #pragma omp task */
/*       { */
/*       receiveMessage(particles[i], psoData->iterationsNumber, numberOfProcesses, */
/*                      processId, processToNumberOfParticles); */
/*       } */
/* #pragma omp single nowait */
/*     for (int i = 0; i < numberOfParticles; i++) */
/*    #pragma omp task */
/*       { */
/*       sendMessage(particles[i], psoData->iterationsNumber, processId, */
/*                   numberOfProcesses); */
/*       } */
/* #pragma omp parallel for */
/*     for (int i = 0; i < numberOfParticles; i++) */
/*       computeNewPosition(particles[i], psoData->iterationsNumber, processId, */
/*                          psoData, particles, numberOfParticles); */
/*   } */
#pragma omp parallel
  {
#pragma omp for nowait
    for (int i = 0; i < numberOfParticles; i++)
      receiveMessage(particles[i], psoData->iterationsNumber, numberOfProcesses,
                     processId, processToNumberOfParticles);
#pragma omp for nowait
    for (int i = 0; i < numberOfParticles; i++)
      sendMessage(particles[i], psoData->iterationsNumber, processId,
                  numberOfProcesses);
#pragma omp for nowait
    for (int i = 0; i < numberOfParticles; i++)
      computeNewPosition(particles[i], psoData->iterationsNumber, processId,
                         psoData, particles, numberOfParticles);
  }
  printf("Best fitness %f\n", particles[0]->particle->socialBest->fitness);

  for (int i = 0; i < numberOfParticles; i++)
    destroyProcessParticle(particles[i]);
}
