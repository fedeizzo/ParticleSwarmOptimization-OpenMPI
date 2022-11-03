/* #include "arraylist/arraylist.h" */
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

void openMP_tutorial() {
  // this is used to ignore optimization that can be made to spawn number of
  // threads specified even if not required
  omp_set_dynamic(0);
  // this sets the upper bound of threads that could be used for the whole
  // program can be also with environment variables
  omp_set_num_threads(16);
  // set nested parallelis on
  omp_set_nested(1);
  printf("Max threads number: %d\n", omp_get_max_threads());
  // number of threads can be specified direclty on the pragma
#pragma omp parallel num_threads(16)
  {
    int ID = omp_get_thread_num();
    printf("Hello %d\n", ID);
    printf("world %d\n", ID);
    int i = 0;
    // splits each iteartion of the loop on different threads
    // and variable i is private by deafult
#pragma omp for
    for (i = 0; i < 100; i++) {
      printf("%d\n", i);
    }
    // region after omp critical can be exectued only by one
    //  thread at the same time
    /* #pragma omp critical */
    // similar to critical but the mutual exlusion is only for
    // write purposes
    /* #pragma omp atomic */
    // each thread waits until all threads arrive
    // in some situations barrier is implicit, for exampel at the end of omp for
    // or at the end of a omp parallel region. This can be avoided adding nowait
    // to for directive
    /* #pragma omp barrier */
    /* #pragma omp for nowait */
  }
#pragma omp master
  {
    // block executed only by the master thread
  }
#pragma omp single
  {
    // block executed by only one thread
    // barrier at the end implied
  }
  // parallel and for pragma can be used togheter
#pragma omp parallel for
  for (int i = 0; i < 100; i++) {
    printf("%d\n", i);
  }

  double ave = 0.0, A[100];
  // a parallel loop where the ave varialbe is shared and a reduction
  // is requried given the fact that result is coposed iteratino after iteration
  // the plus before the colon means that the reduction operation is made with a
  // summation and the variable starts with a value of 0
#pragma omp parallel for reduction(+ : ave)
  for (int i = 0; i < 100; i++)
    ave += A[i];
  ave = ave / 100;
  // memory is managed specifing clause keywords
  //  - SHARED
  //  - PRIVATE
  //  - FIRSTPRIVATE
  //  - LASTPRIVATE
  //  - threadprivate
  // by default all variables are private
  int tmp = 0;
  // tmp is initialized with 0
#pragma omp parallel for private(tmp)
  for (int i = 0; i < 100; i++)
    tmp += i;
    // firstprivate initialize tmp with the original
    // value inside the main thread
#pragma omp parallel for firstprivate(tmp)
  for (int i = 0; i < 100; i++)
    tmp += i;
    // lastprivate copy the last iteration value of tmp
    // also outside the omp block and can then be usede
    // by the master thread
#pragma omp parallel for lastprivate(tmp)
  for (int i = 0; i < 100; i++)
    tmp += i;
  printf("%d\n", tmp);
  // threadprivate preserves global scope wihtin each thread
  // copyprivate can be used to propagate a private value to other threads

#pragma omp parallel
  {
// inside sections each section pragma define a different task that will be
// executed on a different thread
#pragma omp sections
    {
#pragma omp section
      printf("task number 1");
#pragma omp section
      printf("task number 2");
#pragma omp section
      printf("task number 3");
    }
  }

  // flush function can be used to force the update in memory of a variable
  // in this way all threads are ensured to have the same value updated
  // this is important because compilers move the order execution for
  // effficiency purposes
  omp_set_num_threads(5);
  omp_set_nested(1);
#pragma omp parallel sections
  {
#pragma omp section
    printf("task1");
#pragma omp section
    printf("task2");
#pragma omp section
#pragma omp parallel for
    for (int i = 0; i < 3; i++)
      printf("task3");
  }

  // ##################
  // TASK
  // ##################
  // A task is composed by:
  //  - code to execute
  //  - a data environment
  //  - an assigned thread that exectues the code and uses the data
  // and it is mainly divided into packaging and execution
  // In generale it is an advanced topic and I think it is not useful for our
  // purposes.
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

void task1(int particleId) {
  int max_iterations = 5;
  int iteration = 0;
  bool isMessageReadyToSend = false;
  do {
    do {
      /* printf("Particle %d task1 with thread number %d\n", particleId, */
      /*        omp_get_thread_num()); */
      // isMessageReadyToSend = check_if_ready();
      sleep(2);
    } while (!isMessageReadyToSend);
    /* MPI_broadcast(message); */
    // mpi broadcast and I am the root
    iteration++;
  } while (iteration != max_iterations);
}
void task2(int particleId) {
  do {
    printf("Particle %d task2 with thread number %d\n", particleId,
           omp_get_thread_num());
    sleep(2);
  } while (1 != 0);
}
void task3(int particleId) {
  do {
#pragma omp parallel for
    for (int i = 0; i < 5; i++)
      printf("Particle %d task3 iteration %d with thread number %d\n",
             particleId, i, omp_get_thread_num());
    sleep(2);
  } while (1 != 0);
}

int main(int argc, char **argv) {
  /* struct arguments arguments; */

  /* argp_parse(&argp, argc, argv, 0, 0, &arguments); */

  /* log_set_level(LOG_INFO); */
  /* log_info("Experiment name: %s, %d processes, %d threads for each process",
   */
  /*          arguments.experimentName, arguments.numberOfProcesses, */
  /*          arguments.numberOfThreads); */
  /* int num_dim = 2; */
  /* int num_particles = 100; */
  /* log_info("Particles initialization"); */
  /* ArrayList particles = newArrayList(); */
  /* initParticles(particles, num_dim, num_particles, 100.0, -100.0, 10.0, 0.1,
   */
  /*               prova); */
  /* log_info("Swarm optimization"); */
  /* particleSwarmOptimization(particles, num_dim, 100, 1.0, 0.5, 0.5, prova);
   */
  /* destroyArrayList(particles, destroyParticle); */
  /* yaml_parser_t parser; */
  /* yaml_event_t event; */
  /* int done = 0; */
  /* yaml_parser_initialize(&parser); */

  /* char *input = "./problem.yml"; */
  /* size_t length = strlen(input); */
  /* yaml_parser_set_input_string(&parser, input, length); */
  /* FILE *input = fopen("./problem.yml", "rb"); */

  /* yaml_parser_set_input_file(&parser, input); */
  /* openMP_tutorial(); */
  log_set_level(LOG_INFO);
  int problemDimension = 2;
  int particlesNumber = 16;
  int iterationsNumber = 10;
  int numberOfThreads = 8;
  int neighborhoodPopulation = 16;
  double w = 1;
  double phi_1 = 0.5;
  double phi_2 = 0.5;
  double initMaxPosition = 10;
  double initMinPosition = 0;
  double initMaxVelocity = 1;
  double initMinVelocity = 0.1;
  if (neighborhoodPopulation > particlesNumber) {
    log_error("Neighborhood population must be lower or equal than particles number");
    exit(1);
  }
  PSOData psoData = newPSOData(
      problemDimension, particlesNumber, iterationsNumber,
      neighborhoodPopulation, w, phi_1, phi_2, initMaxPosition, initMinPosition,
      initMaxVelocity, initMinVelocity, *sphere, *euclideanDistance, *minimize);
  int n_processes, process_id;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  /** EVERY PROCESS HAS A DIFFERENT RANDOM GENERATOR **/
  srand(85 + process_id);

  int particlesNumberPerProcess = psoData->particlesNumber / n_processes;
  int particlesNumberReminder = psoData->particlesNumber % n_processes;
  int processToNumberOfParticles[n_processes];
  for (int i = 0; i < n_processes; i++) {
    processToNumberOfParticles[i] = particlesNumberPerProcess;
    if (i != 0 && i <= particlesNumberReminder)
      processToNumberOfParticles[i]++;
  }

  int startingId = process_id * particlesNumberPerProcess;
  processRoutine(n_processes, numberOfThreads, process_id, startingId,
                 processToNumberOfParticles, psoData);

  /* int data = 5; */
  /* if (process_id == 0) { */
  /*   data = 10; */
  /* } else if (process_id == 2) { */
  /*   data = 20; */
  /* } */

  /* if (process_id == 0) { */
  /*   MPI_Request *request = (MPI_Request*) malloc(sizeof(MPI_Request)); */
  /*   // timestamp 0 */
  /*   MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, request); */
  /*   data = 15; */
  /*   sleep(5); */
  /*   // timestamp 5 */
  /*   MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, request); */
  /* } */
  /* else if (process_id == 1) { */
  /*   int completed_1 = 0, completed_2 = 0; */
  /*   MPI_Request *request[2]; */
  /*   MPI_Status status; */
  /*   for (int i = 0; i < 2; i++) */
  /*    request[i] = (MPI_Request*) malloc(sizeof(MPI_Request)); */
  /*   int data2; */
  /*   // timestamp 20 */
  /*   sleep(10); */
  /*   MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, request[0]); */
  /*   MPI_Ibcast(&data2, 1, MPI_INT, 0, MPI_COMM_WORLD, request[1]); */
  /*   do { */
  /*     MPI_Test(request[0], &completed_1, &status); */
  /*     printf("Completed req 0: %d\n", completed_1); */
  /*     MPI_Test(request[1], &completed_2, &status); */
  /*     printf("Completed req 1%d\n", completed_2); */
  /*     sleep(1); */
  /*   } while (completed_1 != 1 || completed_2 != 1); */
  /*   printf("process_id %d data %d %d\n", process_id, data, data2); */
  /* } else if (process_id == 2) { */
  /*   MPI_Request *request = (MPI_Request*) malloc(sizeof(MPI_Request)); */
  /*   sleep(1); */
  /*   // timestamp 1 */
  /*   MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, request); */
  /*   printf("process_id %d send data = %d\n", process_id, data); */
  /*   sleep(1); */
  /*   // timestamp 2 */
  /*   MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, request); */
  /*   printf("process_id %d send data = %d\n", process_id, data); */
  /* } */

  MPI_Finalize();
  return 0;
}
