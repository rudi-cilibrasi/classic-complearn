#include <complearn/complearn.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>

struct CompLearnCommand {
  const gchar *name;
  void (*execute)(GString *inpkt, guint32 *curpos, GString *opkt);
};

static  CompLearnRealCompressor *decode_realcompressor(GString *inpkt, guint32 *curpos);
static  CompLearnCompressorDriver *decode_compressordriver(GString *inpkt, guint32 *curpos);

GSList *ptr_real_compressor_list, *ptr_compressor_driver_list;

static guint32 toexit = 0;

#define DECLCMD(cmdname) \
void cmd_##cmdname(GString *inpkt, guint32 *curpos, GString *opkt);

DECLCMD(compressor_list)
DECLCMD(compress)
DECLCMD(clone)
DECLCMD(decompress)
DECLCMD(blurb)
DECLCMD(canonical_extension)
DECLCMD(name)
DECLCMD(compressor_version)
DECLCMD(binding_version)
DECLCMD(is_threadsafe)
DECLCMD(is_decompressible)
DECLCMD(is_just_size)
DECLCMD(is_hash_function)
DECLCMD(hash)
DECLCMD(is_operational)
DECLCMD(compressed_size)
DECLCMD(window_size)
DECLCMD(clone)
DECLCMD(new_compressor)
DECLCMD(free_compressor)
DECLCMD(quit)
//DECLCMD(is_private_property)
//DECLCMD(is_compressible)
//MAKESTRBOOLFUNC(is_compressible)

DECLCMD(new_compressordriver)
DECLCMD(free_compressordriver)
DECLCMD(cd_size)
DECLCMD(cd_is_same_block)
DECLCMD(cd_store)
DECLCMD(cd_compress_single)
DECLCMD(cd_compress_pair)
DECLCMD(cd_compression_sequence)
DECLCMD(cd_compression_vector)
DECLCMD(cd_compression_matrix)
DECLCMD(cd_execute_instruction)

#define MAKECMD(x) { #x, cmd_##x },
struct CompLearnCommand ctab[] = {
MAKECMD(compressor_list)
MAKECMD(compress)
MAKECMD(clone)
MAKECMD(decompress)
MAKECMD(blurb)
MAKECMD(canonical_extension)
MAKECMD(name)
MAKECMD(compressor_version)
MAKECMD(binding_version)
MAKECMD(is_threadsafe)
MAKECMD(is_decompressible)
MAKECMD(is_just_size)
MAKECMD(is_hash_function)
MAKECMD(hash)
MAKECMD(is_operational)
MAKECMD(compressed_size)
MAKECMD(window_size)
MAKECMD(clone)
MAKECMD(new_compressor)
MAKECMD(free_compressor)
MAKECMD(quit)

MAKECMD(new_compressordriver)
MAKECMD(free_compressordriver)
MAKECMD(cd_size)
MAKECMD(cd_is_same_block)
MAKECMD(cd_store)
MAKECMD(cd_compress_single)
MAKECMD(cd_compress_pair)
MAKECMD(cd_compression_sequence)
MAKECMD(cd_compression_vector)
MAKECMD(cd_compression_matrix)
MAKECMD(cd_execute_instruction)

{ NULL, NULL }
};

GString *wrap_packet(GString *inp);
GString *unwrap_packet(GIOChannel *gi);

gdouble decode_double(GString *pkt, guint32 *curpos);

CompLearnCompressorDriverCommand decode_cdc(GString *pkt, guint32 *curpos);
GArray * decode_compseq(GString *pkt, guint32 *curpos);
GArray * decode_indexseq(GString *pkt, guint32 *curpos);
guint32 decode_matspec(GString *pkt, guint32 *curpos);

guint32 decode_integer(GString *pkt, guint32 *curpos);
gboolean decode_boolean(GString *pkt, guint32 *curpos);
GString *decode_string(GString *pkt, guint32 *curpos);
void *decode_pointer(GString *pkt, guint32 *cur_offset);

void encode_boolean(GString *pkt, gboolean b);
void encode_pointer(GString *pkt, void *ptr);
void encode_double(GString *pkt, gdouble g);
void encode_string(GString *pkt, GString *enc);
void encode_cstring(GString *pkt, char *str);
void encode_integer(GString *pkt, guint32 i);
void encode_gslvector(GString *pkt, gsl_vector *g);
void encode_gslmatrix(GString *pkt, gsl_matrix *g);

