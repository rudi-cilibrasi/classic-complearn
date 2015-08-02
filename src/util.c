/*
Copyright (c) 2003-2008 Rudi Cilibrasi, Rulers of the RHouse
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the University nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE RULERS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/
#include <stdio.h>
#include <string.h>
#if OPENMP_ENABLED
#include <omp.h>
#endif
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
//#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <libintl.h>
#include <unistd.h>

#include "complearn/util.h"

#define _(O) gettext(O)

void complearn_mrbreaker(void)
{
  printf("break!\n");
}

int complearn_make_directory_if_necessary(const char *name)
{
  return g_mkdir_with_parents(name, 0700);
}

char *complearn_get_hostname()
{
  static char *answer;
  if (answer == NULL) {
    static char buf[1024];
#ifdef __MINGW32__
	strcpy(buf, "localhost");
#else
    gethostname(buf, sizeof(buf));
#endif
    buf[sizeof(buf)-1] = '\0';
    answer = buf;
  }
  return g_strdup(answer);
}

int complearn_get_pid(void)
{
  return getpid();
}

GSList *complearn_read_directory_of_files(const char *dirname)
{
  GError *err = NULL;
  GSList *result = NULL;
  GDir *d = g_dir_open(dirname, 0, &err);
  if (err)
    g_error(_("Cannot open directory %s"), dirname);
  const char *n;
  while ( (n = g_dir_read_name(d)) ) {
    char *cur = g_build_filename(dirname, n, NULL);
    if (g_file_test(cur, G_FILE_TEST_IS_REGULAR)) {
      result = g_slist_append(result, complearn_new_arg(complearn_read_whole_file(cur),cur));
    }
    g_free(cur);
  }
  g_dir_close(d);
  return result;
}

static GSList *complearn_read_list_of_files_or_strings(const char *filename, int isfiles)
{
  GIOChannel *in;
  GError *err = NULL;
  GSList *result = NULL;
  in = g_io_channel_new_file(filename, "r", &err);
  if (err)
    g_error(_("Cannot open file %s"), filename);
  for (;;) {
    GIOStatus s;
    gsize tpos;
    err = NULL;
    GString *block = g_string_new("");
    s = g_io_channel_read_line_string(in, block, &tpos, &err);
    if (s == G_IO_STATUS_EOF)
      break;
    if (err != NULL)
      g_error(_("Cannot read line from file %s: %s"), filename, err->message);
    g_string_truncate(block, tpos);
    GString *tokeep;
    tokeep = (isfiles != 0) ? complearn_read_whole_file(block->str) : g_string_new_len(block->str, block->len);
    result = g_slist_append(result, complearn_new_arg(tokeep, block->str));
  }
  g_io_channel_close(in);
  return result;
}

GSList *complearn_read_list_of_files(const char *filename)
{
  return complearn_read_list_of_files_or_strings(filename, 1);
}

GSList *complearn_read_list_of_strings(const char *filename)
{
  return complearn_read_list_of_files_or_strings(filename, 0);
}

GString *complearn_read_whole_file_ptr(FILE *fp)
{
  GString *result = g_string_new("");
  char buf[512];
  int rl;
  while ( (rl = fread(buf, 1, 512, fp)) == 512)
    g_string_append_len(result, buf, rl);
  if (rl > 0)
    g_string_append_len(result, buf, rl);
  return result;
}

char *complearn_chdir(const char *newdir)
{
  char *retval = g_get_current_dir();
  int rv;
  rv = g_chdir(newdir);
  if (rv != 0) {
    g_error(_("Cannot change directory to %s"), newdir);
  }
  return retval;
}

int complearn_remove_directory_recursively(const char *dirname, int f_err_out)
{
  GError *err = NULL;
  int result = 0;
  GDir *d = g_dir_open(dirname, 0, &err);
  if ( (err) ) {
    if (f_err_out)
      g_error(_("Cannot open directory %s. Error: %s"), dirname, err->message);
    else
      return 1;
  }
  const char *n;
  while ( (n = g_dir_read_name(d)) ) {
    char *cur = g_build_filename(dirname, n, NULL);
    if (g_file_test(cur, G_FILE_TEST_IS_REGULAR))
      g_unlink(cur);
    g_free(cur);
  }
  g_dir_close(d);
  result = g_rmdir(dirname);
  return result;
}

int complearn_write_file(const char *fname, const GString *f)
{
  g_assert(fname != NULL && strlen(fname) > 0);
  FILE *fp = fopen(fname, "wb");
  if (fp == NULL) {
    g_error(_("Cannot open file %s for writing."), fname);
  }
  fwrite(f->str, 1, f->len, fp);
  fclose(fp);
  return 0;
}

GString *complearn_read_whole_file(const char *fname)
{
  FILE *fp = fopen(fname, "rb");
  if (fp == NULL)
    g_error(_("Cannot open file %s for reading"), fname);
  fseek(fp, 0, SEEK_END);
  int sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *b = calloc(sz, 1);
  GString *result;
  fread(b, 1, sz, fp);
  fclose(fp);
  result = g_string_new_len(b, sz);
  g_free(b);
  return result;
}

char *complearn_make_temp_dir(void)
{
  char *result = complearn_make_temp_dir_name();
#ifdef __MINGW32__
  mkdir(result);
#else
  mkdir(result, 0700);
#endif
  return result;
}

/* different each time */
char *complearn_make_temp_file_name(void)
{
  char *tnam, *result;
  static int i = 0;
  char pidbuf[32];
  sprintf(pidbuf, "%d-%d", ++i, getpid());
  tnam = getenv("TMPDIR");
  if (tnam == NULL)
    tnam = "/tmp";
  result = g_strconcat(tnam, "/clfile", pidbuf, NULL);
  return result;
}

