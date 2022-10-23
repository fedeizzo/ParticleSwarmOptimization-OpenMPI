#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "../solution/solution.h"
#include <sqlite3.h>

typedef struct database_t {
  sqlite3 *db;
  sqlite3_stmt *insertStmt;
} database_t;

typedef database_t *Database;

Database newDatabase(const char *filename, const int solutionDimension);
void insertSolution(Database db, const Solution solution, const int particleId,
                    const int iteration_step);
void destroyDatabase(Database db);
#endif
