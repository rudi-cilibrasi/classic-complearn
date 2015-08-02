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
#include <glib.h>
#include <libintl.h>
#include <locale.h>
#include "complearn/newcomplearn.h"
#include "complearn/clconfig.h"

#define _(O) gettext(O)

#define clStringstackPush(a,v) {void*_u=(void*)v;g_array_append(a,_u);}while(0)

GString *complearn_matrix_prettyprint_text(LabeledMatrix *result) {
  int i, j;
  int html = complearn_ncd_get_html_output(complearn_ncd_top());
  char *rowstart = "", *rowend = "\n", *numstart = "", *numend = "", *labstart = "", *labend = " ", *lastcellstart="", *lastcellend ="";
  GString *toWrite = g_string_new("");
  CompLearnNcd *top = complearn_ncd_top();
  gsl_matrix *goodmat =gsl_matrix_alloc(result->mat->size1, result->mat->size2);
  gsl_matrix_memcpy(goodmat, result->mat);
  if (html) {
    g_string_append(toWrite, "<table cellpadding='0' cellspacing='0'>\n");
      rowstart = "<tr>\n"; rowend = "</tr>\n";
      lastcellstart = "<td style='font-family: sans-serif; padding: 7px; border: 0px gray solid; border-bottom-width: 1px; border-right-style: dashed; border-right-width:1px; border-left-style: dashed; border-left-width:1px;'>"; lastcellend = "</td>";
      numstart = "<td style='font-family: sans-serif; padding: 7px; border: 0px gray solid; border-bottom-width: 1px; border-left-style: dashed; border-left-width:1px;'>"; numend = "</td>";
      labstart = "<td style='color: green; font-weight: bold; padding: 7px; border: 0px gray solid; border-bottom-width: 1px;'>"; labend = "</td>";
    }
    for (j = 0; j < goodmat->size2; j += 1) {
      g_string_append(toWrite, rowstart);
      if (complearn_ncd_get_show_labels(top)) {
        g_string_append(toWrite, labstart);
        if (result->labels2[j] == NULL)
          g_error("bad labels2 %d with mat size2 %d", j, goodmat->size2);
        g_string_append(toWrite, result->labels2[j]);
        g_string_append(toWrite, labend);
      }
      for (i = 0; i < goodmat->size1; i += 1) {
        char buf[256];
        sprintf(buf, "%f ", gsl_matrix_get(goodmat, i, j));
        g_string_append(toWrite, i == goodmat->size1-1 ? lastcellstart : numstart);
        g_string_append(toWrite, buf);
        g_string_append(toWrite, i == goodmat->size1-1 ? lastcellend : numend);
      }
      g_string_append(toWrite, rowend);
    }
  if (html)
    g_string_append(toWrite, "</table>\n");
  gsl_matrix_free(goodmat);
  return toWrite;
}

#include <complearn/complearn.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/encoding.h>

#define MY_ENCODING "ISO-8859-1"

#include <gsl/gsl_linalg.h>

#define PWD_RDY 1
#define UTS_RDY 1

#ifndef __MINGW32__

#if PWD_RDY
#include <pwd.h>
#endif

#if UTS_RDY
#include <sys/utsname.h>
#endif

#endif


GArray *clDefaultLabels(int i);

static void complearn_handle_string_list(xmlDocPtr doc, xmlNodePtr node, GArray *ss,
                             const char *tagname) {
  node = node->xmlChildrenNode;
  while (node != NULL) {
    if (strcmp(tagname, (char *) node->name) == 0) {
      guchar *v = xmlNodeListGetString(doc, node->xmlChildrenNode,1);
      g_array_append_val(ss, v);
    }
    node = node->next;
  }
}

static void handleDM(xmlDocPtr doc, xmlNodePtr node, GArray *ss,
struct CLDistMatrix *result) {
  result->title = (char *) xmlGetProp(node, (unsigned char *) "title");
  result->creationTime = (char *) xmlGetProp(node, (unsigned char *) "creationtime");
  node = node->xmlChildrenNode;
  GArray *numa, *lab1a, *lab2a;
  numa = result->numa;
  lab1a = result->labels1a;
  lab2a = result->labels2a;
  while (node != NULL) {
    do {
        if (strcmp("entries", (char *) node->name) == 0) {
          complearn_handle_string_list(doc, node, numa, "number");
          continue;
        }
        if (strcmp((char *) node->name, "axis1") == 0) {
          complearn_handle_string_list(doc, node, lab1a, "name");
          continue;
        }
        if (strcmp((char *) node->name, "axis2") == 0) {
          complearn_handle_string_list(doc, node, lab2a, "name");
          continue;
        }
      } while(0);
    node = node->next;
  }
}

