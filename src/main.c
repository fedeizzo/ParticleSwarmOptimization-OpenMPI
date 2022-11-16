#include "log/log.h"
#include "problems/problems.h"
#include "process/process.h"
#include "pso/pso.h"
#include <argp.h>
#include <error.h>
#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

const char *argp_program_version = "ParticleSwarmOptimization-OpenMPI 0.1";
static char doc[] =
    "A Cooperating parallelized solution for Genetic Algorithm\n\
A tool that takes a set of continuous or discrete variables and an optimization problem designed \
to work with them. The goal is to find the optimal solution \
by exploiting Genetic Algorithms and the computational power offered by the cluster";

static char args_doc[] = "<config-file> <database-file>";

/* Keys for options without short-options. */
#define OPT_ABORT 1 /* –abort */

/* The options we understand. */
static struct argp_option options[] = {
    {"number-of-threads", 'm', "COUNT", OPTION_ARG_OPTIONAL,
     "Number of threads for process"},
    {"use-openmpi", 'u', 0, 0, "Use OpenMPI"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  char *configFile;
  char *databaseFile;
  bool useOpenMPI;
  int numberOfThreads;
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
  case 'm':
    arguments->numberOfThreads = arg ? atoi(arg) : -1;
    break;
  case 'u':
    arguments->useOpenMPI = true;
    break;
  case ARGP_KEY_NO_ARGS:
    argp_usage(state); /* No argument specified */
  case ARGP_KEY_ARG:
    if (state->arg_num >= 2)
      argp_usage(state);
    else if (state->arg_num == 0)
      arguments->configFile = arg;
    else
      arguments->databaseFile = arg;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1) {
      argp_usage(state);
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

bool isValidFile(const char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    fclose(fp);
    return false;
  } else {
    fclose(fp);
    return true;
  }
}

int main(int argc, char **argv) {
  struct arguments arguments;
  arguments.numberOfThreads = 1;
  arguments.useOpenMPI = false;
  arguments.databaseFile = "";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  int numberOfThreads = arguments.numberOfThreads;
  log_set_level(LOG_ERROR);

  if (!isValidFile(arguments.configFile)) {
    log_error("%-10s :: %s", "INIT", "Provide valid config file");
    exit(1);
  }
  PSOData psoData = newPSODataFromFile(arguments.configFile);

  if (psoData->neighborhoodPopulation > psoData->particlesNumber) {
    log_error(
        "%-10s :: %s", "INIT",
        "Neighborhood population must be lower or equal than particles number");
    exit(1);
  }
  log_info("%-10s :: %s", "INIT",
           arguments.useOpenMPI == 1 ? "Using parallel openMPI version"
                                     : "Using serial version");
  if (!arguments.useOpenMPI) {
    srand(0);
    omp_set_num_threads(0);
    omp_set_max_active_levels(0);
    // #################################################
    // # NO OPENMPI or openMP solution                 #
    // #################################################
    Particle particles[psoData->particlesNumber];
    initParticles(particles, psoData->particlesNumber, psoData, 0);
    particleSwarmOptimization(particles, psoData, arguments.databaseFile);
  } else {
    // #################################################
    // # OPENMPI or openMP solution                    #
    // #################################################
    int n_processes, process_id;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    // Impossible to paralleize
    if (n_processes > psoData->particlesNumber) {
      if (process_id == 0)
        log_error(
            "Processes number must be lower or equal than particles number");
      MPI_Finalize();
      exit(1);
    }

    /** EVERY PROCESS HAS A DIFFERENT RANDOM GENERATOR **/
    srand(0 + process_id * 10);

    // Partition the particles equally: each process should have the same,
    // except fo tthe process 0 which is prioritize to have 1 less
    int particlesNumberPerProcess = psoData->particlesNumber / n_processes;
    int particlesNumberReminder = psoData->particlesNumber % n_processes;
    int processToNumberOfParticles[n_processes];
    // TODO si può parallelizzare
    for (int i = 0; i < n_processes; i++) {
      processToNumberOfParticles[i] = particlesNumberPerProcess;
      if (i != 0 && i <= particlesNumberReminder)
        processToNumberOfParticles[i]++;
    }

    // Otteniamo l'id della particella
    int startingId = process_id * particlesNumberPerProcess;
    processRoutine(n_processes, numberOfThreads, process_id, startingId,
                   processToNumberOfParticles, psoData, arguments.databaseFile);
    MPI_Finalize();
  }
  return 0;
}
