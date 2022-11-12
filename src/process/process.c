#include "./process.h"
#include <stdio.h>

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
  // TODO can be parallelized?
  for (int i = 0; i < psoData->neighborhoodPopulation; i++) {
    double fitness = inputBuffer[indexes[i]].solution.fitness;
    if (psoData->fitnessChecker(fitness, *bestFitness)) {
      *bestFitness = fitness;
      *indexBestFitness = i;
    }
  }
}

int partition(broadcastMessage_t *a, int p, int r,
              bool (*fitnessChecker)(double, double)) {
  broadcastMessage_t lt[r - p];
  broadcastMessage_t gt[r - p];
  broadcastMessage_t key = a[r];
  int i;
  int j;
  int lt_n = 0;
  int gt_n = 0;

  for (i = p; i < r; i++) {
    if (fitnessChecker(a[i].solution.fitness, a[r].solution.fitness)) {
      lt[lt_n++] = a[i];
    } else {
      gt[gt_n++] = a[i];
    }
  }

  for (i = 0; i < lt_n; i++) {
    a[p + i] = lt[i];
  }

  a[p + lt_n] = key;

  for (j = 0; j < gt_n; j++) {
    a[p + lt_n + j + 1] = gt[j];
  }

  return p + lt_n;
}

void quicksort(broadcastMessage_t *a, int p, int r,
               bool (*fitnessChecker)(double, double)) {
  int div;

  if (p < r) {
    div = partition(a, p, r, fitnessChecker);
#pragma omp task shared(a)
    quicksort(a, p, div - 1, fitnessChecker);
#pragma omp task shared(a)
    quicksort(a, div + 1, r, fitnessChecker);
  }
}

void computeNewPosition(Particle particle, const int iteration,
                        const int processId, PSOData psoData,
                        Particle *particles, const int index,
                        const int numberOfProcesses,
                        broadcastMessage_t *inputBuffer,
                        broadcastMessage_t *outputBuffer) {
  double distances[psoData->particlesNumber];
  int indexes[psoData->particlesNumber];
  // Compute the distances in a parallel manner
  computeDistances(particle, indexes, distances, psoData, inputBuffer);
  // TODO missing sort, would be better to put it before the send and then a
  // merge
  double bestFitness;
  int indexBestFitness;
  // Obtain the best social fitness
  obtainBestSocialFitness(particle, indexes, &bestFitness, &indexBestFitness,
                          psoData, inputBuffer);

  destroySolution(particle->socialBest);
  // Get the social best
  particle->socialBest = cloneSolution(&inputBuffer[indexBestFitness].solution);

  updateVelocity(particle, psoData->w, psoData->phi_1, psoData->phi_2);
  updatePosition(particle, psoData->fitnessFunction, psoData->fitnessChecker);

  // Ready to initialize the broadcast message
  if (numberOfProcesses > 1) {
    initalizeBroacastMessage(&outputBuffer[index], processId, iteration + 1,
                             particle->id, particle->current);
  }
}

void initOutputBuffer(broadcastMessage_t *outputBuffer, Particle *particles,
                      const int process_id, const int processParticlesNumber,
                      const int iteration) {
  // TODO can be parallelized
  for (int i = 0; i < processParticlesNumber; i++) {
    initalizeBroacastMessage(&outputBuffer[i], process_id, iteration,
                             particles[i]->id, particles[i]->current);
  }
}

void mergeArrays(broadcastMessage_t *arr, int *displacements, const int dim,
                 const int numberOfProcesses,
                 bool (*fitnessChecker)(double, double)) {
  // Set of indexes of the array
  int indexes[numberOfProcesses];
  broadcastMessage_t old[dim];

  // Initialization stage: old is the old array and indexes is the array of
  // current index values
#pragma omp parallel
  {
#pragma omp for
    for (int i = 0; i < dim; i++) {
      old[i] = arr[i];
    }
#pragma omp for
    for (int i = 0; i < numberOfProcesses; i++) {
      indexes[i] = displacements[i];
    }
  }

  double *min = NULL;
  double val = 0.0;
  // Number of dimension
  for (int i = 0; i < dim; i++) {
    int indexToUpdate;
    min = NULL;
    for (int j = 0; j < numberOfProcesses; j++) {
      // Reached the limit of their chunks
      if (((j < numberOfProcesses - 1) &&
           (indexes[j] >= displacements[j + 1])) ||
          ((j == numberOfProcesses - 1) && (indexes[j] >= dim))) {
        continue;
      }
      // Set the best is found
      if (min == NULL ||
          fitnessChecker(old[indexes[j]].solution.fitness, *min)) {
        indexToUpdate = j;
        min = &val;
        *min = old[indexes[j]].solution.fitness;
      }
    }

    // Update
    arr[i] = old[indexes[indexToUpdate]];
    indexes[indexToUpdate]++;
  }
}