static void complearn_write_string_list(xmlTextWriterPtr tw, char **ss, const char *topname, const char *itemname) {
  int rc;
  int i;
  rc = xmlTextWriterStartElement(tw, (unsigned char *) topname);
  for (i = 0; i < complearn_count_strings((const char * const * )ss); i += 1)
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) itemname, (unsigned char *) ss[i]);
  rc = xmlTextWriterEndElement(tw);
}

static char *getTimestrNow(void)
{
  static char buf[32];
  sprintf(buf, "%lu", (unsigned long) time(NULL));
  return buf;
}

static struct CLDistMatrix *fill_in_blanks(struct CLDistMatrix *clb)
{
  if (clb->m == NULL) {
    g_error(_("Cannot write NULL gsl_matrix, exitting."));
  }
  if (clb->m->mat->size1 < 1 || clb->m->mat->size2 < 1) {
    g_error(_("Invalid gsl_matrix size, cannot write.  Exitting."));
  }
  if (clb->fileverstr == NULL)
    clb->fileverstr = g_strdup("1.0");
  if (clb->cllibver == NULL)
    clb->cllibver = g_strdup(complearn_package_version);
  if (clb->username == NULL)
    clb->username = g_strdup(g_get_user_name());
  if (clb->hostname == NULL)
    clb->hostname = g_strdup(complearn_get_hostname());
  if (clb->title == NULL)
    clb->title = g_strdup(_("untitled"));
  if (clb->compressor == NULL)
    clb->compressor = g_strdup(_("unknown"));
  if (clb->creationTime == NULL)
    clb->creationTime = g_strdup(getTimestrNow());
  return clb;
}

struct CLDistMatrix *complearn_clone_cldm(struct CLDistMatrix *clb)
{
  struct CLDistMatrix *r = calloc(sizeof(*r), 1);
  if (clb->m == NULL) {
    g_error(_("Cannot write NULL gsl_matrix, exitting."));
  }
#define DUPSTR(s) if (clb->s) r->s = g_strdup(clb->s)
  DUPSTR(cllibver);
  DUPSTR(username);
  DUPSTR(hostname);
  DUPSTR(title);
  DUPSTR(compressor);
  DUPSTR(creationTime);
  r->m = calloc(sizeof(*(r->m)), 1);
  r->m->mat = gsl_matrix_alloc(clb->m->mat->size1, clb->m->mat->size2);
  gsl_matrix_memcpy(r->m->mat, clb->m->mat);
  r->m->labels1 = complearn_dupe_strings((const char * const *)clb->m->labels1);
  r->m->labels2 = complearn_dupe_strings((const char * const *)clb->m->labels2);
  r->cmds = complearn_dupe_strings((const char * const *)clb->cmds);
  r->cmdtimes = complearn_dupe_strings((const char * const *)clb->cmdtimes);
  return r;
}

void complearn_free_cldm(struct CLDistMatrix *clb)
{
  if (clb->cllibver) g_free(clb->cllibver);
  if (clb->username) g_free(clb->username);
  if (clb->hostname) g_free(clb->hostname);
  if (clb->title) g_free(clb->title);
  if (clb->compressor) g_free(clb->compressor);
  if (clb->creationTime) g_free(clb->creationTime);
  if (clb->m->mat) gsl_matrix_free(clb->m->mat);
  g_strfreev(clb->m->labels1);
  g_strfreev(clb->m->labels2);
  g_strfreev(clb->cmdtimes);
  g_strfreev(clb->cmds);
  g_free(clb->m);
  memset(clb, 0, sizeof(*clb));
  g_free(clb);
}

static char *clXMLQuoteStr(char *inp)
{
  int i, c, j;
  static char *outstr;
  if (outstr != NULL)
    g_free(outstr);
  outstr = calloc(strlen(inp) * 10, 1);
  j = 0;
  for (i = 0; inp[i]; i += 1) {
    c = inp[i];
    switch(c) {
      case '&':
        outstr[j++] = '&'; outstr[j++] = 'a'; outstr[j++] = 'm';
        outstr[j++] = 'p'; outstr[j++] = ';'; break;
      case '>':
        outstr[j++] = '&'; outstr[j++] = 'g'; outstr[j++] = 't';
        outstr[j++] = ';'; break;
      case '<':
        outstr[j++] = '&'; outstr[j++] = 'l'; outstr[j++] = 't';
        outstr[j++] = ';'; break;
      case '"':
        outstr[j++] = '&'; outstr[j++] = 'q'; outstr[j++] = 'o';
        outstr[j++] = 'o'; outstr[j++] = 't'; outstr[j++] = ';'; break;
      case '\'':
        outstr[j++] = '&'; outstr[j++] = 'a'; outstr[j++] = 'p';
        outstr[j++] = 'o'; outstr[j++] = 's'; outstr[j++] = ';'; break;
//      case '\\':
 //     case '#':
//        outstr[j++] = '\\'; outstr[j++] = c; break;
      default:
        outstr[j++] = c;
    }
  }
  outstr[j++] = 0;
  return outstr;
}

