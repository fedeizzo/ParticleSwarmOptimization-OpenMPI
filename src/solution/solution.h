#ifndef __SOLUTION_H__
#define __SOLUTION_H__
#include "../../include/config.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * @struct solution_t
 * Contains a solution to the problem
 */
typedef struct solution_t {
  int dimension;               /**< dimension of the problem */
  double fitness;              /**< fitness of the solution */
  double pos[MAX_MESSAGE_SOL]; /**< the position of the solution */
} solution_t;

typedef solution_t *Solution;

/**
 * @brief Creates a new solution
 * @return returns the crated solution
 */
Solution newSolution();

/**
 * @brief Destroys the passed solution
 * @param[in] solution solution to destroy
 */
void destroySolution(Solution solution);

/**
 * @brief Clones the passed solution allocating a new one on the heap
 * @param[in] solution solution to clone
 * @return returns the new crated solution
 */
Solution cloneSolution(Solution solution);

/**
 * @brief Prints the passed solution
 * @param[in] solution solution to print
 */
void printSolution(Solution solution);
#endif