void sortParticles(broadcastMessage_t *outputBuffer,
                   const int processParticlesNumber,
                   bool (*fitnessChecker)(double, double)) {
#pragma omp parallel
  {
#pragma omp single
    quicksort(outputBuffer, 0, processParticlesNumber - 1, fitnessChecker);
  }
}

void processRoutine(const int processesNumber, const int threadsNumber,
                    const int pid, const int startingId,
                    const int *processToNumberOfParticles, PSOData psoData) {
  // Number of particles for process
  int processParticlesNumber = processToNumberOfParticles[pid];

  Particle *particles =
      (Particle *)malloc(processParticlesNumber * sizeof(particle_t));
  // Messages buffer
  broadcastMessage_t inputBuffer[psoData->particlesNumber];
  broadcastMessage_t outputBuffer[processParticlesNumber];
  // MPI message type
  MPI_Datatype DT_BROADCAST_MESSAGE =
      define_datatype_broadcast_message(psoData->problemDimension);

  // Cumulated sum for the gather
  int cumulatedSum[processesNumber];

  // Gather displacement calculation (probably cannot be parallelized)
  for (int i = 0; i < processesNumber; i++)
    if (i == 0)
      cumulatedSum[i] = 0;
    else
      cumulatedSum[i] = cumulatedSum[i - 1] + processToNumberOfParticles[i - 1];

  // Initialize the particles
  initParticles(particles, psoData, startingId);

  // OMP settings
  omp_set_num_threads(threadsNumber);
  omp_set_max_active_levels(2);

  log_info("Using %d thread for process number %d with %d particles",
           threadsNumber, pid, processParticlesNumber);

  // Initialize the input buffer
  initOutputBuffer(outputBuffer, particles, pid, processParticlesNumber, 0);

  // Parallel algorithm
  for (int iteration = 0; iteration < psoData->iterationsNumber; iteration++) {
    if (processesNumber > 1) {
      sortParticles(outputBuffer, processParticlesNumber,
                    psoData->fitnessChecker);
      MPI_Allgatherv(outputBuffer, processParticlesNumber, DT_BROADCAST_MESSAGE,
                     inputBuffer, processToNumberOfParticles, cumulatedSum,
                     DT_BROADCAST_MESSAGE, MPI_COMM_WORLD);
      mergeArrays(inputBuffer, cumulatedSum, psoData->particlesNumber,
                  processesNumber, psoData->fitnessChecker);
      printf("Received buffer: ");
      for (int k = 0; k < psoData->particlesNumber; k++) {
        printf("%f\t", inputBuffer[k].solution.fitness);
      }
      printf("\n");
    }

    processLog("GATHERING", iteration, pid, omp_get_thread_num(), "done");

#pragma omp parallel for
    for (int i = 0; i < processParticlesNumber; i++)
      computeNewPosition(particles[i], iteration, pid, psoData, particles, i,
                         processesNumber, inputBuffer, outputBuffer);
    processLog("COMPUTING", iteration, pid, omp_get_thread_num(), "done");
  }

  // BEST FITNESS:
  double bestFitness = particles[0]->personalBest->fitness;
  // TODO si può fare con una reduce [forse no perché dobbiamo tenere conto del
  // vicinato]
  for (int i = 0; i < processParticlesNumber; i++)
    bestFitness = psoData->fitnessChecker(bestFitness,
                                          particles[i]->personalBest->fitness)
                      ? bestFitness
                      : particles[i]->personalBest->fitness;
  printf("Best fitness for process %d %f\n", pid, bestFitness);

  // TODO si può parallelizzare
  for (int i = 0; i < processParticlesNumber; i++)
    destroyParticle(particles[i]);

  MPI_Type_free(&DT_BROADCAST_MESSAGE);
}
