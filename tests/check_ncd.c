#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/complearn/complearn.h"
#include "../src/complearn/complearn-external-file-compressor.h"

START_TEST (test_complearn_ncd_external_file_compressor)
{
  CompLearnExternalFileCompressor *ef =COMPLEARN_EXTERNAL_FILE_COMPRESSOR(complearn_external_file_compressor_new());
  GString *result;
  result = complearn_external_file_compressor_transform_with_external_command(ef,
    "tar cvfj out.tar.bz2 inf", complearn_read_whole_file("/bin/ls"), "inf", "out.tar.bz2");
  fail_unless (result != NULL, "Cannot have NULL result");
}
END_TEST

START_TEST (test_complearn_ncd)
{
  CompLearnNcd *ncd = complearn_ncd_top();
  fail_unless (ncd != NULL,
    "Cannot have NULL CompLearnNcd");
}
END_TEST

START_TEST (test_complearn_ncd_literal)
  complearn_ncd_set_literal_mode ();
  complearn_ncd_set_zero_diagonal(complearn_ncd_top(), FALSE);
  complearn_ncd_add_argument ("hello");
  complearn_ncd_add_argument ("hello");
  LabeledMatrix *lm;
  lm = complearn_ncd_ncd_matrix(
complearn_ncd_get_argument(0),
complearn_ncd_get_argument(1));
  fail_if (lm == NULL);
  fail_if (lm->mat == NULL);
  fail_if (lm->mat->size1 != 1 || lm->mat->size2 != 1,
    "Error, got matrix of %dx%d expected 1x1\n", lm->mat->size1,lm->mat->size2);
  fail_if (gsl_matrix_get(lm->mat, 0, 0) == 0.0);
  complearn_ncd_set_zero_diagonal(complearn_ncd_top(), TRUE);
  lm = complearn_ncd_ncd_matrix(
complearn_ncd_get_argument(0),
complearn_ncd_get_argument(1));
  fail_if (lm == NULL);
  fail_if (lm->mat == NULL);
  fail_if (lm->mat->size1 != 1 || lm->mat->size2 != 1,
    "Error, got matrix of %dx%d expected 1x1\n", lm->mat->size1,lm->mat->size2);
  fail_if (gsl_matrix_get(lm->mat, 0, 0) != 0.0);
END_TEST

Suite *
ncd_suite (void)
{
  Suite *s = suite_create ("ncd");

  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_complearn_ncd);
  tcase_add_test (tc_core, test_complearn_ncd_literal);
  tcase_add_test (tc_core, test_complearn_ncd_external_file_compressor);
  suite_add_tcase(s, tc_core);

  return s;
}

