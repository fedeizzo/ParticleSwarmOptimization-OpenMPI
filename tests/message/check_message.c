#include "./check_message.h"

START_TEST(test_newBrodcastMessage) {
  BroadcastMessage b;
  extern BroadcastMessage newBroadcastMessage();
  b = newBroadcastMessage();
  ck_assert_ptr_nonnull(b);
}
END_TEST

START_TEST(test_initializeBroadcastMessage) {
  extern BroadcastMessage newBroadcastMessage();
  extern Solution newSolution();
  extern void initalizeBroacastMessage();
  BroadcastMessage b;
  Solution s;
  int process_id = 0;
  int iteration = 0;
  int particleId = 0;
  b = newBroadcastMessage();
  s = newSolution(5);

  initalizeBroacastMessage(b, process_id, iteration, particleId, s);

  ck_assert_int_eq(b->mpi_process, process_id);
  ck_assert_int_eq(b->iteration, iteration);
  ck_assert_int_eq(b->particleId,particleId);
  ck_assert_int_eq(b->solution.dimension, s->dimension);
}
END_TEST

START_TEST(test_destroyBroadcastMessage) {
  BroadcastMessage b;
  extern BroadcastMessage newBroadcastMessage();
  extern void destroyBroadcastMessage();
  b = newBroadcastMessage();
  destroyBroadcastMessage(b);
}
END_TEST

Suite *message_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Message");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_newBrodcastMessage);
  tcase_add_test(tc_core, test_initializeBroadcastMessage);
  tcase_add_test(tc_core, test_destroyBroadcastMessage);
  suite_add_tcase(s, tc_core);
  return s;
}