void process_commands(GIOChannel *inpchan);


void process_commands(GIOChannel *inpchan)
{
  GString *pkt = unwrap_packet(inpchan);
  guint32 cur_offset = 0;
    GString *cmd = decode_string(pkt, &cur_offset);
    int i;
    for (i = 0; ctab[i].name; i += 1) {
      if (strcmp(ctab[i].name, cmd->str) == 0) {
//        fprintf(stderr, "Executing command: %s\n", cmd->str);
        g_string_free(cmd, TRUE);
        GString *opkt = g_string_new("");
        ctab[i].execute(pkt, &cur_offset, opkt);
        GString *wopkt = wrap_packet(opkt);
        write(1, wopkt->str, wopkt->len);
        g_string_free(opkt, TRUE);
        g_string_free(wopkt, TRUE);
        break;
      }
    }
    if (ctab[i].name == NULL) {
      fprintf(stderr, "Unknown command '%s', exitting\n", cmd->str);
      exit(1);
    }
  g_string_free(pkt, TRUE);
}

void cmd_compressor_list(GString *inpkt, guint32 *curpos, GString *opkt) {
  char **complist = complearn_environment_compressor_list();
  g_strv_length(complist);
  encode_integer(opkt, g_strv_length(complist));
  int i;
  for (i = 0; complist[i]; i += 1)
    encode_cstring(opkt, complist[i]);
  g_strfreev(complist);
}

/*
void cmd_is_private_property(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos);
  GString *plainmsg = decode_string(inpkt, curpos);
  gboolean result = real_compressor_is_private_property(res, plainmsg->str);
  encode_boolean(opkt, result);
  g_string_free(plainmsg, TRUE);
}
*/

void cmd_clone(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos);
  void * result = real_compressor_clone(res);
  ptr_real_compressor_list = g_slist_prepend(ptr_real_compressor_list, result);
  encode_pointer(opkt, result);
}

void cmd_window_size(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos);
  guint32 result = real_compressor_window_size(res);
  encode_integer(opkt, result);
}

void cmd_compressed_size(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos);
  GString *plainmsg = decode_string(inpkt, curpos);
  gdouble result = real_compressor_compressed_size(res, plainmsg);
  encode_double(opkt, result);
  g_string_free(plainmsg, TRUE);
}

void cmd_free_compressor(GString *inpkt, guint32 *curpos, GString *opkt)
{
  void *ptr = decode_realcompressor(inpkt, curpos);
  g_object_unref(ptr);
  ptr_real_compressor_list = g_slist_remove(ptr_real_compressor_list, ptr);
  encode_integer(opkt, 0);
}

static  CompLearnCompressorDriver *decode_compressordriver(GString *inpkt, guint32 *curpos) {
  void *ptr = decode_pointer(inpkt,curpos);
  if (g_slist_index(ptr_compressor_driver_list, ptr) == -1) {
    fprintf(stderr, "No such compressor driver %p, exitting\n", ptr);
    exit(1);
  }

 return (CompLearnCompressorDriver *) ptr;
}

static  CompLearnRealCompressor *decode_realcompressor(GString *inpkt, guint32 *curpos) {
  void *ptr = decode_pointer(inpkt,curpos);
  if (g_slist_index(ptr_real_compressor_list, ptr) == -1) {
    fprintf(stderr, "No such compressor %p, exitting\n", ptr);
    exit(1);
  }

 return (CompLearnRealCompressor *) ptr;
}

#define IMPLEMENTSIMPLEBOOL(funcname) \
void cmd_##funcname(GString *inpkt, guint32 *curpos, GString *opkt) \
{ \
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos); \
  gboolean result = real_compressor_##funcname(res); \
  encode_boolean(opkt, result); \
}

#define IMPLEMENTSIMPLESTR(funcname) \
void cmd_##funcname(GString *inpkt, guint32 *curpos, GString *opkt) \
{ \
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos); \
  GString *result = real_compressor_##funcname(res); \
  encode_string(opkt, result); \
  g_string_free(result, TRUE); \
}

