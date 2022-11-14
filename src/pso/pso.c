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
  if (MATCH("global", "problemDimension"))
    psoData->problemDimension = atoi(value);
  else if (MATCH("global", "particlesNumber"))
    psoData->particlesNumber = atoi(value);
  else if (MATCH("global", "iterationsNumber"))
    psoData->iterationsNumber = atoi(value);
  else if (MATCH("global", "neighborhoodPopulation"))
    psoData->neighborhoodPopulation = atoi(value);
  else if (MATCH("velocity", "w"))
    psoData->w = strtod(value, &ptr);
  else if (MATCH("velocity", "phi_1"))
    psoData->phi_1 = strtod(value, &ptr);
  else if (MATCH("velocity", "phi_2"))
    psoData->phi_2 = strtod(value, &ptr);
  else if (MATCH("randomBounds", "initMaxPosition"))
    psoData->initMaxPosition = strtod(value, &ptr);
  else if (MATCH("randomBounds", "initMinPosition"))
    psoData->initMinPosition = strtod(value, &ptr);
  else if (MATCH("randomBounds", "initMaxVelocity"))
    psoData->initMaxVelocity = strtod(value, &ptr);
  else if (MATCH("randomBounds", "initMinVelocity"))
    psoData->initMinVelocity = strtod(value, &ptr);
  else if (MATCH("functions", "fitness")) {
    if (strcmp(value, "sphere"))
      psoData->fitnessFunction = sphere;
    else if (strcmp(value, "wave"))
      psoData->fitnessFunction = wave;
    else {
      log_error("Error reading pso data from ini file: fitness function %s not "
                "implemented",
                value);
      return 1;
    }
  } else if (MATCH("functions", "distance")) {
    if (strcmp(value, "euclidean"))
      psoData->distanceFunction = euclideanDistance;
    else {
      log_error(
          "Error reading pso data from ini file: distance function %s not "
          "implemented",
          value);
      return 1;
    }
  } else if (MATCH("functions", "fitnessGoal")) {
    if (strcmp(value, "minimum"))
      psoData->fitnessChecker = minimize;
    else if (strcmp(value, "maximum"))
      psoData->fitnessChecker = minimize;
    else {
      log_error("Error reading pso data from ini file: fitness goal %s not "
                "implemented",
                value);
      return 1;
    }
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

void updateGlobal(Particle *particles, Solution globalBest,
                  const int numberOfParticles,
                  bool (*fitnessChecker)(double, double));

void updateGlobal(Particle *particles, Solution globalBest,
                  const int numberOfParticles,
                  bool (*fitnessChecker)(double, double)) {
  int i;
  for (i = 0; i < numberOfParticles; i++) { // n particle
    // Update the solution
    Particle particle = particles[i];
    if (fitnessChecker(particle->current->fitness, globalBest->fitness)) {
      log_debug("New best global solution found with fitness value %f",
                particle->current);
      destroySolution(globalBest);
      globalBest = cloneSolution(particle->current);
    }
  }

  for (i = 0; i < numberOfParticles; i++) { // n particle
    log_debug("Update social best solution of particle %d", i);
    Particle particle = particles[i];
    destroySolution(particle->socialBest);
    particle->socialBest = cloneSolution(globalBest);
  }
}

bool initParticles(Particle *particles, PSOData psoData, int startingId) {
  int i;
  int rc;
  Particle particle;
  // TODO could be parallelized
  for (i = 0; i < psoData->particlesNumber; i++) {
    log_debug("Initializing particle %d/%d", i + 1 + startingId,
              psoData->particlesNumber);
    particle = newParticle(i + startingId, psoData->problemDimension,
                           psoData->initMaxPosition, psoData->initMinPosition,
                           psoData->initMaxVelocity, psoData->initMinVelocity,
                           psoData->fitnessFunction);
    rc = checkAllocationError(particle);
    if (rc == FAILURE)
      return false;
    particles[i] = particle;
  }
  return true;
}

void dumpParticles(Particle *particles, Database db, const int iteration_step,
                   const int numberOfParticles) {
  for (int i = 0; i < numberOfParticles; i++) { // n particles
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

  for (_ = 0; _ < psoData->iterationsNumber; _++) { // n iterations
    updateGlobal(particles, globalBestSolution, psoData->particlesNumber,
                 psoData->fitnessChecker);
    for (i = 0; i < psoData->particlesNumber; i++) { // n particles
      Particle particle = particles[i];
      updateVelocity(particle, psoData->w, psoData->phi_1, psoData->phi_2);
      log_debug("Iteration %d/%d particle %d/%d velocity updated", _ + 1,
                psoData->iterationsNumber, i + 1, psoData->particlesNumber);
      log_debug("Iteration %d/%d particle %d/%d position updated", _ + 1,
                psoData->iterationsNumber, i + 1, psoData->particlesNumber);
      updatePosition(particle, psoData->fitnessFunction,
                     psoData->fitnessChecker);
    }
    log_info("Swarm optimization iteration %d/%d with best fitness %f", _ + 1,
             psoData->iterationsNumber, globalBestSolution->fitness);
    if (useDB)
      dumpParticles(particles, db, _, psoData->particlesNumber);
  }
  if (useDB)
    destroyDatabase(db);
}
