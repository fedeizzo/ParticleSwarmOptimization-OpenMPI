#include "./check_database.h"

START_TEST(test_newDatabase) {
  Database db;
  PSOData psoData;
  extern Database newDatabase();
  extern PSOData newPSOData();
  psoData = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  db = newDatabase("/tmp/test.db", psoData, 1, 1);
  ck_assert_ptr_nonnull(db);
}
END_TEST

START_TEST(test_destroyDatabase) {
  Database db;
  PSOData psoData;
  extern Database newDatabase();
  extern PSOData newPSOData();
  extern void destroyDatabase();
  psoData = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  db = newDatabase("/tmp/test.db", psoData, 1, 1);
  destroyDatabase(db);
}
END_TEST

START_TEST(test_insertSolution) {
  Database db;
  PSOData psoData;
  Solution s;
  extern Database newDatabase();
  extern PSOData newPSOData();
  extern Solution newSolution();
  extern void insertSolution();
  psoData = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  db = newDatabase("/tmp/test.db", psoData, 1, 1);
  s = newSolution(5);
  insertSolution(db, s, 0, 0);
}
END_TEST

Suite *database_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Database");
  tc_core = tcase_create("Core");

  /* tcase_add_test(tc_core, test_newDatabase); */
  /* tcase_add_test(tc_core, test_destroyDatabase); */
  /* tcase_add_test(tc_core, test_insertSolution); */
  suite_add_tcase(s, tc_core);
  return s;
}