IMPLEMENTSIMPLESTR(name)
IMPLEMENTSIMPLESTR(compressor_version)
IMPLEMENTSIMPLESTR(binding_version)
IMPLEMENTSIMPLESTR(blurb)
IMPLEMENTSIMPLESTR(canonical_extension)

IMPLEMENTSIMPLEBOOL(is_threadsafe)
IMPLEMENTSIMPLEBOOL(is_just_size)
IMPLEMENTSIMPLEBOOL(is_hash_function)
IMPLEMENTSIMPLEBOOL(is_operational)


void cmd_quit(GString *inpkt, guint32 *curpos, GString *opkt)
{
  encode_integer(opkt, 0);
  toexit = 1;
}

#define MAKESTRSTRFUNC(funcname) \
void cmd_##funcname(GString *inpkt, guint32 *curpos, GString *opkt) \
{ \
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos); \
  GString *plainmsg = decode_string(inpkt, curpos); \
  GString *result = real_compressor_##funcname(res, plainmsg); \
  encode_string(opkt, result); \
  g_string_free(plainmsg, TRUE); \
  g_string_free(result, TRUE); \
}

#define MAKESTRBOOLFUNC(funcname) \
void cmd_##funcname(GString *inpkt, guint32 *curpos, GString *opkt) \
{ \
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos); \
  GString *plainmsg = decode_string(inpkt, curpos); \
  gboolean result = real_compressor_##funcname(res, plainmsg); \
  encode_boolean(opkt, result); \
  g_string_free(plainmsg, TRUE); \
}

MAKESTRSTRFUNC(compress)
MAKESTRSTRFUNC(decompress)
MAKESTRSTRFUNC(hash)
MAKESTRBOOLFUNC(is_decompressible)

void cmd_new_compressor(GString *inpkt, guint32 *curpos, GString *opkt)
{
  GString *compname = decode_string(inpkt, curpos);
  CompLearnRealCompressor *res = complearn_environment_load_compressor_named(compname->str);
  encode_pointer(opkt, res);
  if (res != NULL) {
    ptr_real_compressor_list = g_slist_prepend(ptr_real_compressor_list, res);
  }
  g_string_free(compname, TRUE);
}

void *decode_pointer(GString *pkt, guint32 *cur_offset)
{
  guint64 u = 0;
  void *ptr;
  GString *str = decode_string(pkt, cur_offset);
  sscanf(str->str, "%lu", &u);
  ptr = (void *) u;
  g_string_free(str, TRUE);
  return ptr;
}

void encode_pointer(GString *pkt, void *ptr)
{
  char buf[128];
  guint64 u = (guint64) ptr;
  sprintf(buf, "%lu", u);
  encode_cstring(pkt, buf);
}

void encode_cstring(GString *pkt, char *str)
{
  GString *med = g_string_new(str);
  encode_string(pkt, med);
  g_string_free(med, TRUE);
}

void encode_double(GString *pkt, gdouble g)
{
  char buf[255];
  sprintf(buf, "%lf", g);
  GString *str = g_string_new(buf);
  GString *wrapped = wrap_packet(str);
  g_string_append_len(pkt, wrapped->str, wrapped->len);
  g_string_free(wrapped, TRUE);
  g_string_free(str, TRUE);
}

void encode_string(GString *pkt, GString *enc)
{
  GString *wrapped = wrap_packet(enc);
  g_string_append_len(pkt, wrapped->str, wrapped->len);
  g_string_free(wrapped, TRUE);
}

void encode_integer(GString *pkt, guint32 i)
{
  guint32 j = GUINT32_TO_BE(i);
  g_string_append_len(pkt, (const gchar *) &j, 4);
}

void encode_boolean(GString *pkt, gboolean b)
{
  encode_integer(pkt, (guint32) b);
}

gboolean decode_boolean(GString *pkt, guint32 *curpos)
{
  gboolean result;
  result = decode_integer(pkt, curpos);
  return result;
}

gdouble decode_double(GString *pkt, guint32 *curpos)
{
  gdouble result;
  GString *str = decode_string(pkt, curpos);
  result = atof(str->str);
  g_string_free(str, TRUE);
  return result;
}

