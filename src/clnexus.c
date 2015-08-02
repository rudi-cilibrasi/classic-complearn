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
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include "complearn/complearn.h"

GArray *clGetNexusLabels(GString *db);

static const char *nexHeaderString = ""
"#nexus\n"
"\n"
"BEGIN Taxa;\n"
"DIMENSIONS ntax=%d;\n"
"TAXLABELS\n";

static const char *nexMidString = ""
";\n"
"END; [Taxa]\n"
"\n";

static const char *nexPreDistString = ""
"BEGIN Distances;\n"
"DIMENSIONS ntax=%d;\n"
"FORMAT labels=left diagonal triangle=both;\n"
"MATRIX\n";

static const char *makePreDistString(gsl_matrix *gm)
{
  static char result[1024];
  sprintf(result, nexPreDistString, gm->size1);
  return result;
}

static const char *makeHeaderString(int s)
{
  static char result[1024];
  sprintf(result, nexHeaderString, s);
  return result;
}

GString *complearn_matrix_prettyprint_nex(LabeledMatrix *inp, GString *treeblock)
{
  int s = complearn_count_strings((const char * const *) inp->labels1);
  if (s < 1)
    g_error("Must have more than 0 labels.");
  GString *res = g_string_new(makeHeaderString(s));
  GString *cur = g_string_new("");
  int i, j;
  for (i = 0; i < s; i += 1) {
    g_string_sprintf(cur,"[%d] '%s'\n",i+1,inp->labels1[i]);
    g_string_append(res, cur->str);
  }
  g_string_sprintf(cur, "%s", nexMidString); g_string_append(res, cur->str);
  if (inp->mat) {
    g_string_sprintf(cur, "%s", makePreDistString(inp->mat));
    g_string_append(res, cur->str);
    for (i = 0; i < s; i += 1) {
      g_string_sprintf(cur,"[%d] '%s'",i+1,inp->labels1[i]);
      g_string_append(res, cur->str);
      for (j = 0; j < s; j += 1) {
        g_string_sprintf(cur, " %f", gsl_matrix_get(inp->mat,j,i));
        g_string_append(res, cur->str);
      }
      g_string_append(res, "\n");
    }
    g_string_append(res, ";\nEND; [Distances]\n");
  }
  if (treeblock) {
    g_string_append(res, treeblock->str);
  }
#if 0
  if (ta) {
    char *res = nodeToNewick(ta, clTreeaLabelPerm(ta), labels, 0, -1);
    curSpot += sprintf(curSpot, "\nBEGIN Trees;\n[1] tree 'CLQCS'= %s;\nEND; [Trees]\n", res);
    clFree(res);
  }
  res = clStringToDataBlockPtr(dbm);
  clFree(dbm);
#endif
  return res;

}

gboolean complearn_is_nexus_file(const GString *db)
{
  const char *targetStr = "#nexus";
  char s[7];
  int i;
  if (db->len < sizeof(s))
    return 0;
  memcpy(s, db->str, sizeof(s));
  for (i = 0; i < sizeof(s); i += 1)
    s[i] = tolower(s[i]);
  if (!isspace(s[strlen(targetStr)]))
    return 0;
  s[sizeof(s)-1] = '\0';
  return strcmp(targetStr, s) == 0;
}

static int find_block_length(GString *db)
{
  int state = 0;
  int s;
  unsigned char *t = (unsigned char *) db->str;
//  assert(complearn_is_nexus_file(db));
  for (s = 0; s < db->len; s += 1) {
    char c = t[s];
    if (isspace(c)) {
      state = 1;
      continue;
    }
    switch (tolower(c)) {
      case 'e': if (state == 1) state = 2; break;
      case 'n': if (state == 2) state = 3; break;
      case 'd': if (state == 3) state = 4; break;
      case ';': if (state == 4) state = 5; break;
      default:
        state = 0;
    }
    if (state == 5)
      break;
  }
  if (state == 5) {
    unsigned char *t2 = (unsigned char *) index((char *) (t+s), '\n');
    if (t2 == NULL)
      return -1;
    return (t2-t)+1;
  }
  return -1;
}

char *complearn_find_substring_case_insensitive(const GString *db, const char *tstr);
GString *complearn_grab_nexus_block(const GString *big, const char *blockName)
{
  int blen = strlen(blockName);
  int bend;
  char *fs1, *s1 = calloc(blen+20,1);
  GString *db2 = NULL;
  sprintf(s1, "begin %s;", blockName);
  fs1 = complearn_find_substring_case_insensitive(big, s1);
  if (fs1 == NULL)
    goto done;
  db2 = g_string_new(fs1);
  bend = find_block_length(db2);
  g_string_truncate(db2, bend);
    done:
  return db2;
}

