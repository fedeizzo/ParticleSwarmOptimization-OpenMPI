#include "utils.h"

int checkAllocationError(void *ptr) {
  int rc_t = SUCCESS;
  if (ptr == NULL) {
    perror("Error during malloc allocation");
    rc_t = FAILURE;
  }

  return rc_t;
}

bool copyDoubleArray(ArrayList source, ArrayList destination,
                     void deleteData(void *)) {
  int i;
  double *data;
  int rc;
  // Reset array
  emptyArray(destination, deleteData);
  // Loop over source array
  for (i = 0; i < getNumberElements(source); i++) {
    data = (double *)malloc(sizeof(double));
    rc = checkAllocationError(data);
    if (rc == FAILURE) {
      return false;
    }
    *data = (*(double *)getDataAtIndex(source, i));
    push_back(destination, data);
  }
  return true;
}

void die(char *msg) {
  log_error(msg);
  exit(1);
}

void printDouble(void *ptr) {
  double *data = (double *)ptr;
  printf("%f\n", *data);
}

void printDoubleInLine(void *ptr) {
  double *data = (double *)ptr;
  printf("%f ", *data);
}

double randfrom(double min, double max) {
  double range = (max - min);
  double div = RAND_MAX / range;
  return min + (rand() / div);
}