guint32 decode_integer(GString *pkt, guint32 *curpos)
{
  guint32 result;
  g_assert((*curpos) + 3 < pkt->len);
  result = GUINT32_FROM_BE(*(guint32 *)((gchar *)(pkt->str) + (*curpos)));
  curpos[0] += 4;
  return result;
}

void encode_gslmatrix(GString *pkt, gsl_matrix *g)
{
  encode_integer(pkt, g->size1);
  encode_integer(pkt, g->size2);
  int i, j;
  for (i = 0; i < g->size1; i += 1)
    for (j = 0; j < g->size2; j += 1)
      encode_double(pkt, gsl_matrix_get(g, i,j));
}

void encode_gslvector(GString *pkt, gsl_vector *g)
{
  encode_integer(pkt, g->size);
  int i;
  for (i = 0; i < g->size; i += 1)
    encode_double(pkt, gsl_vector_get(g, i));
}

CompLearnCompressorDriverCommand decode_cdc(GString *pkt, guint32 *curpos)
{
  int i = decode_integer(pkt, curpos);
  int a = decode_integer(pkt, curpos);
  if (i == 1)
    return complearn_compressor_driver_command_single(a);
  int b = decode_integer(pkt, curpos);
  if (i == 2)
    return complearn_compressor_driver_command_pair(a, b);
  fprintf(stderr, "Must have command with 1-2 elements, not %d. exitting\n", i);
  exit(1);
}

GArray * decode_indexseq(GString *pkt, guint32 *curpos)
{
  GArray *res=g_array_new(FALSE,TRUE, sizeof(guint32));
  int len = decode_integer(pkt, curpos);
  int i;
//  fprintf(stderr, "Decoding integer array of size %d\n", len);
  for (i = 0; i < len; i += 1) {
    guint32 j = decode_integer(pkt, curpos);
    g_array_append_vals(res, &j, 1);
  }
  return res;
}

guint32 decode_matspec(GString *pkt, guint32 *curpos)
{
  guint32 result = CL_MATRIX_FULL;
  GString *res = decode_string(pkt, curpos);
  if (strcmp(res->str, "full") == 0) {
    result = CL_MATRIX_FULL;
    goto done;
  }
  if (strcmp(res->str, "bigx") == 0) {
    result = CL_MATRIX_BIGXTRIANGLE;
    goto done;
  }
  if (strcmp(res->str, "bigy") == 0) {
    result = CL_MATRIX_BIGYTRIANGLE;
    goto done;
  }
  fprintf(stderr, "Unknown matspec <%s>\n", res->str);
  exit(1);
  done:
    g_string_free(res, TRUE);
    return result;
}

GArray * decode_compseq(GString *pkt, guint32 *curpos)
{
  GArray *res=g_array_new(FALSE,TRUE, sizeof(CompLearnCompressorDriverCommand));
  int len = decode_integer(pkt, curpos);
  int i;
  for (i = 0; i < len; i += 1) {
    CompLearnCompressorDriverCommand cdc = decode_cdc(pkt, curpos);
    g_array_append_vals(res, &cdc, 1);
  }
  return res;
}

GString *decode_string(GString *pkt, guint32 *curpos)
{
  guint32 len;
  g_assert((*curpos) + 3 < pkt->len);
  len = GUINT32_FROM_BE(*(guint32 *)((gchar *)(pkt->str) + (*curpos)));
  curpos[0] += 4;
  g_assert((*curpos) + len - 1 < pkt->len);
  GString *result = g_string_new_len(((gchar *)(pkt->str) + (*curpos)),len);
  curpos[0] += len;
//  printf("String <%s>", result->str);
  return result;
}

GString *unwrap_packet(GIOChannel *gi)
{
  guint32 pkt_header;
  gsize actual;
  GError *err = NULL;
  g_io_channel_read_chars(gi, (gchar *) &pkt_header, 4, &actual, &err);
  if (err != NULL) {
    fprintf(stderr, "Error reading packet: %s\n", err->message);
    exit(1);
  }
  if (actual == 0)
    exit(0); // clean exit
  if (actual != 4) {
    fprintf(stderr, "Short packet error: %d/%d\n", actual, 4);
    exit(1);
  }
  pkt_header = GUINT32_FROM_BE(pkt_header);
  g_assert(pkt_header >= 0 && pkt_header < 2000000000);
  if (pkt_header == 0)
    return g_string_new("");
  char *buf = g_malloc0(pkt_header);
  err = NULL;
  g_io_channel_read_chars(gi, (gchar *) buf, pkt_header, &actual, &err);
  if (err != NULL) {
    fprintf(stderr, "Error reading packet: %s\n", err->message);
    exit(1);
  }
  if (actual != pkt_header) {
    fprintf(stderr, "Short packet error: %d/%d\n", actual,pkt_header);
    exit(1);
  }
  GString *result = g_string_new_len(buf, pkt_header);
  g_free(buf);
  return result;
}

