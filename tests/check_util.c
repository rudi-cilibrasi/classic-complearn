#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <check.h>
#include "../src/complearn/complearn.h"

START_TEST (test_complearn_util_dir1)
  char *tnam;
  int rv;
  tnam = complearn_make_temp_dir_name();
  fail_if (tnam == NULL);
  fail_unless (strlen(tnam) > 0);
  rv = complearn_remove_directory_recursively(tnam,0);
  fail_if (rv == 0);
  rv = complearn_make_directory_if_necessary(tnam);
  fail_unless(rv == 0);
  rv = complearn_make_directory_if_necessary(tnam);
  fail_unless(rv == 0);
  rv = complearn_remove_directory_recursively(tnam,0);
  fail_unless (rv == 0);
  rv = complearn_remove_directory_recursively(tnam,0);
  fail_if (rv == 0);
END_TEST

START_TEST (test_complearn_util_dir2)
  char *tnam, *c1, *c2, *t2;
  int rv;
  tnam = complearn_make_temp_dir_name();
  rv = complearn_make_directory_if_necessary(tnam);
  fail_unless(rv == 0);
  c1 = complearn_chdir(tnam);
  fail_if(c1 == NULL);
  fail_if(strlen(c1) <= 0);
  c2 = complearn_chdir(c1);
  fail_unless(strcmp(tnam, c2) == 0);
  t2 = complearn_make_temp_dir_name();
  fail_if(strcmp(tnam, t2) == 0);
END_TEST

Suite *
util_suite (void)
{
  Suite *s = suite_create ("util");

  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_complearn_util_dir2);
  tcase_add_test (tc_core, test_complearn_util_dir1);
  suite_add_tcase(s, tc_core);

  return s;
}