static GString *clRealWriteCLBDistMatrix(struct CLDistMatrix *clb)
{
  int rc;
  GString *result, *cres;
  int dim1, dim2, i, j;
  gsl_matrix *m = clb->m->mat;
  xmlBufferPtr b;
  xmlTextWriterPtr tw;
  clb = fill_in_blanks(complearn_clone_cldm(clb));
  b = xmlBufferCreate();
  tw =xmlNewTextWriterMemory(b, 0);
  rc = xmlTextWriterStartDocument(tw, NULL, MY_ENCODING, NULL);
  rc = xmlTextWriterStartElement(tw, (unsigned char *) "clb");
  rc = xmlTextWriterWriteAttribute(tw, (unsigned char *) "version", (unsigned char *) clb->fileverstr);

    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "cllibver", (unsigned char *) clb->cllibver);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "username", (unsigned char *) clb->username);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "hostname", (unsigned char *) clb->hostname);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "compressor", (unsigned char *) clb->compressor);
    rc = xmlTextWriterStartElement(tw,(unsigned char *)  "distmatrix");
    rc = xmlTextWriterWriteAttribute(tw,(unsigned char *)  "creationtime", (unsigned char *) clb->creationTime);
    rc = xmlTextWriterWriteAttribute(tw, (unsigned char *) "title",(unsigned char *)  clb->title);
    complearn_write_string_list(tw, clb->m->labels1,  "axis1",  "name");
    complearn_write_string_list(tw, clb->m->labels2,  "axis2",  "name");
    dim1 = complearn_count_strings((const char * const *) clb->m->labels1);
    dim2 = complearn_count_strings((const char * const *) clb->m->labels2);
      rc = xmlTextWriterStartElement(tw,(unsigned char *)  "entries");
      for (i = 0; i < dim1; i += 1) {
        for (j = 0; j < dim2; j += 1) {
          double g = gsl_matrix_get(m, i, j);
            xmlTextWriterWriteFormatElement(tw, (unsigned char *) "number", (char *) "%f", g);
        }
      }
      rc = xmlTextWriterEndElement(tw);
    rc = xmlTextWriterEndElement(tw);
    if (clb->cmds) {
      char *str;
      rc = xmlTextWriterStartElement(tw,(unsigned char *)  "commands");
      for (i = 0; i < complearn_count_strings((const char * const *) clb->cmds); i += 1) {
        char *t = clb->cmdtimes ? (clb->cmdtimes[i]) : NULL;
        rc = xmlTextWriterStartElement(tw,(unsigned char *)  "cmdstring");
        if (t)
          rc = xmlTextWriterWriteAttribute(tw,(unsigned char *)  "creationtime", (unsigned char *) t);
        str = clb->cmds[i];
        xmlTextWriterWriteRaw(tw, (unsigned char *)  clXMLQuoteStr(str));
        rc = xmlTextWriterEndElement(tw);
      }
      rc = xmlTextWriterEndElement(tw);
    }
  rc = xmlTextWriterEndElement(tw);
  rc = xmlTextWriterEndDocument(tw);
  xmlFreeTextWriter(tw);
  result = g_string_new((char *) b->content);
  xmlBufferFree(b);
  /* TODO: remove more mem leaks from above func */
  complearn_free_cldm(clb);
  CompLearnRealCompressor *bz = COMPLEARN_REAL_COMPRESSOR(complearn_environment_get_nameable("bzlib"));
  if (bz == NULL)
    g_error(_("Cannot load bzlib"));
  cres = real_compressor_compress(bz, result);
  g_string_free(result, TRUE);
  return cres;
}

gsl_matrix *complearn_clb_dist_matrix(const GString *db)
{
  return complearn_read_clb_dist_matrix(db)->m->mat;
}

