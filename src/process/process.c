#include "./process.h"

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

void computeDistances(Particle particle, int *indexes, double *distances,
                      PSOData psoData, broadcastMessage_t *inputBuffer) {
#pragma omp parallel for
  for (int i = 0; i < psoData->particlesNumber; i++) {
    indexes[i] = i;
    distances[i] = psoData->distanceFunction(particle->current->pos,
                                             inputBuffer[i].solution.pos,
                                             particle->current->dimension);
  }
}

void obtainBestSocialFitness(Particle particle, int *indexes,
                             double *bestFitness, int *indexBestFitness,
                             PSOData psoData, broadcastMessage_t *inputBuffer) {
  *bestFitness = inputBuffer[indexes[0]].solution.fitness;
  *indexBestFitness = 0;
  for (int i = 0; i < psoData->neighborhoodPopulation; i++) {
    double fitness = inputBuffer[indexes[i]].solution.fitness;
    if (psoData->fitnessChecker(fitness, *bestFitness)) {
      *bestFitness = fitness;
      *indexBestFitness = i;
    }
  }
}

void computeNewPosition(Particle particle, const int iteration,
                        const int processId, PSOData psoData,
                        Particle *particles, const int numberOfParticles,
                        const int numberOfProcesses,
                        broadcastMessage_t *inputBuffer,
                        broadcastMessage_t *outputBuffer) {
  /* printf("Current openMP level %d\n", omp_get_level()); */
  // list of functions that have a openMP depth = 3
  //   - waitInputBuffer
  //   - computeDistances
  //   - eraseInputBuffer
  //   - updateVelocity
  //   - updatePosition
  double distances[psoData->particlesNumber];
  int indexes[psoData->particlesNumber];
  computeDistances(particle, indexes, distances, psoData, inputBuffer);
  // sort
  double bestFitness;
  int indexBestFitness;
  obtainBestSocialFitness(particle, indexes, &bestFitness, &indexBestFitness,
                          psoData, inputBuffer);

  destroySolution(particle->socialBest);
  particle->socialBest = cloneSolution(&inputBuffer[indexBestFitness].solution);

  updateVelocity(particle, psoData->w, psoData->phi_1, psoData->phi_2);
  updatePosition(particle, psoData->fitnessFunction, psoData->fitnessChecker);
  if (numberOfProcesses > 1) {
    initalizeBroacastMessage(&outputBuffer[particle->id], processId,
                             iteration + 1, particle->id, particle->current);
  }
}

void processRoutine(const int processesNumber, const int threadsNumber,
                    const int pid, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData) {
  int processParticlesNumber = processToNumberOfParticles[pid];

  Particle *particles =
      (Particle *)malloc(processParticlesNumber * sizeof(particle_t));
  broadcastMessage_t inputBuffer[psoData->particlesNumber];
  broadcastMessage_t outputBuffer[processParticlesNumber];
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(psoData->problemDimension);
  int cumulatedSum[processesNumber];

  for (int i = 0; i < processesNumber; i++)
    if (i == 0)
      cumulatedSum[i] = 0;
    else
      cumulatedSum[i] = cumulatedSum[i - 1] + processToNumberOfParticles[i - 1];

  initParticles(particles, psoData);
  omp_set_num_threads(threadsNumber);
  omp_set_max_active_levels(2);
  log_info("Using %d thread for process number %d with %d particles",
           threadsNumber, pid, processParticlesNumber);

  for (int iteration = 0; iteration < psoData->iterationsNumber; iteration++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (processesNumber > 1)
      MPI_Allgatherv(outputBuffer, processParticlesNumber, DT_BROADCAST_MESSAGE,
                     inputBuffer, processToNumberOfParticles, cumulatedSum,
                     DT_BROADCAST_MESSAGE, MPI_COMM_WORLD);
    processLog("GATHERING", iteration, pid, omp_get_thread_num(), "done");
#pragma omp parallel for
    for (int i = 0; i < processesNumber; i++)
      computeNewPosition(particles[i], iteration, pid, psoData, particles,
                         processesNumber, processesNumber, inputBuffer,
                         outputBuffer);
    processLog("COMPUTING", iteration, pid, omp_get_thread_num(), "done");
  }
  double bestFitness = particles[0]->personalBest->fitness;
  for (int i = 0; i < processParticlesNumber; i++)
    bestFitness = fmax(bestFitness, particles[i]->personalBest->fitness);
  printf("Best fitness for process %d %f\n", pid, bestFitness);

  for (int i = 0; i < processParticlesNumber; i++)
    destroyParticle(particles[i]);
  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}
