#include "pso.h"
#include "../database/database.h"

PSOData newPSOData(const int problemDimension, const int particlesNumber,
                   const int iterationsNumber, const int neighborhoodPopulation,
                   const double w, const double phi_1, const double phi_2,
                   const double initMaxPosition, const double initMinPosition,
                   const double initMaxVelocity, const double initMinVelocity,
                   double (*fitnessFunction)(double *, int),
                   double (*distanceFunction)(double *, double *, int),
                   bool (*fitnessChecker)(double, double)) {
  PSOData psoData = (PSOData)malloc(sizeof(pso_data_t));
  psoData->problemDimension = problemDimension;
  psoData->particlesNumber = particlesNumber;
  psoData->iterationsNumber = iterationsNumber;
  psoData->neighborhoodPopulation = neighborhoodPopulation;
  psoData->w = w;
  psoData->phi_1 = phi_1;
  psoData->phi_2 = phi_2;
  psoData->initMaxPosition = initMaxPosition;
  psoData->initMinPosition = initMinPosition;
  psoData->initMaxVelocity = initMaxVelocity;
  psoData->initMinVelocity = initMinVelocity;
  psoData->fitnessFunction = fitnessFunction;
  psoData->distanceFunction = distanceFunction;
  psoData->fitnessChecker = fitnessChecker;
  return psoData;
}

static int PSOIniHandler(PSOData psoData, const char *section, const char *name,
                         const char *value) {
  char *ptr;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("global", "problemDimension")) {
    psoData->problemDimension = atoi(value);
    log_debug("%-10s :: problem dimension :: %d", "PSODATA",
              psoData->problemDimension);
  } else if (MATCH("global", "particlesNumber")) {
    psoData->particlesNumber = atoi(value);
    log_debug("%-10s :: particles number  :: %d", "PSODATA",
              psoData->particlesNumber);
  } else if (MATCH("global", "iterationsNumber")) {
    psoData->iterationsNumber = atoi(value);
    log_debug("%-10s :: iterations number :: %d", "PSODATA",
              psoData->iterationsNumber);
  } else if (MATCH("global", "neighborhoodPopulation")) {
    psoData->neighborhoodPopulation = atoi(value);
    log_debug("%-10s :: neighborhood      :: %d", "PSODATA",
              psoData->neighborhoodPopulation);
  } else if (MATCH("velocity", "w")) {
    psoData->w = strtod(value, &ptr);
    log_debug("%-10s :: w                 :: %f", "PSODATA", psoData->w);
  } else if (MATCH("velocity", "phi_1")) {
    psoData->phi_1 = strtod(value, &ptr);
    log_debug("%-10s :: phi 1             :: %f", "PSODATA", psoData->phi_1);
  } else if (MATCH("velocity", "phi_2")) {
    psoData->phi_2 = strtod(value, &ptr);
    log_debug("%-10s :: phi 2             :: %f", "PSODATA", psoData->phi_2);
  } else if (MATCH("randomBounds", "initMaxPosition")) {
    psoData->initMaxPosition = strtod(value, &ptr);
    log_debug("%-10s :: max position      :: %f", "PSODATA",
              psoData->initMaxPosition);
  } else if (MATCH("randomBounds", "initMinPosition")) {

    psoData->initMinPosition = strtod(value, &ptr);
    log_debug("%-10s :: min position      :: %f", "PSODATA",
              psoData->initMinPosition);
  } else if (MATCH("randomBounds", "initMaxVelocity")) {
    psoData->initMaxVelocity = strtod(value, &ptr);
    log_debug("%-10s :: max velocity      :: %f", "PSODATA",
              psoData->initMaxVelocity);
  } else if (MATCH("randomBounds", "initMinVelocity")) {
    psoData->initMinVelocity = strtod(value, &ptr);
    log_debug("%-10s :: min velocity      :: %f", "PSODATA",
              psoData->initMinVelocity);
  } else if (MATCH("functions", "fitness")) {
    if (strcmp(value, "sphere") == 0)
      psoData->fitnessFunction = sphere;
    else if (strcmp(value, "wave") == 0)
      psoData->fitnessFunction = wave;
    else if (strcmp(value, "Easom") == 0)
      psoData->fitnessFunction = Easom;
    else if (strcmp(value, "Ackley") == 0)
      psoData->fitnessFunction = Ackley;
    else if (strcmp(value, "Himmelblau") == 0)
      psoData->fitnessFunction = Himmelblau;
    else if (strcmp(value, "HolderTable") == 0)
      psoData->fitnessFunction = HolderTable;
    else {
      log_error("Error reading pso data from ini file: fitness function %s not "
                "implemented",
                value);
      return 1;
    }
    psoData->fitnessFunctionName = (char *)malloc(strlen(value) * sizeof(char));
    strcpy(psoData->fitnessFunctionName, value);
    log_debug("%-10s :: fitness function  :: %s", "PSODATA", value);
  } else if (MATCH("functions", "distance")) {
    if (strcmp(value, "euclidean") == 0)
      psoData->distanceFunction = euclideanDistance;
    else {
      log_error(
          "Error reading pso data from ini file: distance function %s not "
          "implemented",
          value);
      return 1;
    }

    psoData->distanceFunctionName =
        (char *)malloc(strlen(value) * sizeof(char));
    strcpy(psoData->distanceFunctionName, value);
    log_debug("%-10s :: distance function :: %s", "PSODATA", value);
  } else if (MATCH("functions", "fitnessGoal")) {
    if (strcmp(value, "minimum") == 0)
      psoData->fitnessChecker = minimize;
    else if (strcmp(value, "maximum") == 0)
      psoData->fitnessChecker = maximize;
    else {
      log_error("Error reading pso data from ini file: fitness goal %s not "
                "implemented",
                value);
      return 1;
    }
    psoData->fitnessCheckerName = (char *)malloc(strlen(value) * sizeof(char));
    strcpy(psoData->fitnessCheckerName, value);
    log_debug("%-10s :: fitness goal      :: %s", "PSODATA", value);
  } else
    return 1;

  return 0;
}

