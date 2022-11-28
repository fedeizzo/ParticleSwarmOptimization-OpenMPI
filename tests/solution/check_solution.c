#include "./check_solution.h"

START_TEST(test_newSolution) {
  Solution s;
  extern Solution newSolution();
  s = newSolution(5);
  ck_assert_int_eq(s->dimension, 5);
}
END_TEST

START_TEST(test_destroySolution) {
  Solution s;
  extern Solution newSolution();
  extern void destroySolution();
  s = newSolution(5);
  destroySolution(s);
}
END_TEST


START_TEST(test_cloneSolution) {
  Solution s;
  Solution s2;
  extern Solution newSolution();
  extern Solution cloneSolution();
  s = newSolution(5);
  s2 = cloneSolution(s);
  ck_assert_int_eq(s->dimension, s2->dimension);
  ck_assert_double_eq(s->fitness, s2->fitness);
  for (int i = 0; i < s->dimension; i ++)
    ck_assert_double_eq(s->pos[i], s2->pos[i]);
}
END_TEST

Suite *solution_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Solution");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_newSolution);
  tcase_add_test(tc_core, test_destroySolution);
  tcase_add_test(tc_core, test_cloneSolution);
  suite_add_tcase(s, tc_core);
  return s;
}