char *complearn_find_substring_case_insensitive(const GString *db, const char *tstr)
{
  char *targetStr = strdup(tstr);
  char *testStr = g_strdup(db->str);
  char *sub;
  char *result = NULL;
  int i;
  for (i = 0; targetStr[i]; i += 1)
    targetStr[i] = tolower(targetStr[i]);
  for (i = 0; testStr[i]; i += 1)
    testStr[i] = tolower(testStr[i]);
  sub = strstr(testStr, targetStr);
  if (sub != NULL)
    result = (char *) (db->str + (sub - testStr));
  g_free(targetStr);
  g_free(testStr);
  return result;
}

GString *complearn_get_nexus_taxa_block(const GString *db)
{
  return complearn_grab_nexus_block(db, "Taxa");
}

GString *complearn_get_nexus_distances_block(const GString *db)
{
  return complearn_grab_nexus_block(db, "Distances");
}

gsl_matrix *complearn_get_nexus_distance_matrix(const GString *db)
{
  char *labelbuf = calloc(db->len+1, 1);
  int lptr = 0;
  GString *tdb = complearn_get_nexus_distances_block(db);
  GArray *ss = complearn_get_nexus_labels(db);
  char *d = (char *) tdb->str;
  int i, s = tdb->len;
  int dc = 0;
  int ms = ss->len;
  gsl_matrix *g = gsl_matrix_calloc(ms, ms);
  for (i = 0; i < s; i += 1) {
    char c = d[i];
    if (isspace(c)) {
      if (lptr > 0) {
        labelbuf[lptr++] = '\0';
        if (strspn(labelbuf, "0123456789.-+eE") == lptr-1) {
          if (dc >= ms*ms)
            g_warning("Discarding extra value %s in matrix.", labelbuf);
          else {
            gsl_matrix_set(g, dc%ms, dc/ms, atof(labelbuf));
            dc += 1;
          }
        }
      }
      lptr = 0;
      continue;
    }
    labelbuf[lptr++] = c;
  }
  free(labelbuf);
  g_string_free(tdb, TRUE);
  g_array_free(ss, TRUE); // TODO more?
  if (dc != ms*ms)
    g_warning("Not enough values in matrix, filling with 0.");
  return g;
}

GString *complearn_get_nexus_tree_block(const GString *db)
{
  return complearn_grab_nexus_block(db, "Trees");
}

GArray *complearn_get_nexus_labels(const GString *db)
{
  char *labelbuf = calloc(db->len+1, 1);
  int lptr = 0;
  GString *tdb = complearn_get_nexus_taxa_block(db);
  g_assert(tdb != NULL);
  GArray *ss = g_array_new(FALSE, TRUE, sizeof(gpointer));
  char *d = (char *) tdb->str;
  int i, s = tdb->len;
  for (i = 0; i < s; i += 1) {
    char c = d[i];
    if (isspace(c)) {
      lptr = 0;
      continue;
    }
    if (c == '\'') {
      if (lptr > 0) {
        labelbuf[lptr++] = '\0';
        char *toadd = strdup(labelbuf);
        g_array_append_val(ss, toadd);
        lptr = 0;
      }
      continue;
    }
    labelbuf[lptr++] = c;
  }
  free(labelbuf);
  g_string_free(tdb, TRUE);
  return ss;
}

LabeledMatrix *complearn_load_nexus_matrix(const GString *inp) {
  GArray *lab = complearn_get_nexus_labels(inp);
  LabeledMatrix *result = calloc(sizeof(*result), 1);
  result->mat = complearn_get_nexus_distance_matrix(inp);
  int i;
  result->labels1 = calloc(sizeof(gpointer), lab->len+1);
  result->labels2 = calloc(sizeof(gpointer), lab->len+1);
  for (i = 0;i < lab->len; i += 1) {
    result->labels1[i] = g_strdup(g_array_index(lab, char *, i));
    result->labels2[i] = g_strdup(g_array_index(lab, char *, i));
    g_free(g_array_index(lab, char *, i));
  }
  g_array_free(lab, TRUE);
  return result;
}

int complearn_text_matrix_row_size(const GString *db)
{
  int i;
  int sz = db->len;
  unsigned char *d = (guchar *) db->str;
  int goodrow = 0;
  int c=0, lc;
  int rowCount = 0;
  for (i = 0; i < sz; i += 1) {
    lc = c;
    c = d[i];
    if (!(c == '\r' || c == '\n' || c == '\t' || c == ' '))
      goodrow = 1;
    if ((c == '\r' || c == '\n') && (lc != '\r' && lc != '\n')) {
      rowCount += goodrow;
      goodrow = 0;
    }
  }
  return rowCount;
}

static GArray *cl_default_labels(int sz)
{
  int i;
  GArray *ss = g_array_new(FALSE, TRUE, sizeof(gpointer));
  for (i = 0; i < sz; i += 1) {
    char buf[80], *b2;
    sprintf(buf, "item%02d", i+1);
    b2 = strdup(buf);
    g_array_append_val(ss,b2);
  }
  return ss;
}

