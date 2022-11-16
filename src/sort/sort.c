int partition(int *neighborhoodIndex, double *distances, int p, int r) {
  int lt[r - p];
  int gt[r - p];
  int key = neighborhoodIndex[r];
  int i;
  int j;
  int lt_n = 0;
  int gt_n = 0;

  for (i = p; i < r; i++) {
    if (distances[neighborhoodIndex[i]] < distances[neighborhoodIndex[r]]) {
      lt[lt_n++] = neighborhoodIndex[i];
    } else {
      gt[gt_n++] = neighborhoodIndex[i];
    }
  }

  for (i = 0; i < lt_n; i++) {
    neighborhoodIndex[p + i] = lt[i];
  }

  neighborhoodIndex[p + lt_n] = key;

  for (j = 0; j < gt_n; j++) {
    neighborhoodIndex[p + lt_n + j + 1] = gt[j];
  }

  return p + lt_n;
}

void quicksort(int *neighborhoodIndex, double *distances, int p, int r) {
  int div;

  if (p < r) {
    div = partition(neighborhoodIndex, distances, p, r);
#pragma omp task shared(neighborhoodIndex, distances)
    quicksort(neighborhoodIndex, distances, p, div - 1);
#pragma omp task shared(neighborhoodIndex, distances)
    quicksort(neighborhoodIndex, distances, div + 1, r);
  }
}
