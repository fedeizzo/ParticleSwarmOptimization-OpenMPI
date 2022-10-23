#include "./database.h"
#include "../log/log.h"
#include "../solution/solution.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

const int STR_BUFFER_SIZE = 200;
const char *CREATE_SOLUTION_TABLE =
    "CREATE TABLE solutions ("
    "solution_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
    "particle_id INTEGER NOT NULL,"
    "iteration_step INTEGER NOT NULL,"
    "fitness REAL NOT NULL"
    ");";
const char *SOLUTIONS_ADD_COLUMN =
    "ALTER TABLE solutions ADD COLUMN dim_%d REAL NOT NULL;";
const char *SOLUTIONS_ADD_SOLUTION =
    "INSERT INTO solutions(particle_id, iteration_step, fitness, ";

void createSolutionTable(sqlite3 *db, const int solutionDimension) {
  log_info("Create solution table");
  int rc = sqlite3_exec(db, CREATE_SOLUTION_TABLE, NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    log_error("Error creating solution table: %s", sqlite3_errmsg(db));
    exit(1);
  }

  for (int i = 0; i < solutionDimension; i++) {
    char *tmp = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
    sprintf(tmp, SOLUTIONS_ADD_COLUMN, i);
    int rc = sqlite3_exec(db, tmp, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
      log_error("Error adding column solution table: %s", sqlite3_errmsg(db));
      exit(1);
    }
    free(tmp);
  }
}

void createInsertStatement(Database db, const int solutionDimension) {
  char *query = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
  strcpy(query, SOLUTIONS_ADD_SOLUTION);
  for (int i = 0; i < solutionDimension; i++) {
    char *tmp = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
    if (i == solutionDimension - 1) {
      sprintf(tmp, "dim_%d)", i);
      strcat(query, tmp);
    } else {
      sprintf(tmp, "dim_%d, ", i);
      strcat(query, tmp);
    }
    free(tmp);
  }
  strcat(query, " VALUES (?, ?, ?, ");
  for (int i = 0; i < solutionDimension; i++) {
    if (i == solutionDimension - 1)
      strcat(query, "?);");
    else
      strcat(query, "?, ");
  }
  log_debug("%s\n", query);
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->db, query, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    log_error("Error preparing solution table insert statement: %s",
              sqlite3_errmsg(db->db));
    exit(1);
  }
  db->insertStmt = stmt;
}

void insertSolution(Database db, const Solution solution,
                    const int particleId, const int iteration_step) {
  const int ELEMENT_TO_SKIP = 4;
  sqlite3_stmt *stmt;
  stmt = db->insertStmt;
  sqlite3_bind_int(stmt, 1, particleId);
  sqlite3_bind_int(stmt, 2, iteration_step);
  sqlite3_bind_double(stmt, 3, solution->fitness);
  int rc;
  for (int i = 0; i < getNumberElements(solution->pos); i++) {
    double value = *((double *)getDataAtIndex(solution->pos, i));
    sqlite3_bind_double(stmt, i + ELEMENT_TO_SKIP, value);
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    log_error("Error executing solution table statement: %s",
              sqlite3_errmsg(db->db));
    exit(1);
  }
  sqlite3_reset(stmt);
}

Database newDatabase(const char *filename, const int solutionDimension) {
  Database database = (Database)malloc(sizeof(database_t));
  int rc = sqlite3_open(filename, &database->db);
  if (rc != SQLITE_OK) {
    log_error("Error opening database : %s", sqlite3_errmsg(database->db));
    exit(1);
  }
  log_info("Database opened");
  createSolutionTable(database->db, solutionDimension);
  createInsertStatement(database, solutionDimension);
  return database;
}

void destroyDatabase(Database db) {
  sqlite3_finalize(db->insertStmt);
  sqlite3_close(db->db);
  free(db);
}
