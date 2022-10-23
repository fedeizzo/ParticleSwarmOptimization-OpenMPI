#include "pso.h"
#include "../../include/config.h"
#include "../database/database.h"
#include "../log/log.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void updateGlobal(ArrayList particles, Solution globalBest);

void updateGlobal(ArrayList particles, Solution globalBest) {
  int i;
  for (i = 0; i < getNumberElements(particles); i++) { // n particle
    // Update the solution
    Particle particle = (Particle)getDataAtIndex(particles, i);
    if (particle->current->fitness > globalBest->fitness) {
      log_debug("New best global solution found with fitness value %f",
                particle->current);
      destroySolution(globalBest);
      globalBest = cloneSolution(particle->current);
    }
  }

  for (i = 0; i < getNumberElements(particles); i++) { // n particle
    log_debug("Update social best solution of particle %d", i);
    Particle particle = (Particle)getDataAtIndex(particles, i);
    destroySolution(particle->socialBest);
    particle->socialBest = cloneSolution(globalBest);
  }
}

bool initParticles(ArrayList particles, int dim, int n, double max, double min,
                   double v_max, double v_min,
                   double (*fitnessFunction)(ArrayList)) {
  int i;
  int rc;
  Particle particle;
  for (i = 0; i < n; i++) {
    log_debug("Initializing particle %d/%d", i + 1, n);
    particle = newParticle(i, dim, max, min, v_max, v_min, fitnessFunction);
    rc = checkAllocationError(particle);
    if (rc == FAILURE)
      return false;
    push_back(particles, particle);
  }
  return true;
}

void dumpParticles(ArrayList particles, Database db, const int iteration_step) {
  for (int i = 0; i < getNumberElements(particles); i++) { // n particles
    Particle particle = (Particle)getDataAtIndex(particles, i);
    insertSolution(db, particle->current, particle->id, iteration_step);
  }
}

void particleSwarmOptimization(ArrayList particles, int dimension,
                               int n_iterations, double w, double phi_1,
                               double phi_2,
                               double (*fitnessFunction)(ArrayList)) {
  int _;
  int i;
  Database db = newDatabase("./database.db", dimension);
  Solution globalBestSolution;
  globalBestSolution =
      cloneSolution(((Particle)getDataAtIndex(particles, 0))->current);

  for (_ = 0; _ < n_iterations; _++) { // n iterations
    updateGlobal(particles, globalBestSolution);
    for (i = 0; i < getNumberElements(particles); i++) { // n particles
      Particle particle = (Particle)getDataAtIndex(particles, i);
      updateVelocity(particle, w, phi_1, phi_2);
      log_debug("Iteration %d/%d particle %d/%d velocity updated", _ + 1,
                n_iterations, i + 1, getNumberElements(particles));
      log_debug("Iteration %d/%d particle %d/%d position updated", _ + 1,
                n_iterations, i + 1, getNumberElements(particles));
      updatePosition(particle, fitnessFunction);
    }
    log_info("Swarm optimization iteration %d/%d with best fitness %f", _ + 1,
             n_iterations, globalBestSolution->fitness);
    dumpParticles(particles, db, _);
  }
  destroyDatabase(db);
}
