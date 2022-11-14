#include "./database.h"
#include "../log/log.h"
#include "../solution/solution.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

const int STR_BUFFER_SIZE = 400;
const char *CREATE_EXPERIMENT_TABLE =
    "CREATE TABLE IF NOT EXISTS experiments ("
    "experiment_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
    "problem_dimension INTEGER NOT NULL,"
    "particles_number INTEGER NOT NULL,"
    "iterations_number INTEGER NOT NULL,"
    "threads_number INTEGER,"
    "processes_number INTEGER,"
    "neighborhood_population INTEGER NOT NULL,"
    "inertia REAL NOT NULL,"
    "phi_1 REAL NOT NULL,"
    "phi_2 REAL NOT NULL,"
    "init_max_position REAL NOT NULL,"
    "init_min_position REAL NOT NULL,"
    "init_max_velocity REAL NOT NULL,"
    "init_min_velocity REAL NOT NULL"
    ");";
const char *GET_EXPERIMENT_ID =
    "SELECT * FROM experiments ORDER BY experiment_id DESC LIMIT 1;";
const char *CREATE_SOLUTION_TABLE =
    "CREATE TABLE IF NOT EXISTS solutions_experiment_%d ("
    "solution_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
    "experiment_id INTEGER,"
    "particle_id INTEGER NOT NULL,"
    "iteration_step INTEGER NOT NULL,"
    "fitness REAL NOT NULL,"
    "timestamp DATETIME,"
    "FOREIGN KEY(experiment_id) REFERENCES experiments(experiment_id)"
    ");";
const char *SOLUTIONS_ADD_COLUMN =
    "ALTER TABLE solutions_experiment_%d ADD COLUMN dim_%d REAL NOT NULL;";
const char *SOLUTIONS_ADD_SOLUTION =
    "INSERT INTO solutions_experiment_%d(experiment_id, "
    "particle_id, iteration_step, fitness, timestamp, ";
const char *EXPERIMENT_ADD_EXPERIMENT =
    "INSERT INTO experiments(problem_dimension, particles_number, "
    "iterations_number, threads_number, processes_number, "
    "neighborhood_population, inertia, phi_1, phi_2, init_max_position, "
    "init_min_position, init_max_velocity, init_min_velocity) values "
    "(?,?,?,?,?,?,?,?,?,?,?,?,?)";

void createExperimentTable(sqlite3 *db) {
  log_info("Create experiment table");
  int rc = sqlite3_exec(db, CREATE_EXPERIMENT_TABLE, NULL, NULL, NULL);
  if (rc != SQLITE_OK)
    log_error("Error creating experiment table: %s", sqlite3_errmsg(db));
}

void createSolutionTable(Database db, const int solutionDimension) {
  log_info("Create solution table");
  char *query = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
  sprintf(query, CREATE_SOLUTION_TABLE, db->experiment_id);
  int rc = sqlite3_exec(db->db, query, NULL, NULL, NULL);
  if (rc != SQLITE_OK)
    log_error("Error creating solution table: %s", sqlite3_errmsg(db->db));

  for (int i = 0; i < solutionDimension; i++) {
    char *tmp = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
    sprintf(tmp, SOLUTIONS_ADD_COLUMN, db->experiment_id, i);
    int rc = sqlite3_exec(db->db, tmp, NULL, NULL, NULL);
    if (rc != SQLITE_OK)
      log_error("Error adding column solution table: %s",
                sqlite3_errmsg(db->db));
    free(tmp);
  }
}

void createInsertSolutionStatement(Database db, const int solutionDimension) {
  char *query = (char *)malloc(STR_BUFFER_SIZE * sizeof(char));
  sprintf(query, SOLUTIONS_ADD_SOLUTION, db->experiment_id);
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
  strcat(query, " VALUES (?, ?, ?, ?, datetime(), ");
  for (int i = 0; i < solutionDimension; i++) {
    if (i == solutionDimension - 1)
      strcat(query, "?);");
    else
      strcat(query, "?, ");
  }
  log_debug("%s\n", query);
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->db, query, -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    log_error("Error preparing solution table insert statement: %s",
              sqlite3_errmsg(db->db));
  db->insertSolutionStmt = stmt;
}

