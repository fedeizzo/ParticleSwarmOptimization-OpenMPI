#include "./check_pso.h"

START_TEST(test_newPSOData) {
  PSOData d;
  extern PSOData newPSOData();
  d = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  ck_assert_ptr_nonnull(d);
}
END_TEST
START_TEST(test_newPSODataFromFile) {
  PSOData d;
  extern PSOData newPSODataFromFile();
  d = newPSODataFromFile("./pso-data.ini");
  ck_assert_ptr_nonnull(d);
}
END_TEST
START_TEST(test_destroyPSOData) {
  PSOData d;
  extern PSOData newPSOData();
  extern void destroyPSOData();
  d = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  destroyPSOData(d);
}

END_TEST
START_TEST(test_initParticles) {
  PSOData d;
  Particle particles[5];
  extern PSOData newPSOData();
  extern void initParticles();
  d = newPSOData(1, 1, 1, 1, 0.5, 0.5, 0.5, 3.0, -3.0, 3.0, -3.0, sphere,
                       euclideanDistance, minimize);
  initParticles(particles, 5, d, 0);
  for (int i = 0; i < 5; i++)
    ck_assert_int_eq(particles[i]->id, i);
}
END_TEST



Suite *pso_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("PSO");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_newPSOData);
  tcase_add_test(tc_core, test_destroyPSOData);
  tcase_add_test(tc_core, test_initParticles);
  suite_add_tcase(s, tc_core);
  return s;
}
