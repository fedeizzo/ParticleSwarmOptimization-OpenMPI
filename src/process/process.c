#include "./process.h"

ProcessParticle newProcessParticle(const int particleId,
                                   const int totalNumberOfParticles,
                                   const int problemDimension, const double max,
                                   const double min, const double v_max,
                                   const double v_min,
                                   double (*fitnessFunction)(double *, int)) {
  const int bufferSize = 2;
  ProcessParticle processParticle =
      (ProcessParticle)malloc(sizeof(ProcessParticle));
  processParticle->totalNumberOfParticles = totalNumberOfParticles;
  processParticle->particle = newParticle(particleId, problemDimension, max,
                                          min, v_max, v_min, fitnessFunction);
  processParticle->inputBuffer = (BroadcastMessage **)malloc(
      totalNumberOfParticles * sizeof(BroadcastMessage));
  for (int i = 0; i < totalNumberOfParticles; i++)
    processParticle->inputBuffer[i] =
        (BroadcastMessage *)malloc(bufferSize * sizeof(BroadcastMessage));
  processParticle->outputBuffer =
      (BroadcastMessage)malloc(sizeof(BroadcastMessage));
  processParticle->isMessageReady = false;
  omp_init_lock(&processParticle->inputBufferLock);
  return processParticle;
}

void destroyProcessParticle(ProcessParticle particle) {
  for (int i = 0; i < particle->totalNumberOfParticles; i++)
    free(particle->inputBuffer[i]);
  free(particle->inputBuffer);
  free(particle->outputBuffer);
  destroyParticle(particle->particle);
  omp_destroy_lock(&particle->inputBufferLock);
  free(particle);
}

void receiveMessage() {

}

void computeNewPosition(ProcessParticle particle, const int iteration) {
  int isBufferFull = 0;
  do {
    omp_set_lock(&particle->inputBufferLock);
#pragma omp parallel for reduction(+ : isBufferFull)
    for (int i = 0; i < particle->totalNumberOfParticles; i++)
      isBufferFull += particle->inputBuffer[i][0] != NULL ? 1 : 0;
    omp_unset_lock(&particle->inputBufferLock);
  } while (isBufferFull != particle->totalNumberOfParticles);
  log_info("Particle %d ready for iteration %d", particle->particle->id,
           iteration);
}
void sendMessage() {}

void processRoutine(const int numberOfThreads, const int startingId,
                    const int numberOfParticles,
                    const int totalNumberOfParticles, const int totalIterations,
                    const int problemDimension, const double max,
                    const double min, const double v_max, const double v_min,
                    double (*fitnessFunction)(double *, int)) {
  ProcessParticle *particles =
      (ProcessParticle *)malloc(numberOfParticles * sizeof(particle_t));
  log_info("Initializing process with %d particles", numberOfParticles);
  for (int i = 0; i < numberOfParticles; i++)
    particles[i] = newProcessParticle(i + startingId, totalNumberOfParticles,
                                      problemDimension, max, min, v_max, v_min,
                                      fitnessFunction);
  int actualNumberOfThreads = numberOfThreads;
  if (2 > numberOfThreads)
    actualNumberOfThreads = 2;
  // specify the process here, maybe the pid
  log_info("Using %d thread for process ");
  omp_set_num_threads(actualNumberOfThreads);
  for (int iteration = 0; iteration < totalIterations; iteration++) {
#pragma omp parallel
    {
#pragma omp for
      for (int i = 0; i < numberOfParticles; i++) {
#pragma omp sections nowait
        {
#pragma omp section nowait
          receiveMessage();
#pragma omp section nowait
          sendMessage();
        }
        computeNewPosition(particles[i], iteration);
      }
    }
  }

  for (int i = 0; i < numberOfParticles; i++)
    destroyProcessParticle(particles[i]);
}
