#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "../src/complearn/complearn.h"

static char *dmstr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
"<clb version=\"1.0\"><cllibver>0.9.9</cllibver><username>cilibrar</username><hostname>reprox</hostname><compressor>unknown</compressor><distmatrix creationtime=\"1178458118\" title=\"untitled\"><axis1><name>maketree</name><name>treemolder</name><name>improvetree</name><name>treescore</name><name>treeadaptor</name><name>treemaster</name><name>splittree</name><name>treeblaster</name><name>reordertree</name><name>treeparser</name><name>treeholder</name></axis1><axis2><name>maketree</name><name>treemolder</name><name>improvetree</name><name>treescore</name><name>treeadaptor</name><name>treemaster</name><name>splittree</name><name>treeblaster</name><name>reordertree</name><name>treeparser</name><name>treeholder</name></axis2><entries><number>0.000000</number><number>0.712270</number><number>0.707949</number><number>0.731855</number><number>0.739055</number><number>0.682316</number><number>0.699885</number><number>0.712558</number><number>0.713422</number><number>0.762385</number><number>0.721198</number><number>0.707949</number><number>0.000000</number><number>0.557563</number><number>0.501807</number><number>0.505937</number><number>0.695601</number><number>0.535581</number><number>0.471951</number><number>0.551884</number><number>0.567372</number><number>0.483738</number><number>0.708813</number><number>0.553433</number><number>0.000000</number><number>0.516490</number><number>0.466032</number><number>0.734018</number><number>0.540730</number><number>0.546577</number><number>0.151155</number><number>0.426872</number><number>0.498024</number><number>0.729839</number><number>0.495612</number><number>0.518167</number><number>0.000000</number><number>0.487982</number><number>0.708211</number><number>0.557584</number><number>0.507977</number><number>0.523756</number><number>0.538849</number><number>0.458357</number><number>0.737615</number><number>0.515746</number><number>0.469206</number><number>0.503633</number><number>0.000000</number><number>0.721114</number><number>0.558989</number><number>0.538857</number><number>0.463492</number><number>0.476825</number><number>0.480519</number><number>0.686636</number><number>0.693548</number><number>0.735191</number><number>0.712317</number><number>0.717595</number><number>0.000000</number><number>0.717302</number><number>0.617302</number><number>0.742229</number><number>0.754839</number><number>0.691202</number><number>0.693260</number><number>0.537453</number><number>0.551966</number><number>0.563202</number><number>0.551498</number><number>0.714956</number><number>0.000000</number><number>0.560393</number><number>0.551966</number><number>0.605337</number><number>0.561330</number><number>0.712558</number><number>0.464745</number><number>0.552753</number><number>0.512095</number><number>0.533711</number><number>0.616422</number><number>0.560393</number><number>0.000000</number><number>0.545548</number><number>0.568708</number><number>0.489964</number><number>0.711406</number><number>0.550852</number><number>0.149055</number><number>0.523197</number><number>0.467937</number><number>0.741642</number><number>0.538858</number><number>0.541431</number><number>0.000000</number><number>0.391078</number><number>0.512705</number><number>0.761809</number><number>0.571502</number><number>0.428272</number><number>0.542202</number><number>0.474921</number><number>0.756891</number><number>0.610955</number><number>0.571796</number><number>0.391078</number><number>0.000000</number><number>0.534161</number><number>0.721198</number><number>0.490965</number><number>0.503106</number><number>0.466182</number><number>0.479390</number><number>0.700000</number><number>0.566011</number><number>0.496140</number><number>0.517222</number><number>0.531903</number><number>0.000000</number></entries></distmatrix><commands/></clb>\n";
START_TEST (test_complearn_cloutput)
{
  struct CLDistMatrix *bdm;
  CompLearnRealCompressor *rc = complearn_environment_load_compressor_named("bzlib");
  fail_unless(rc != NULL);
  GString *cdm = real_compressor_compress(rc, g_string_new(dmstr));
  fail_unless(cdm != NULL);
  bdm = complearn_read_clb_dist_matrix(cdm);
  fail_unless(bdm != NULL);
  fail_unless(bdm->labels1a != NULL);
  fail_unless(bdm->labels2a != NULL);
  fail_unless(bdm->labels1a->len >= 0);
  fail_unless(bdm->labels2a->len >= 0);
  fail_unless(bdm->labels1a->len == 11);
  fail_unless(bdm->labels2a->len == 11);
  fail_unless(bdm->m->mat->size1 == 11);
  fail_unless(bdm->m->mat->size2 == 11);
  GString *txtmat = complearn_matrix_prettyprint_text(bdm->m);
  GString *nexmat = complearn_matrix_prettyprint_nex(bdm->m, NULL);
  fail_unless(nexmat != NULL);
  fail_unless(nexmat->str != NULL);
  fail_unless(nexmat->len > 100);
  fail_unless(txtmat != NULL);
  fail_unless(txtmat->str != NULL);
  fail_unless(txtmat->len > 100);
  fail_unless(complearn_is_nexus_file(nexmat));
  fail_unless(complearn_is_text_matrix(txtmat));
  fail_unless(strstr(nexmat->str, "treeholder") != NULL);
  GArray *a = complearn_get_nexus_labels(nexmat);
  fail_unless(bdm->m->mat->size1 == a->len);
  int i;
  LabeledMatrix *lm = complearn_load_nexus_matrix(nexmat);
  fail_unless(bdm->m->mat->size1 == complearn_count_strings(
              (const char * const *)lm->labels1));
  for (i = 0; i < bdm->m->mat->size1; i += 1) {
    char *w1 = bdm->m->labels1[i];
    char *w2 = lm->labels1[i];
    fail_unless(strcmp(w1, w2) == 0);
  }
  gsl_matrix *newm = lm->mat;
  fail_unless(newm->size1 == bdm->m->mat->size1);
  fail_unless(newm->size2 == bdm->m->mat->size2);
  int j;
  for (i = 0; i < bdm->m->mat->size1; i += 1) {
    for (j = 0; j < bdm->m->mat->size2; j += 1) {
      double d;
      d = fabs(gsl_matrix_get(bdm->m->mat, i, j) - gsl_matrix_get(newm,i,j));
      fail_unless (d <= 1e-5);
    }
  }
  LabeledMatrix *lm2 = complearn_load_any_matrix(nexmat);
  LabeledMatrix *lm3 = complearn_load_any_matrix(cdm);
  LabeledMatrix *lm4 = complearn_load_any_matrix(txtmat);
  fail_unless(lm2 != NULL);
  fail_unless(lm3 != NULL);
  fail_unless(lm4 != NULL);
  fail_unless(lm2->labels1 != NULL);
  fail_unless(lm3->labels1 != NULL);
  fail_unless(lm4->labels1 != NULL);
  fail_unless(lm2->mat->size1 == newm->size1);
  fail_unless(lm3->mat->size1 == newm->size1);
  fail_unless(lm4->mat->size1 == newm->size1);
  fail_unless(bdm->m->mat->size1 == complearn_count_strings(
              (const char * const *)lm2->labels1));
  fail_unless(bdm->m->mat->size1 == complearn_count_strings(
              (const char * const *)lm3->labels1));
  fail_unless(bdm->m->mat->size2 == complearn_count_strings(
              (const char * const *)lm3->labels2));
  fail_unless(bdm->m->mat->size1 == complearn_count_strings(
              (const char * const *)lm4->labels1));
  fail_unless(bdm->m->mat->size2 == complearn_count_strings(
              (const char * const *)lm4->labels2));
}
END_TEST

START_TEST (test_complearn_rcmem)
{
  struct CLDistMatrix *bdm;
  CompLearnRealCompressor *rc = complearn_environment_load_compressor_named("bzlib");
  fail_unless(rc != NULL);
  GString *cdm = real_compressor_compress(rc, g_string_new(dmstr));
  int i;
  for (i = 0; i < 100000; i += 1) {
    CompLearnRealCompressor *c = real_compressor_clone(rc);
    g_object_unref(c);
  }
//  fail_unless(bdm->m->mat->size2 == complearn_count_strings(
//              (const char * const *)lm4->labels2));
}
END_TEST

Suite *
cloutput_suite (void)
{
  Suite *s = suite_create ("cloutput");

  TCase *tc_core = tcase_create ("Core");
  tcase_set_timeout(tc_core, 20);
  tcase_add_test (tc_core, test_complearn_cloutput);
  tcase_add_test (tc_core, test_complearn_rcmem);
  suite_add_tcase(s, tc_core);

  return s;
}