void createInsertExperimentStatement(Database db) {
  sqlite3_stmt *stmt;
  int rc =
      sqlite3_prepare_v2(db->db, EXPERIMENT_ADD_EXPERIMENT, -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    log_error("Error preparing solution table insert statement: %s",
              sqlite3_errmsg(db->db));
  db->insertExperimentStmt = stmt;
}

int getExperimentId(sqlite3 *db) {
  sqlite3_stmt *res;
  int rc = sqlite3_prepare_v2(db, GET_EXPERIMENT_ID, -1, &res, 0);
  if (rc != SQLITE_OK)
    log_error("Error retrieving experiment_id: %s", sqlite3_errmsg(db));

  rc = sqlite3_step(res);

  if (rc != SQLITE_ROW)
    log_error("Error retrieving experiment_id: %s", sqlite3_errmsg(db));
  return sqlite3_column_int(res, 0);
}

void insertSolution(Database db, const Solution solution, const int particleId,
                    const int iteration_step) {
  const int ELEMENT_TO_SKIP = 5;
  sqlite3_stmt *stmt;
  stmt = db->insertSolutionStmt;
  sqlite3_bind_int(stmt, 1, db->experiment_id);
  sqlite3_bind_int(stmt, 2, particleId);
  sqlite3_bind_int(stmt, 3, iteration_step);
  sqlite3_bind_double(stmt, 4, solution->fitness);
  int rc;
  for (int i = 0; i < solution->dimension; i++) {
    double value = solution->pos[i];
    sqlite3_bind_double(stmt, i + ELEMENT_TO_SKIP, value);
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE)
    log_error("Error executing solution table statement: %s",
              sqlite3_errmsg(db->db));
  sqlite3_reset(stmt);
}

void insertExperiment(Database db, PSOData psoData, const int threadsNumber,
                      const int processesNumber) {
  sqlite3_stmt *stmt;
  stmt = db->insertExperimentStmt;
  sqlite3_bind_int(stmt, 1, psoData->problemDimension);
  sqlite3_bind_int(stmt, 2, psoData->particlesNumber);
  sqlite3_bind_int(stmt, 3, psoData->iterationsNumber);
  if (threadsNumber > 0)
    sqlite3_bind_int(stmt, 4, threadsNumber);
  if (processesNumber > 0)
    sqlite3_bind_int(stmt, 5, processesNumber);
  sqlite3_bind_int(stmt, 6, psoData->neighborhoodPopulation);
  sqlite3_bind_double(stmt, 7, psoData->w);
  sqlite3_bind_double(stmt, 8, psoData->phi_1);
  sqlite3_bind_double(stmt, 9, psoData->phi_2);
  sqlite3_bind_double(stmt, 10, psoData->initMaxPosition);
  sqlite3_bind_double(stmt, 11, psoData->initMinPosition);
  sqlite3_bind_double(stmt, 12, psoData->initMaxVelocity);
  sqlite3_bind_double(stmt, 13, psoData->initMinVelocity);
  int rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE)
    log_error("Error executing experiment table statement: %s",
              sqlite3_errmsg(db->db));
  sqlite3_reset(stmt);
}
Database newDatabase(const char *filename, PSOData psoData,
                     const int threadsNumber, const int processesNumber) {
  Database database = (Database)malloc(sizeof(database_t));
  int rc = sqlite3_open(filename, &database->db);
  if (rc != SQLITE_OK) {
    log_error("Error opening database : %s", sqlite3_errmsg(database->db));
    exit(1);
  }
  log_info("Database opened");
  createExperimentTable(database->db);
  createInsertExperimentStatement(database);
  insertExperiment(database, psoData, threadsNumber, processesNumber);
  database->experiment_id = getExperimentId(database->db);
  createSolutionTable(database, psoData->problemDimension);
  createInsertSolutionStatement(database, psoData->problemDimension);
  return database;
}

void destroyDatabase(Database db) {
  sqlite3_finalize(db->insertSolutionStmt);
  sqlite3_close(db->db);
  free(db);
}
