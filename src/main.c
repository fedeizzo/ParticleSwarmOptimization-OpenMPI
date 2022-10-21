#include <argp.h>
#include <error.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Program name and version */
const char *argp_program_version = "HPC4DS 0.1";
/* Program documentation. */
static char doc[] = "A cooperant parallelized solution for Genetic Algorithm\n\
A tool that takes a set of continous or discrete variables and an optimization problem desgined \
to work with them. The goal is to find the optimal solution to complex \
optimization problem explointing Genetic Algorithms and the computational power offered by the cluster";

/* A description of the arguments we accept. */
static char args_doc[] = "ARRAY [SERIAL...]";

/* Keys for options without short-options. */
#define OPT_ABORT 1 /* –abort */

/* The options we understand. */
static struct argp_option options[] = {
    {"serial", 's', 0, OPTION_ARG_OPTIONAL,
     "Run a serial execution of the program"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  GArray *array; /* required double array */
  bool serial;   /* whether to run the serial computation --serial */
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
  case 's':
    arguments->serial = true; /* Serial execution */
    break;
  case ARGP_KEY_NO_ARGS:
    argp_usage(state); /* No argument specified */
  case ARGP_KEY_ARG:
    /* Here we know that state->arg_num == 0, since we
       force argument parsing to end before any more arguments can
       get here. */
    // Append the double data to the array
    // g_array_append_val(arguments->array, atof(arg));
    printf("\n%s\n", arg);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  struct arguments arguments;
  /* Default values. */
  arguments.serial = false;
  arguments.array = g_array_new(0, 0, sizeof(double));

  printf("Hello, World!\n");

  /* Parse our arguments; every option seen by parse_opt will be
     reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  g_array_free(arguments.array, true);
  return 0;
}