PSOData newPSODataFromFile(const char *path) {
  PSOData psoData = (PSOData)malloc(sizeof(pso_data_t));
  if (ini_parse(path, (ini_handler)PSOIniHandler, psoData) < 0) {
    log_error("Error reading pso data from ini file");
    exit(1);
  }
  return psoData;
}

void destroyPSOData(PSOData psoData) { free(psoData); }

void computeDistancesSerial(Particle particle, int *indexes, double *distances,
                      PSOData psoData, Particle *particles) {
/* #pragma omp for */
  for (int i = 0; i < psoData->particlesNumber; i++) {
    indexes[i] = i;
    distances[i] = psoData->distanceFunction(particle->current->pos,
                                             particles[i]->current->pos,
                                             particle->current->dimension);
  }
}

void sortDistancesSerial(int *neighborhoodIndex, double *distances,
                   const int dimension) {
/* #pragma omp parallel */
  {
/* #pragma omp single */
    quicksort(neighborhoodIndex, distances, 0, dimension - 1);
  }
}

void computeNeighborsSerial(PSOData psoData, Particle *particles,
                      int **neighborhoodIndex, double **distances,
                      const int processParticlesNumber) {
// Compute the distances for each of my particle using parallelism
/* #pragma omp parallel for */
  for (int i = 0; i < processParticlesNumber; i++)
    computeDistancesSerial(particles[i], neighborhoodIndex[i], distances[i], psoData,
                     particles);

// Sort the distances for each of my particle
/* #pragma omp for */
  for (int i = 0; i < processParticlesNumber; i++)
    sortDistancesSerial(neighborhoodIndex[i], distances[i], psoData->particlesNumber);
}

void updateGlobal(Particle *particles, Solution globalBest,
                  const int numberOfParticles,
                  bool (*fitnessChecker)(double, double));

void updateGlobal(Particle *particles, Solution globalBest,
                  const int numberOfParticles,
                  bool (*fitnessChecker)(double, double)) {
  int i;
  for (i = 0; i < numberOfParticles; i++) {
    Particle particle = particles[i];
    if (fitnessChecker(particle->current->fitness, globalBest->fitness)) {
      log_debug("New best global solution found with fitness value %f",
                particle->current->fitness);
      destroySolution(globalBest);
      globalBest = cloneSolution(particle->current);
    }
  }
}

