#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../../include/config.h"
#include "../log/log.h"
#include "../solution/solution.h"
#include "../utils/utils.h"
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_MESSAGE_SOL 500

typedef struct timeval timestamp_t;

typedef struct solution_msg_t {
  int dim;
  double fitness;
  double pos[MAX_MESSAGE_SOL];
} solution_msg_t;

char *logSolutionMessage(solution_msg_t message);

typedef struct broadcastMessage_t {
  timestamp_t timestamp;
  int iteration;
  int particleId;
  // mpi_nodo
  int mpi_process;
  // mpi_thread
  solution_msg_t solution;
} broadcastMessage_t;

typedef broadcastMessage_t *BroadcastMessage;

MPI_Datatype define_datatype_broadcast_message(int dimension);
BroadcastMessage newBroadcastMessage();
MPI_Op maxFitnessFunction();
void initalizeBroacastMessage(BroadcastMessage message, const int process_id,
                              const int iteration, const int particleId,
                              Solution solution);
void destroyBroadcastMessage(BroadcastMessage message);
#endif
