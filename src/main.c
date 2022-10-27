/* #include "arraylist/arraylist.h" */
#include "log/log.h"
/* #include "particle/particle.h" */
/* #include "problems/problems.h" */
/* #include "pso/pso.h" */
/* #include "pso/multiprocess.h" */
#include <argp.h>
#include <error.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
  int i;
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


#pragma omp prallel
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
  // this is important because compilers move the order execution for effficiency
  // purposes

  // ##################
  // TASK
  // ##################
  // A task is composed by:
  //  - code to execute
  //  - a data environment
  //  - an assigned thread that exectues the code and uses the data
  // and it is mainly divided into packaging and execution
  // In generale it is an advanced topic and I think it is not useful for our purposes.
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  struct arguments arguments;
  srand(85);

  /* argp_parse(&argp, argc, argv, 0, 0, &arguments); */

  /* log_set_level(LOG_INFO); */
  /* log_info("Experiment name: %s, %d processes, %d threads for each process",
   */
  /*          arguments.experimentName, arguments.numberOfProcesses, */
  /*          arguments.numberOfThreads); */
  /* log_set_level(LOG_DEBUG); */
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

  /* openMP_tutorial(); */
  return 0;
}
