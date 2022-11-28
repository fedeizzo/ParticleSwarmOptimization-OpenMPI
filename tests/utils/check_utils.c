#include "./check_utils.h"

/* START_TEST(test_newBrodcastMessage) { */
/*   BroadcastMessage b; */
/*   extern BroadcastMessage newBroadcastMessage(); */
/*   b = newBroadcastMessage(); */
/*   ck_assert_ptr_nonnull(b); */
/* } */
/* END_TEST */

Suite *utils_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Utils");
  tc_core = tcase_create("Core");

  /* tcase_add_test(tc_core, test_newBrodcastMessage); */
  suite_add_tcase(s, tc_core);
  return s;
}