static int find_longest_string(GArray *ga)
{
  int maxlen = 0;
  int i, c;
  for (i = 0; i < ga->len; i += 1)
    if ((c=strlen(g_array_index(ga, char *, i)) ) > maxlen)
      maxlen = c;
  return maxlen;
}

static int grabFields(gsl_matrix *m, const char *rowStart, int row, GArray *labels)
{
  char numbuf[2048];
  char *nclone;
  int numtop = 100;
  int numpos;
  const char *rowEnd = index(rowStart, '\n');
  if (rowEnd == NULL)
    rowEnd = rowStart + strlen(rowStart);
  numpos = numtop;
  numbuf[numtop+1] = 0;
  int colind = m->size1-1;
  while (colind >= 0 && rowEnd > rowStart) {
    int c;
    c = *rowEnd--;
    if ((c >= '0' && c <= '9') || c == '.' || c == ',') {
      if (c == ',')
        c = '.';
      numbuf[numpos--] = c;
    }
    else {
      if (numpos < numtop) {
        double v = atof(numbuf+numpos+1);
        gsl_matrix_set(m, colind--, row, v);
        numpos = numtop;
      }
    }
  }
  memset(numbuf, 0, sizeof(numbuf));
  if (labels) {
    int spanlen;
    spanlen = strspn(rowStart, "abcdefghijklmnopqrstuvwxyz"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "0123456789_.");
    strncpy(numbuf, rowStart, spanlen);
    nclone = strdup(numbuf);
    g_array_append_val(labels, nclone);
  }
  return 0;
}

gsl_matrix *complearn_text_dist_matrix(const GString *db, GArray *ulabels)
{
  int i;
  int sz = db->len;
  char *d = (char *) db->str;
  int goodrow = 0;
  int c=0, lc;
  int curRow = 0;
  int rows = 0;
  char *lastRow;
  GArray *labels;
  gsl_matrix *result;
  labels = g_array_new(FALSE, TRUE, sizeof(gpointer));

  rows = complearn_text_matrix_row_size(db);
  if (rows < 4)
    g_error( "Error, only %d rows in matrix, but need at least 4.\n", rows);
  result = gsl_matrix_alloc(rows,rows);
  lastRow = d;
  for (i = 0; i < sz; i += 1) {
    lc = c;
    c = d[i];
    if (!(c == '\r' || c == '\n' || c == '\t' || c == ' ')) {
      goodrow = 1;
      if (lc == '\r' || lc == '\n')
        lastRow = d + i;
    }
    if ((c == '\r' || c == '\n') && (lc != '\r' && lc != '\n')) {
      if (goodrow)
        grabFields(result, lastRow, curRow, labels);
      curRow += goodrow;
      goodrow = 0;
    }
  }
  if (find_longest_string(labels) == 0) {
    g_array_free(labels, TRUE);
    labels = cl_default_labels(sz);
  }
  for (i = 0; i < labels->len; i += 1) {
    char *v = g_array_index(labels, char *, i);
    g_array_append_val(ulabels, v);
  }
  g_array_free(labels, TRUE);
  return result;
}

gboolean complearn_is_text_matrix(const GString *db)
{
  char **lines = g_strsplit(db->str, "\n\r", 0);
  char *firstline = NULL, **l2 = NULL;
  int i;
  int isit = 0;
  for (i = 0; lines[i] != NULL; i += 1)
    if (strlen(lines[i]) > 1)
      break;
  if (lines[i] == NULL) {
    isit = 0;
    goto done;
  }
  firstline = lines[i];
  if (firstline[0] == '\0') {
    isit = 0;
    goto done;
  }
  l2 = g_strsplit(firstline, " \t", 0);
  for (i = 1; l2[i]; i += 1) {
    if (!isdigit(l2[i][0]) && l2[i][0]) {
      isit = 0;
      goto done;
    }
  }
  isit = 1;
  done:
    g_strfreev(lines);
    g_strfreev(l2);
    return isit;
}

LabeledMatrix *complearn_load_text_matrix(const GString *inp)
{
  LabeledMatrix *result = calloc(sizeof(*result), 1);
  GArray *lab = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->mat = complearn_text_dist_matrix(inp, lab);
  int i;
  result->labels1 = calloc(sizeof(gpointer), lab->len+1);
  result->labels2 = calloc(sizeof(gpointer), lab->len+1);
  for (i = 0;i < lab->len; i += 1) {
    result->labels1[i] = g_strdup(g_array_index(lab, char *, i));
    result->labels2[i] = g_strdup(g_array_index(lab, char *, i));
    g_free(g_array_index(lab, char *, i));
  }
  g_array_free(lab, TRUE);
  return result;
}

