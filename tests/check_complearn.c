#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/complearn/complearn.h"

#include "suite.h"

Suite *
complearn_suite (void)
{
  Suite *s = suite_create ("master");
  complearn_init();
  complearn_init();
  return s;
}

int
main (void)
{
  int number_failed;
  g_type_init();
  complearn_init();
  Suite *s = complearn_suite ();
  SRunner *sr = srunner_create (s);
  srunner_add_suite (sr, environment_suite ());
  srunner_add_suite (sr, ncd_suite ());
  srunner_add_suite (sr, util_suite ());
  srunner_add_suite (sr, cloutput_suite ());
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
