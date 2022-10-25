#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../solution/solution.h"
#include <sys/time.h>

typedef struct broadcastMessage_t {
  timeval timestamp;
  int iteration;
  int particleId;
  // mpi_nodo
  int mpi_process;
  // mpi_thread
  Solution solution;
} broadcastMessage_t;

typedef broadcastMessage_t *BroadcastMessage;

BroadcastMessage newMessage(const int iteration, const int particleId, Solution solution);
void destroyMessage(BroadcastMessage message);
#endif
