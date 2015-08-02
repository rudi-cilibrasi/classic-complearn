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
#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>
#include <getopt.h>
#include <locale.h>
#include <libintl.h>
#include "complearn/complearn.h"
#include "complearn/clconfig.h"

#define _(O) gettext(O)

static void print_anycompress_help_and_exit(void) {
    fprintf(stderr, "%s",
_("Usage: anycompress [options] <infile> [<outfile>]\n"
"\n"
"OPTIONS:                                                              \n"
"  -c, --compressor=COMPNAME   set compressor to use                   \n"
"  -h, --help                  print this help message                 \n"
"  -L, --list                  list compressors                        \n"
"  -v, --verbose               print extra detailed information        \n"
"\n")
);
    exit(0);
  }

int main(int argc, char **argv)
{
  char t[1] = { 0 };
  setlocale(LC_ALL, t);
  bindtextdomain (complearn_package_name, "/usr/share/locale");
  textdomain (complearn_package_name);
  g_type_init ();
  complearn_environment_handle_log_domain(G_LOG_DOMAIN);
  complearn_ncd_set_filelist_mode();
  optind = 1;
  int option_index = 0, c;
  struct option long_options[] = {
    {"help", 0, 0, 'h'},
    {"verbose", 0, 0, 'v'},
    {"compressor", 0, 0, 'c'},
    {"list", 0, 0, 'L'},
    { 0, 0, 0, 0 } };
  while (1) {
    c = getopt_long(argc, (char **) argv, "hvc:L", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
      case 'h': print_anycompress_help_and_exit(); break;
      case 'v': complearn_environment_set_verbose_mode(); break;
      case 'c': complearn_ncd_set_compressor(complearn_ncd_top(), optarg);break;
      case 'L': complearn_ncd_print_compressor_list(); break;
      case '?': exit(1);
      default:
        g_error("%s'%c'\n", _("Unrecognized option: "), c); break;
    }
  }
  CompLearnRealCompressor *rc = complearn_ncd_load_compressor(
    complearn_ncd_get_compressor(complearn_ncd_top()));
  if (real_compressor_is_just_size(rc))
    g_error(_("Compressor %s is a non-coding compressor."),
             real_compressor_name(rc)->str);
  FILE *inFile = NULL, *outFile = NULL;
  if (optind >= argc) {
    inFile = stdin;
    outFile = stdout;
  } else {
    char *in_filename, *out_filename = NULL;
    in_filename = g_strdup(argv[optind]);
    if (optind + 1 == argc)
      out_filename = g_strconcat(in_filename, ".",
  real_compressor_canonical_extension(rc)->str,
  NULL);
    else {
      if (optind + 2 == argc)
        out_filename = g_strdup(argv[optind+1]);
      else
        g_error(_("Don't know what to do with extra arguments."));
    }
    g_notice(_("Opening output file %s"), out_filename);
    inFile = fopen(in_filename, "rb");
    if (inFile == NULL)
      g_error("Cannot open %s", in_filename);
    outFile = fopen(out_filename, "wb");
    if (outFile == NULL)
      g_error("Cannot open %s for writing", out_filename);
  }
  GString *compblock = complearn_read_whole_file_ptr(inFile);
  GString *outblock = real_compressor_compress(rc, compblock);
  fwrite(outblock->str, 1, outblock->len, outFile);
  return 0;
}
