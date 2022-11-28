#include "./check_sort.h"
#include <stdio.h>

START_TEST(test_partition) {
  int idxs[5] = {1, 2, 3, 4, 5};
  double distances[5] = {5, 1, 3, 2, 4};
  int p = 1;
  int r = 3;
  extern int partition();
  int result = partition(idxs, distances, p, r);
  ck_assert_int_eq(result, 3);
}
END_TEST

START_TEST(test_quicksort) {
  int *idxs = (int *)malloc(3 * sizeof(int));
  idxs[0] = 0;
  idxs[1] = 1;
  idxs[2] = 2;
  int *idxs_sorted = (int *)malloc(3 * sizeof(int));
  idxs_sorted[0] = 0;
  idxs_sorted[1] = 1;
  idxs_sorted[2] = 2;
  double *distances = (double *)malloc(3 * sizeof(double));
  distances[0] = 1;
  distances[1] = 2;
  distances[2] = 0;
  double *distances_sorted = (double *)malloc(3 * sizeof(double));
  distances_sorted[0] = 0;
  distances_sorted[1] = 1;
  distances_sorted[2] = 2;
  extern void quicksort();
  quicksort(idxs, distances, 0, 2);
  
/*   for (int i = 0; i < 3; i++) */
/*     printf("%f ", distances[i]); */
/* printf("\n"); */
  for (int i = 0; i < 3; i++) {
    /* ck_assert_int_eq(idxs_sorted[i], idxs[i]); */
    /* ck_assert_double_eq(distances_sorted[i], distances[i]); */
  }
  free(idxs);
  free(idxs_sorted);
  free(distances);
  free(distances_sorted);
}
END_TEST

Suite *sort_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Sort");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_partition);
  tcase_add_test(tc_core, test_quicksort);
  suite_add_tcase(s, tc_core);
  return s;
}
