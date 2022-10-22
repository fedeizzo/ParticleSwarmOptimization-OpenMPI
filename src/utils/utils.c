#include "utils.h"
#include "../../include/config.h"
#include <glib.h>
#include <stdio.h>

int checkAllocationError(void *ptr) {
  int rc_t = SUCCESS;
  if (ptr == NULL) {
    perror("Error during malloc allocation");
    rc_t = FAILURE;
  }

  return rc_t;
}

bool copyDoubleArray(GArray *source, GArray *destination) {
  int i;
  double *data;
  int rc;
  // Reset array
  g_array_free(destination, true);
  destination = g_array_new(false, false, sizeof(double));
  // Loop over source array
  for (i = 0; i < source->len; i++) {
    data = (double *)malloc(sizeof(double));
    rc = checkAllocationError(data);
    if (rc == FAILURE) {
      return false;
    }
    *data = source->data[i];
    g_array_append_val(destination, data);
  }
  return true;
}