void obtainBestSocialFitnessSerial(Particle particle, int *indexes,
                             double *bestFitness, int *indexBestFitness,
                             PSOData psoData, Particle *particles) {
  *bestFitness = particles[indexes[0]]->current->fitness;
  *indexBestFitness = 0;
  // TODO: understand if it is possible to parallelize
  for (int i = 0; i < psoData->neighborhoodPopulation; i++) {
    double fitness = particles[indexes[i]]->current->fitness;
    if (psoData->fitnessChecker(fitness, *bestFitness)) {
      *bestFitness = fitness;
      *indexBestFitness = i;
    }
  }
}

void updateSocialSerial(Particle particle, int *indexes, PSOData psoData, Particle *particles) {
  double bestFitness;
  int indexBestFitness;
  obtainBestSocialFitnessSerial(particle, indexes, &bestFitness,
                          &indexBestFitness, psoData, particles);
  destroySolution(particle->socialBest);
  particle->socialBest = cloneSolution(particles[indexBestFitness]->current);
  log_debug("%-10s :: Update social best solution of particle %d", "COMPUTING",
            particle->id);
}

void initParticles(Particle *particles, const int particlesNumber,
                   PSOData psoData, const int startingId) {
/* #pragma omp parallel for */
  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = newParticle(
        i + startingId, psoData->problemDimension, psoData->initMaxPosition,
        psoData->initMinPosition, psoData->initMaxVelocity,
        psoData->initMinVelocity, psoData->fitnessFunction);
    particles[i] = particle;
    log_debug("%-10s :: Initializing particle %d in range %d-%d", "INIT",
              i + 1 + startingId, startingId, startingId + particlesNumber);
  }
}

void dumpParticles(Particle *particles, Database db, const int iteration_step,
                   const int numberOfParticles) {
  for (int i = 0; i < numberOfParticles; i++) {
    Particle particle = particles[i];
    insertSolution(db, particle->current, particle->id, iteration_step);
  }
}

void particleSwarmOptimization(Particle *particles, PSOData psoData,
                               const char *databasePath) {
  int _;
  int i;
  bool useDB = strcmp(databasePath, "") == 0 ? false : true;
  Database db;
  if (useDB)
    db = newDatabase(databasePath, psoData, -1, -1);
  Solution globalBestSolution;
  globalBestSolution = cloneSolution(particles[0]->current);

  int **neighborhoodIndex =
      (int **)malloc(sizeof(int *) * psoData->particlesNumber);
  double **distances =
      (double **)malloc(sizeof(double *) * psoData->particlesNumber);
/* #pragma omp parallel for */
  for (int i = 0; i < psoData->particlesNumber; i++) {
    neighborhoodIndex[i] =
        (int *)malloc(sizeof(int) * psoData->particlesNumber);
    distances[i] = (double *)malloc(sizeof(double) * psoData->particlesNumber);
  }

  for (_ = 0; _ < psoData->iterationsNumber; _++) {
    computeNeighborsSerial(psoData, particles, neighborhoodIndex, distances,
                     psoData->particlesNumber);
    updateGlobal(particles, globalBestSolution, psoData->particlesNumber,
                 psoData->fitnessChecker);
    for (i = 0; i < psoData->particlesNumber; i++) {
      Particle particle = particles[i];
      updateSocialSerial( particle, neighborhoodIndex[i], psoData, particles);
      updateVelocity(particle, psoData->w, psoData->phi_1, psoData->phi_2);
      log_debug("%-10s :: Iteration %d/%d particle %d/%d velocity updated",
                "COMPUTING", _ + 1, psoData->iterationsNumber, i + 1,
                psoData->particlesNumber);
      updatePosition(particle, psoData->fitnessFunction,
                     psoData->fitnessChecker);
      log_debug("%-10s :: Iteration %d/%d particle %d/%d position updated",
                "COMPUTING", _ + 1, psoData->iterationsNumber, i + 1,
                psoData->particlesNumber);
    }
    log_info("%-10s :: iteration %d/%d with best fitness %f", "COMPUTING",
             _ + 1, psoData->iterationsNumber, globalBestSolution->fitness);
    if (useDB)
      dumpParticles(particles, db, _, psoData->particlesNumber);
  }
  printf("Best fitness %f\n", globalBestSolution->fitness);

  for (int i = 0; i < psoData->particlesNumber; i++) {
    free(neighborhoodIndex[i]);
    free(distances[i]);
  }
  free(neighborhoodIndex);
  free(distances);
  for (int i = 0; i < psoData->particlesNumber; i++)
    destroyParticle(particles[i]);
  if (useDB)
    destroyDatabase(db);
}
