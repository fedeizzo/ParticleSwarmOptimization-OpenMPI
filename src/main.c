#include "log/log.h"
/* #include "particle/particle.h" */
#include "problems/problems.h"
#include "process/process.h"
#include "pso/pso.h"
/* #include "pso/multiprocess.h" */
#include <argp.h>
#include <error.h>
#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* Program name and version */
const char *argp_program_version = "HPC4DS 0.1";
/* Program documentation. */
static char doc[] =
    "A Cooperating parallelized solution for Genetic Algorithm\n\
A tool that takes a set of continuous or discrete variables and an optimization problem designed \
to work with them. The goal is to find the optimal solution \
by exploiting Genetic Algorithms and the computational power offered by the cluster";

/* A description of the arguments we accept. */
static char args_doc[] = "<experiment_name>";

/* Keys for options without short-options. */
#define OPT_ABORT 1 /* –abort */

/* The options we understand. */
static struct argp_option options[] = {
    {"number-of-processes", 'n', "COUNT", OPTION_ARG_OPTIONAL,
     "Number of processes"},
    {"number-of-threads", 'm', "COUNT", OPTION_ARG_OPTIONAL,
     "Number of threads for process"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  char *experimentName;
  int numberOfProcesses;
  int numberOfThreads;
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;
  unsigned int eax = 11, ebx = 0, ecx = 1, edx = 0;

  asm volatile("cpuid"
               : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
               : "0"(eax), "2"(ecx)
               :);
  switch (key) {
  case 'n':
    arguments->numberOfProcesses = arg ? atoi(arg) : eax;
    break;
  case 'm':
    arguments->numberOfThreads = arg ? atoi(arg) : edx;
    break;
  case ARGP_KEY_NO_ARGS:
    argp_usage(state); /* No argument specified */
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1)
      argp_usage(state);
    arguments->experimentName = arg;
    break;
  case ARGP_KEY_END:
    /* argp_usage(state); */
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

void printMatrix(int a[5][5]) {
  for (int i = 0; i < 5; i++) {
    printf("[ ");
    for (int j = 0; j < 5; j++)
      printf("%d, ", a[i][j]);
    printf("]\n");
  }
}

int main(int argc, char **argv) {
  /* struct arguments arguments; */

  /* argp_parse(&argp, argc, argv, 0, 0, &arguments); */

  /* log_set_level(LOG_INFO); */
  /* log_info("Experiment name: %s, %d processes, %d threads for each process",
   */
  /*          arguments.experimentName, arguments.numberOfProcesses, */
  /*          arguments.numberOfThreads); */
  int problemDimension = 10;
  int particlesNumber = 6;
  int iterationsNumber = 10;
  int numberOfThreads = 2;
  int neighborhoodPopulation = 3;
  double w = 1;
  double phi_1 = 0.8;
  double phi_2 = 0.8;
  double initMaxPosition = 30.0;
  double initMinPosition = -30.0;
  double initMaxVelocity = 10;
  double initMinVelocity = -10;
  bool USE_OPENMPI = true;

  if (neighborhoodPopulation > particlesNumber) {
    log_error(
        "Neighborhood population must be lower or equal than particles number");
    exit(1);
  }
  PSOData psoData = newPSOData(
      problemDimension, particlesNumber, iterationsNumber,
      neighborhoodPopulation, w, phi_1, phi_2, initMaxPosition, initMinPosition,
      initMaxVelocity, initMinVelocity, *sphere, *euclideanDistance, *minimize);
  if (!USE_OPENMPI) {
    srand(0);
    // #################################################
    // # NO OPENMPI or openMP solution                 #
    // #################################################
    /* log_info("Particles initialization"); */
    log_set_level(LOG_INFO);
    Particle particles[psoData->particlesNumber];
    initParticles(particles, psoData, 0);
    particleSwarmOptimization(particles, psoData);
  } else {
    log_set_level(LOG_ERROR);
    // #################################################
    // # OPENMPI or openMP solution                    #
    // #################################################
    int n_processes, process_id;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    // Impossible to paralleize
    if (n_processes > particlesNumber) {
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
                   processToNumberOfParticles, psoData);
    MPI_Finalize();
  }
  return 0;
}
