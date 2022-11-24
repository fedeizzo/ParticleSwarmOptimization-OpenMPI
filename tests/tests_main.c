/*
- test function is a single function that typically uses ck_assert_* functions
- test case is a collection of test functions
- test suite is a collection of test cases
- suite runner is a function that takes a test suite and runs all test cases and
their test functions
*/

#include "./message/check_message.h"

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;
  s = message_suite();
  runner = srunner_create(s);
  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