struct CLDistMatrix *complearn_read_clb_dist_matrix(const GString *udb)
{
  struct CLDistMatrix *result = calloc(sizeof(struct CLDistMatrix), 1);
  GArray *ents = g_array_new(FALSE, TRUE, sizeof(gpointer));
  GString *db;
  int dim1, dim2;
  xmlDocPtr doc;
  xmlNodePtr node;
  CompLearnRealCompressor *bz = COMPLEARN_REAL_COMPRESSOR(complearn_environment_get_nameable("bzlib"));
  if (bz == NULL)
    g_error(_("Cannot load bzlib compressor."));
  if (real_compressor_is_decompressible(bz, udb))
    db = real_compressor_decompress(bz, udb);
  else
    db = g_string_new_len(udb->str, udb->len);
  if (db == NULL || db->len < 1)
    return NULL;
  if (db->str[0] != '<')
    return NULL;
  result->fileverstr = "";
  result->username = "";
  result->title = "";
  result->creationTime = "";
  result->cmdsa = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->cmdtimesa = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->labels1a = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->labels2a = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->numa = g_array_new(FALSE, TRUE, sizeof(gpointer));
  result->m = calloc(sizeof(*result->m), 1);
  doc = xmlReadMemory((char *)db->str, db->len, "noname.xml",
                      NULL, 0);
  if (doc == NULL) {
    g_error(_("Failed to parse document."));
    g_string_free(db, TRUE);
    free(result);
    return NULL;
  }
  node = doc->children;
  if (strcmp((char *) node->name, "clb") != 0) {
    free(result);
    g_string_free(db, TRUE);
    return NULL;
  }
  result->fileverstr = (char *) xmlGetProp(node, (unsigned char *) "version");
  node = node->xmlChildrenNode;
 while (node != NULL) {
    do {
      if (strcmp((char *) node->name, "commands") == 0) {
        complearn_handle_string_list(doc, node, result->cmdsa, "cmdstring");
        continue;
      }
      if (strcmp((char *) node->name, "distmatrix") == 0) {
        handleDM(doc, node, ents, result);
  result->m->mat = gsl_matrix_alloc(result->labels1a->len, result->labels2a->len);
  int i, x=0, y=0;
  for (i = 0; i < result->numa->len; i += 1) {
    gsl_matrix_set(result->m->mat,x,y,atof(g_array_index(result->numa, char *, i)));
    g_assert(y < result->m->mat->size2);
    x += 1;
    if (x == result->m->mat->size1) {
      x = 0; y += 1;
    }
  }
        continue;
      }
      if (strcmp((char *) node->name, "cllibver") == 0) {
        result->cllibver = (char *) g_strdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
      if (strcmp((char *) node->name, "compressor") == 0) {
        result->compressor = (char *) g_strdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
      if (strcmp((char *) node->name, "username") == 0) {
        result->username = (char *) g_strdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
    } while (0);
    node = node->next;
    }

  dim1 = result->labels1a->len;
  if (dim1 <= 0) {
    g_error(_("Error, no labels for dimension 1."));
    exit(1);
  }
  dim2 = result->labels2a->len;
  if (dim2 <= 0) {
    g_error(_("Error, no labels for dimension 2."));
    exit(1);
  }
  g_array_free(ents, TRUE);
  g_string_free(db, TRUE);
  result->m->labels1 = calloc(result->labels1a->len+1, sizeof(gpointer));
  result->m->labels2 = calloc(result->labels2a->len+1, sizeof(gpointer));
  memcpy(result->m->labels1, result->labels1a->data, sizeof(gpointer)*(result->labels1a->len));
  memcpy(result->m->labels2, result->labels2a->data, sizeof(gpointer)*(result->labels2a->len));
  return result;
}

const char *complearn_complearn_get_hostname(void)
{
  static char hostname[1024];
#ifdef __MINGW32__
	strcpy(hostname, "localhost");
#else
  gethostname(hostname, 1024);
#endif
  return hostname;
}

const char *complearn_get_uts_name(void)
{
  static char utsname[1024];
#if UTS_RDY
#ifdef __MINGW32__
	strcpy(utsname, "mingw32");
#else
  struct utsname uts;
  uname(&uts);
  sprintf(utsname, "[%s (%s), %s]", uts.sysname, uts.release, uts.machine);
#endif
#else
  strcpy(utsname, "[unknown]");
#endif
  return utsname;
}

GString *complearn_matrix_prettyprint_clb(LabeledMatrix *dm) {
  struct CLDistMatrix inp;
  memset(&inp, 0, sizeof(inp));
  inp.m = dm;
  char *old_locale = setlocale(LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, "C");
  GString *result = clRealWriteCLBDistMatrix(&inp);
  setlocale(LC_NUMERIC, old_locale);
  return result;
}

LabeledMatrix *complearn_load_any_matrix(const GString *inp)
{
  complearn_environment_top();
  if (complearn_is_nexus_file(inp))
    return complearn_load_nexus_matrix(inp);
  struct CLDistMatrix *cld;
  cld = complearn_read_clb_dist_matrix(inp);
  if (cld) {
    return cld->m;
  }
  if (complearn_is_text_matrix(inp)) {
    return complearn_load_text_matrix(inp);
  }
  return NULL;
}

