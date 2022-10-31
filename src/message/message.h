#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../../include/config.h"
#include "../log/log.h"
#include "../solution/solution.h"
#include "../utils/utils.h"
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct timeval timestamp_t;

char *logSolutionMessage(solution_t message);

typedef struct broadcastMessage_t {
  timestamp_t timestamp;
  int iteration;
  int particleId;
  int mpi_process;
  solution_t solution;
} broadcastMessage_t;

typedef broadcastMessage_t *BroadcastMessage;

MPI_Datatype define_datatype_broadcast_message(int dimension);
BroadcastMessage newBroadcastMessage();
MPI_Op maxFitnessFunction();
void initalizeBroacastMessage(BroadcastMessage message, const int process_id,
                              const int iteration, const int particleId,
                              Solution solution);
void destroyBroadcastMessage(BroadcastMessage message);

void includeSolution(solution_t *message, Solution solution);
BroadcastMessage cloneMessage(BroadcastMessage message);
BroadcastMessage cloneMessageStructToPointer(broadcastMessage_t msg);
broadcastMessage_t cloneMessageStructToStruct(broadcastMessage_t msg);
#endif
