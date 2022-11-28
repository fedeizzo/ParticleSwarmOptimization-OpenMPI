#include "./check_problems.h"

START_TEST(test_sphere) {
  double input[5] = {1,2,3,4,5};
  extern double sphere();
  double result = sphere(input, 5);
  ck_assert_double_eq(result, 55);
}
END_TEST

START_TEST(test_wave) {
  double input[5] = {1,2,3,4,5};
  extern double wave();
  double result = wave(input, 5);
  ck_assert_double_eq(result, 55);
}
END_TEST

START_TEST(test_Easom) {
  double input[5] = {1,2,3,4,5};
  extern double Easom();
  double result = Easom(input, 5);
  ck_assert_double_eq_tol(result, 0.00062, 5);
}
END_TEST

START_TEST(test_Ackley) {
  double input[5] = {1,2,3,4,5};
  extern double Ackley();
  double result = Ackley(input, 5);
  ck_assert_double_eq_tol(result, 5.42213, 5);
}
END_TEST

START_TEST(test_Himmelblau) {
  double input[5] = {1,2,3,4,5};
  extern double Himmelblau();
  double result = Himmelblau(input, 5);
  ck_assert_double_eq(result, 68);
}
END_TEST

START_TEST(test_HolderTable) {
  double input[5] = {1,2,3,4,5};
  extern double HolderTable();
  double result = HolderTable(input, 5);
  ck_assert_double_eq_tol(result, -0.46716, 5);
}
END_TEST

START_TEST(test_euclideanDistance) {
  double p1[5] = {1,2,3,4,5};
  double p2[5] = {8,1,46,8,9};
  extern double euclideanDistance();
  double result = euclideanDistance(p1, p2, 5);
  ck_assert_double_eq_tol(result, 43.9431, 4);
}
END_TEST

START_TEST(test_minimize) {
  extern bool minimize();
  bool result = minimize(1, 2);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(test_maximize) {
  extern bool maximize();
  bool result = maximize(2, 1);
  ck_assert_int_eq(result, 1);
}
END_TEST

Suite *problems_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Problems");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_sphere);
  tcase_add_test(tc_core, test_wave);
  tcase_add_test(tc_core, test_Easom);
  tcase_add_test(tc_core, test_Ackley);
  tcase_add_test(tc_core, test_Himmelblau);
  tcase_add_test(tc_core, test_HolderTable);
  tcase_add_test(tc_core, test_euclideanDistance);
  tcase_add_test(tc_core, test_minimize);
  tcase_add_test(tc_core, test_maximize);
  suite_add_tcase(s, tc_core);
  return s;
}