char *complearn_make_temp_dir_name(void)
{
  int tid = 0;
#if OPENMP_ENABLED
    if (!g_thread_supported ()) g_thread_init (NULL);
    tid = omp_get_thread_num();
#endif

  char *result = NULL, *tnam;
  int incr = 0;
  char pidbuf[64];
  for (;;) {
    sprintf(pidbuf, "%dx%dx%d", getpid(),incr,tid);
    tnam = getenv("TMPDIR");
    if (tnam == NULL)
      tnam = "/tmp";
    result = tnam;
    result = g_strconcat(tnam, "/complearn-", pidbuf, NULL);
    if (!g_file_test(result, G_FILE_TEST_EXISTS) && !g_file_test(result, G_FILE_TEST_IS_DIR))
      break;
    incr += 1;
  }
  return result;
}

char ** complearn_dupe_strings(const char * const * str)
{
  int size = complearn_count_strings(str);
  char **result;
  int i;
  result = calloc(size+1, sizeof(gpointer));
  for (i = 0; i < size; i += 1)
    result[i] = g_strdup(str[i]);
  return result;
}

int complearn_count_strings(const char * const * str)
{
  int acc = 0;
  const char * const *cur = str;
  while (cur && *cur) {
    acc += 1;
    cur += 1;
  }
  return acc;
}

CLArgumentEntry *complearn_new_arg(GString *block, const char *label) {
  CLArgumentEntry *cl = calloc(1, sizeof(*cl));
  cl->block = block;
  cl->label = g_strdup(label);
  return cl;
}

/* This function makes the off-diagonal elements symmetric by averaging.  */
gsl_matrix *complearn_average_matrix(gsl_matrix *a)
{
  int i,j;
  gsl_matrix *res;
  res = gsl_matrix_alloc(a->size1, a->size2);

  for (i = 0;i<a->size1;i+=1)
    for (j = 0; j <a->size2; j+=1) {
      double v;
      if (i == j)
        gsl_matrix_set(res,i,j,gsl_matrix_get(a,i,j));
      if (i <= j)
        continue;
      v = (gsl_matrix_get(a,i,j) + gsl_matrix_get(a,j,i))/2.0;
      gsl_matrix_set(res,i,j,v);
      gsl_matrix_set(res,j,i,v);
    }
  return res;
}

gsl_matrix *complearn_svd_project(gsl_matrix *a)
{
  int retval;
  gsl_matrix *res;
  gsl_matrix *u, *v;
  gsl_vector *s;
  u = gsl_matrix_alloc(a->size1, a->size2);
  gsl_matrix_memcpy(u, a);
  v = gsl_matrix_alloc(a->size2, a->size2);
  s = gsl_vector_alloc(a->size1);
  retval = gsl_linalg_SV_decomp_jacobi(u, v, s);
//  g_assert(retval == GSL_OK);
  res = gsl_matrix_alloc(a->size1, a->size2);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, a, u, 0.0, res);
  return res;
}

static gboolean is_last_common_char(char **inp, int which)
{
  int i;
  if (strlen(inp[0]) <= which)
    return FALSE;
  for (i = 1; inp[i]; i += 1)
    if (strlen(inp[i]) <= which || inp[i][strlen(inp[i])-which-1] != inp[0][strlen(inp[0])-which-1])
      return FALSE;
  return TRUE;
}

static gboolean is_common_char(char **inp, int which)
{
  int i;
  if (strlen(inp[0]) <= which)
    return FALSE;
  for (i = 1; inp[i]; i += 1)
    if (strlen(inp[i]) <= which || inp[i][which] != inp[0][which])
      return FALSE;
  return TRUE;
}

static char **chop_strings(char **inp, int prefixc, int suffixc)
{
  int sc = complearn_count_strings((const char * const *) inp);
  char **result = calloc(sc+1,sizeof(gpointer));
  int i;
  for (i = 0; i < sc; i += 1) {
    char *p = inp[i]+prefixc;
    char *res;
    res = g_strdup(p);
    if (suffixc && strlen(p) > suffixc)
      res[strlen(res)-suffixc] = '\0';
    result[i] = res;
  }
  return result;
}

char ** complearn_fix_labels(char **inp)
{
  int prefixc = 0, suffixc = 0;
  if (inp[1] != NULL) {
    while (is_common_char(inp, prefixc))
      prefixc += 1;
    while ( is_last_common_char(inp, suffixc))
      suffixc += 1;
  }
  return chop_strings(inp, prefixc, suffixc);
}
