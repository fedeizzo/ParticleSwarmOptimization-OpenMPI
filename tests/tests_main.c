/*
- test function is a single function that typically uses ck_assert_* functions
- test case is a collection of test functions
- test suite is a collection of test cases
- suite runner is a function that takes a test suite and runs all test cases and
their test functions
*/

#include "./message/check_message.h"
#include "./solution/check_solution.h"

int check_suite(Suite(*suite(void))) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;
  s = suite();
  runner = srunner_create(s);
  srunner_run_all(runner, CK_NORMAL);
  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(void) {
  int rc[2];
  rc[0] = check_suite(message_suite);
  rc[1] = check_suite(solution_suite);

  for (int i = 0; i < 2; i++)
    if (rc[i] == EXIT_FAILURE)
      return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
