/**
 * @file      message.h
 * @brief     MPI message definition
 * @date      Tue Nov  1 09:09:50 2022
 * @author    Federico Izzo, Samuele Bortolotti
 * @copyright MIT
 */

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

/**
 * @struct broadcastMessage_t
 * Broadcast message used by MPI broadcast
 */
typedef struct broadcastMessage_t {
  timestamp_t timestamp; /**< timestamp of the message */
  int iteration;         /**< iteration of particle swarm optimization */
  int particleId;        /**< particle id of the solution */
  int mpi_process;       /**< mpi process pid */
  solution_t solution;   /**< solution to the problem */
} broadcastMessage_t;

/**
 * @typedef BroadcastMessage
 * Pointer of a broadcast message
 */
typedef broadcastMessage_t *BroadcastMessage;

/**
 * @brief Generate MPI datatype
 * @details Generate MPI datatype for the broadcast message
 * @param[in] dimension dimension of the problem of particle swarm optimization
 * @return the created datatype for MPI
 */
MPI_Datatype define_datatype_broadcast_message(int dimension);

/**
 * @brief Create a new brodcast message
 * @return the created broadcast message
 */
BroadcastMessage newBroadcastMessage();
void initalizeBroacastMessage(BroadcastMessage message, const int process_id,
                              const int iteration, const int particleId,
                              Solution solution);

/**
 * @brief Destroy a broadcast message
 * @param[in] message the message to destroy
 */
void destroyBroadcastMessage(BroadcastMessage message);

/**
 * @brief Include a solution within a broadcast message
 * @details Make a deep copy of a solution within the solution_t contained in the broadcast message
 * @param[inout] message message in which the deep copy is done
 * @param[in] solution solution deep copied
 */
void includeSolution(solution_t *message, Solution solution);
BroadcastMessage cloneMessage(BroadcastMessage message);
BroadcastMessage cloneMessageStructToPointer(broadcastMessage_t msg);
broadcastMessage_t cloneMessageStructToStruct(broadcastMessage_t msg);
#endif
