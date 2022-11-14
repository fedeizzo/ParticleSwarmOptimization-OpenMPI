#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "../pso/pso.h"
#include "../solution/solution.h"
#include "../utils/utils.h"
#include <sqlite3.h>

typedef struct database_t {
  sqlite3 *db;
  sqlite3_stmt *insertSolutionStmt;
  sqlite3_stmt *insertExperimentStmt;
  int experiment_id;
} database_t;

typedef database_t *Database;

Database newDatabase(const char *filename, PSOData psoData,
                     const int threadsNumber, const int processesNumber);
void insertSolution(Database db, const Solution solution, const int particleId,
                    const int iteration_step);
void destroyDatabase(Database db);
#endif