GString *wrap_packet(GString *inp)
{
  guint32 pkt_header;
  pkt_header = GUINT32_TO_BE(inp->len);
  GString *result = g_string_new_len((gchar *) &pkt_header, 4);
  g_string_append_len(result, (const gchar *) inp->str, inp->len);
  return result;
}

void cmd_new_compressordriver(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnRealCompressor *res=decode_realcompressor(inpkt,curpos);
  void *result = complearn_compressor_driver_new();
  complearn_compressor_driver_set_compressor((CompLearnCompressorDriver *) result, res);
  ptr_compressor_driver_list = g_slist_prepend(ptr_compressor_driver_list, result);
  encode_pointer(opkt, result);
}

void cmd_free_compressordriver(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  g_object_unref(res);
  ptr_compressor_driver_list = g_slist_remove(ptr_compressor_driver_list, res);
  encode_integer(opkt, 0);
}

void cmd_cd_size(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  encode_integer(opkt, complearn_compressor_driver_size(res));
}

void cmd_cd_is_same_block(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  guint32 a = decode_integer(inpkt, curpos);
  guint32 b = decode_integer(inpkt, curpos);
  encode_boolean(opkt, complearn_compressor_driver_is_same_block(res,a,b));
}

void cmd_cd_store(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  GString *block = decode_string(inpkt, curpos);
  encode_integer(opkt, complearn_compressor_driver_store(res,block));
  g_string_free(block, TRUE);
}

void cmd_cd_compress_single(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  guint32 a = decode_integer(inpkt, curpos);
  encode_double(opkt, complearn_compressor_driver_compress_single(res,a));
}

void cmd_cd_compress_pair(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  guint32 a = decode_integer(inpkt, curpos);
  guint32 b = decode_integer(inpkt, curpos);
  encode_double(opkt, complearn_compressor_driver_compress_pair(res,a, b));
}

void cmd_cd_compression_sequence(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  GArray *seq = decode_compseq(inpkt,curpos);
  gsl_vector *result=complearn_compressor_driver_compression_sequence(res,seq);
  encode_gslvector(opkt, result);
  g_array_free(seq, TRUE);
  gsl_vector_free(result);
}

void cmd_cd_compression_vector
(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  gsl_vector *result=complearn_compressor_driver_compression_vector(res);
  encode_gslvector(opkt, result);
  gsl_vector_free(result);
}

void cmd_cd_compression_matrix
(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
//  fprintf(stderr, "CCDres is %p\n", res);
  GArray *dim1, *dim2;
  dim1 = decode_indexseq(inpkt, curpos);
  dim2 = decode_indexseq(inpkt, curpos);
  guint32 matspec = decode_matspec(inpkt, curpos);
  gsl_matrix *result=complearn_compressor_driver_compression_matrix(res, dim1, dim2, matspec);
  g_array_free(dim1, TRUE);
  g_array_free(dim2, TRUE);
  encode_gslmatrix(opkt, result);
  gsl_matrix_free(result);
}

void cmd_cd_execute_instruction
(GString *inpkt, guint32 *curpos, GString *opkt)
{
  CompLearnCompressorDriver *res=decode_compressordriver(inpkt,curpos);
  CompLearnCompressorDriverCommand cdc;
  cdc = decode_cdc(inpkt, curpos);
  double result = complearn_compressor_driver_execute_instruction(res,cdc);
  encode_double(opkt, result);
}

int umain(void)
{
  GIOChannel *inc = g_io_channel_unix_new(0);
  g_io_channel_set_encoding(inc, NULL, NULL);
  while (!toexit) {
    process_commands(inc);
  }
  exit(0);
  return 0;
}
