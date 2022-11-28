#include "message.h"

void reduceMaxFitness(BroadcastMessage in, BroadcastMessage inout, int *len,
                      MPI_Datatype *dptr);

MPI_Datatype define_double_array(int dimension) {
  MPI_Datatype DT_DOUBLE_ARRAY;
  MPI_Type_vector(MAX_MESSAGE_SOL, 1, MAX_MESSAGE_SOL, MPI_DOUBLE,
                  &DT_DOUBLE_ARRAY);
  MPI_Type_commit(&DT_DOUBLE_ARRAY);
  return DT_DOUBLE_ARRAY;
}

MPI_Datatype define_timeval_array() {
  timestamp_t timeElement = {.tv_sec = 0, .tv_usec = 0};
  MPI_Datatype DT_TIMEVAL;
  int lengths[2] = {1, 1};

  MPI_Aint displacements[2];
  MPI_Aint base_address;
  MPI_Get_address(&timeElement, &base_address);
  MPI_Get_address(&timeElement.tv_sec, &displacements[0]);
  MPI_Get_address(&timeElement.tv_usec, &displacements[1]);
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);
  MPI_Datatype types[2] = {MPI_LONG, MPI_LONG};
  MPI_Type_create_struct(2, lengths, displacements, types, &DT_TIMEVAL);
  MPI_Type_commit(&DT_TIMEVAL);
  return DT_TIMEVAL;
}

MPI_Datatype define_datatype_solution_message(int dimension) {
  solution_t solution;

  // Create the datatype
  MPI_Datatype DT_SOLUTION_MESSAGE;
  MPI_Datatype DT_DOUBLE_ARRAY = define_double_array(dimension);
  int lengths[3] = {1, 1, MAX_MESSAGE_SOL};

  MPI_Aint displacements[3];
  MPI_Aint base_address;
  MPI_Get_address(&solution, &base_address);
  MPI_Get_address(&solution.dimension, &displacements[0]);
  MPI_Get_address(&solution.fitness, &displacements[1]);
  MPI_Get_address(solution.pos, &displacements[2]);
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);
  displacements[2] = MPI_Aint_diff(displacements[2], base_address);
  MPI_Datatype types[3] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE};
  MPI_Type_create_struct(3, lengths, displacements, types,
                         &DT_SOLUTION_MESSAGE);
  MPI_Type_commit(&DT_SOLUTION_MESSAGE);
  MPI_Type_free(&DT_DOUBLE_ARRAY);
  return DT_SOLUTION_MESSAGE;
}

MPI_Datatype define_datatype_broadcast_message(int dimension) {
  broadcastMessage_t broadcast;
  // Create the datatype
  MPI_Datatype DT_BROADCAST_MESSAGE;
  MPI_Datatype DT_SOLUTION_MESSAGE =
      define_datatype_solution_message(dimension);
  MPI_Datatype DT_TIMEVAL = define_timeval_array();
  int lengths[5] = {1, 1, 1, 1, 1};

  MPI_Aint displacements[5];
  MPI_Aint base_address;
  MPI_Get_address(&broadcast, &base_address);
  MPI_Get_address(&broadcast.timestamp, &displacements[0]);
  MPI_Get_address(&broadcast.particleId, &displacements[1]);
  MPI_Get_address(&broadcast.mpi_process, &displacements[2]);
  MPI_Get_address(&broadcast.iteration, &displacements[3]);
  MPI_Get_address(&broadcast.solution, &displacements[4]);
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);
  displacements[2] = MPI_Aint_diff(displacements[2], base_address);
  displacements[3] = MPI_Aint_diff(displacements[3], base_address);
  displacements[4] = MPI_Aint_diff(displacements[4], base_address);
  MPI_Datatype types[5] = {DT_TIMEVAL, MPI_INT, MPI_INT, MPI_INT,
                           DT_SOLUTION_MESSAGE};
  MPI_Type_create_struct(5, lengths, displacements, types,
                         &DT_BROADCAST_MESSAGE);
  MPI_Type_commit(&DT_BROADCAST_MESSAGE);
  MPI_Type_free(&DT_TIMEVAL);
  MPI_Type_free(&DT_SOLUTION_MESSAGE);
  return DT_BROADCAST_MESSAGE;
}

char *logSolutionMessage(solution_t message) {
  char *str = (char *)malloc(sizeof(char) * 200);
  snprintf(str, 200, "Dim is %d fitness %f pos:", message.dimension,
           message.fitness);
  for (int i = 0; i < message.dimension; i++) {
    sprintf(str, "%s %f", str, message.pos[i]);
  }
  return str;
}

void includeSolution(solution_t *message, Solution solution) {
  message->fitness = solution->fitness;
  message->dimension = solution->dimension;
#pragma omp parallel for
  for (int i = 0; i < solution->dimension; i++)
    message->pos[i] = solution->pos[i];
}

BroadcastMessage newBroadcastMessage() {
  BroadcastMessage message =
      (BroadcastMessage)malloc(sizeof(broadcastMessage_t));
  return message;
}

void initalizeBroacastMessage(BroadcastMessage message, const int process_id,
                              const int iteration, const int particleId,
                              Solution solution) {
  message->iteration = iteration;
  message->particleId = particleId;
  message->mpi_process = process_id;
  gettimeofday(&message->timestamp, NULL);
  includeSolution(&(message->solution), solution);
}

void destroyBroadcastMessage(BroadcastMessage message) { free(message); }

void reduceMaxFitness(BroadcastMessage in, BroadcastMessage inout, int *len,
                      MPI_Datatype *dptr) {
  if (in->solution.fitness > inout->solution.fitness) {
    initalizeBroacastMessage(inout, in->mpi_process, in->iteration,
                             in->particleId, &in->solution);
  }
}
