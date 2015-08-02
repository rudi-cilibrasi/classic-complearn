#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/complearn/complearn.h"

START_TEST (test_complearn_environment)
{
  CompLearnEnvironment *cle = complearn_environment_top();
  fail_unless (cle != NULL,
    "Cannot have NULL CompLearnEnvironment");
}
END_TEST

Suite *
environment_suite (void)
{
  Suite *s = suite_create ("environment");

  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_complearn_environment);
  suite_add_tcase(s, tc_core);

  return s;
}
