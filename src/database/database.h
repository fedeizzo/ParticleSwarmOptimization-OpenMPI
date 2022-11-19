/**
 * @file      database.h
 * @brief     Database definition
 * @date      Sat Nov 19 14:24:32 2022
 * @author    Federico Izzo, Samuele Bortolotti
 * @copyright MIT
 */

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "../log/log.h"
#include "../pso/pso.h"
#include "../solution/solution.h"
#include "../utils/utils.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct database_t
 * Contains database stub and associated statements
 */
typedef struct database_t {
  sqlite3 *db;                        /**< Database stub */
  sqlite3_stmt *insertSolutionStmt;   /**< Statement for the solution creation */
  sqlite3_stmt *insertExperimentStmt; /**< Statement for the experiment creation */
  int experiment_id;                  /**< Experiment id */
} database_t;

typedef database_t *Database;

/**
 * @brief Generatse a new database
 * @param[in] filename database location
 * @param[in] psoData initialization data associated to the particle swarm optimization problem
 * @param[in] threadsNumber number of threads
 * @param[in] processesNumber number of processes
 * @return returns the created database
 */
Database newDatabase(const char *filename, PSOData psoData,
                     const int threadsNumber, const int processesNumber);

/**
 * @brief Inserts a new solution inside the database
 * @param[in] db database in which the solution is saved
 * @param[in] solution solution saved
 * @param[in] particleId particle id associated to the solution
 * @param[in] iteration_step iteration associated to the solution
 */
void insertSolution(Database db, const Solution solution, const int particleId,
                    const int iteration_step);

/**
 * @brief Destroies the passed database
 * @param[in] db database to destroy
 */
void destroyDatabase(Database db);
#endif
