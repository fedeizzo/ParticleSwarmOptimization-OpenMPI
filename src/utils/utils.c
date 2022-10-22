#include "utils.h"
#include "../../include/config.h"
#include <stdio.h>
#include <stdlib.h>

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

void printDouble(void *ptr) {
  double *data = (double *)ptr;
  printf("%f\n", *data);
}
