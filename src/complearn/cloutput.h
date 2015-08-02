/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __CLOUTPUT_H
#define __CLOUTPUT_H


#include <math.h>

#include <gsl/gsl_blas.h>

/*! \file cloutput.h */

gsl_matrix *complearn_svd_project(gsl_matrix *a);
GString *complearn_matrix_prettyprint_text(LabeledMatrix *inp);
GString *complearn_matrix_prettyprint_nex(LabeledMatrix *inp, GString *treeblock);

struct CLDistMatrix {
  char *fileverstr;
  char *cllibver;
  char *username;
  char *hostname;
  char *title;
  char *compressor;
  char *creationTime; // Seconds since the epoch
  char * *cmds;
  char * *cmdtimes; // Seconds since the epoch
  LabeledMatrix *m;
  GArray *cmdsa;
  GArray *cmdtimesa;
  GArray *numa;
  GArray *labels1a;
  GArray *labels2a;
};

struct CLDistMatrix *complearn_read_clb_dist_matrix(const GString *db);
GArray *complearn_get_nexus_labels(const GString *db);
gsl_matrix *complearn_get_nexus_distance_matrix(const GString *db);
GString *complearn_matrix_prettyprint_clb(LabeledMatrix *result);
LabeledMatrix *complearn_load_nexus_matrix(const GString *inp);
LabeledMatrix *complearn_load_any_matrix(const GString *inp);
gboolean complearn_is_nexus_file(const GString *db);
GString *complearn_get_nexus_tree_block(const GString *db);
gboolean complearn_is_text_matrix(const GString *db);
LabeledMatrix *complearn_load_text_matrix(const GString *inp);

#endif

