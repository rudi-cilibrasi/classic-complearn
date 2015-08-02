/** \file real-compressor.c */
#ifndef __COMPLEARN_REALCOMPRESSOR_H
#define __COMPLEARN_REALCOMPRESSOR_H

#define COMPLEARN_REAL_COMPRESSOR_TYPE        (real_compressor_get_type ())
#define COMPLEARN_TYPE_REAL_COMPRESSOR        (real_compressor_get_type ())
#define COMPLEARN_REAL_COMPRESSOR(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), COMPLEARN_REAL_COMPRESSOR_TYPE, CompLearnRealCompressor))
#define IS_COMPLEARN_REAL_COMPRESSOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), COMPLEARN_REAL_COMPRESSOR_TYPE))
#define COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), COMPLEARN_REAL_COMPRESSOR_TYPE, CompLearnRealCompressorIface))

#include <glib.h>
#include <glib-object.h>
#include <glib/garray.h>
#include <gsl/gsl_matrix.h>

#define COMPLEARN_ERROR 1
#define COMPLEARN_ERROR_NO_COMPRESSOR_SET 100

typedef struct _CompLearnRealCompressor CompLearnRealCompressor;
typedef struct _CompLearnRealCompressorIface CompLearnRealCompressorIface;

struct _CompLearnRealCompressorIface {
  GTypeInterface parent;

  GString *(*compress)(CompLearnRealCompressor *self, const GString *input);
  GString *(*decompress)(CompLearnRealCompressor *self, const GString *input);
  GString *(*blurb)(CompLearnRealCompressor *self);
  GString *(*canonical_extension)(CompLearnRealCompressor *self);
  GString *(*name)(CompLearnRealCompressor *self);
  GString *(*compressor_version)(CompLearnRealCompressor *self);
  GString *(*binding_version)(CompLearnRealCompressor *self);
  gboolean (*is_threadsafe)(CompLearnRealCompressor *self);
  gboolean (*is_compressible)(CompLearnRealCompressor *self, const GString *input);
  gboolean (*is_decompressible)(CompLearnRealCompressor *self, const GString *input);
  gboolean (*is_just_size)(CompLearnRealCompressor *self);
  gboolean (*is_hash_function)(CompLearnRealCompressor *self);
  GString *(*hash)(CompLearnRealCompressor *self, const GString *input);
  gboolean (*is_operational)(CompLearnRealCompressor *self);
  gboolean (*is_private_property)(CompLearnRealCompressor *self, const char *propname);
  gdouble (*compressed_size)(CompLearnRealCompressor *self, const GString *input);
  guint64 (*window_size)(CompLearnRealCompressor *self);
  CompLearnRealCompressor *(*clone)(CompLearnRealCompressor *self);
};

GType real_compressor_get_type(void);

/// Compress the string by the compressor
/// \param[in] self is a pointer to compressor
/// \param[in] input is a pointer to a GString which must be compressed
/// \return a pointer to compressed string
GString *real_compressor_compress(CompLearnRealCompressor *self,const GString *input);
GString *real_compressor_hash(CompLearnRealCompressor *self,const GString *input);
/// Decompress the string by the compressor
/// \param[in] self is pointer to compressor
/// \param[in] input is a pointer to a GString which must be decompressed
/// \return a pointer to decompressed string
GString *real_compressor_decompress(CompLearnRealCompressor *self,const GString *input);
/// Calculate size of the compressed string
/// \param[in] self is a pointer to a compressor
/// \param[in] input is a pointer to string which must be compress
/// \return size of compressed string in bits
gdouble real_compressor_compressed_size(CompLearnRealCompressor *self,const GString *input);
/// Return a short text description of this compressor
/// \param[in] self is a pointer to a compressor
GString *real_compressor_blurb(CompLearnRealCompressor *self);
GString *real_compressor_name(CompLearnRealCompressor *self);
/// Display the compressor version
/// param[in] is a pointer to compressor
/// \return a GString holding a 3-part version
GString *real_compressor_compressor_version(CompLearnRealCompressor *self);
/// Display the compressor binding version
/// \param[in] is a pointer to compressor
/// \return a GString holding a 3-part version
GString *real_compressor_binding_version(CompLearnRealCompressor *self);
/// This function checks whether the string is compressible by the compressor
/// \param[in] self is a pointer to compressor
/// \param[in] input is a pointer to GString which is under test
/// \return true for every string
gboolean real_compressor_is_compressible(CompLearnRealCompressor *self, const GString *input);
/// Check whether the string is decompressible by the compressor
/// \param[in] self is a pointer to compressor
/// \param[in] input is a pointer to string which is under test
gboolean real_compressor_is_decompressible(CompLearnRealCompressor *self, const GString *input);


gboolean real_compressor_is_private_property(CompLearnRealCompressor *self, const char *input);
guint64 real_compressor_window_size(CompLearnRealCompressor *self);
gboolean real_compressor_is_threadsafe(CompLearnRealCompressor *self);
gboolean real_compressor_is_just_size(CompLearnRealCompressor *self);
gboolean real_compressor_is_hash_function(CompLearnRealCompressor *self);
GString *real_compressor_canonical_extension(CompLearnRealCompressor *rc);
gboolean real_compressor_is_operational(CompLearnRealCompressor *self);
CompLearnRealCompressor *real_compressor_clone(CompLearnRealCompressor *self);

#define SET_DEFAULT_PROPS(groupname, clt, mobj) \
  do { \
  GParamSpec **gps, **cur; \
  g_assert(mobj != NULL); \
    if (complearn_environment_get_nameable(groupname) == NULL) { \
      complearn_environment_register_nameable(groupname, G_OBJECT(mobj)); \
      break; \
    } \
  gps =g_object_class_list_properties(G_OBJECT_CLASS(clt(mobj)), NULL); \
  for (cur = gps; *cur; cur += 1) { \
    GValue v = {0,}; \
    g_value_init(&v, (*cur)->value_type); \
    g_param_value_set_default(*cur, &v); \
    g_object_set_property(G_OBJECT(mobj), (*cur)->name, &v); \
    complearn_environment_register_property(G_OBJECT(mobj), (*cur), &v); \
  } } while(0)

#define SET_DEFAULT_COMPRESSOR_PROPS(groupname, clt, mobj) \
  do { \
      SET_DEFAULT_PROPS(groupname, clt, mobj); \
    if (complearn_environment_get_nameable(groupname) == NULL) { \
      complearn_environment_register_compressor(mobj); \
    } \
  } while (0);

#endif

#define G_LOG_LEVEL_NOTICE G_LOG_LEVEL_USER_SHIFT
#define g_notice(...) g_log(G_LOG_DOMAIN, G_LOG_LEVEL_NOTICE, __VA_ARGS__)

void real_compressor_interface_init (gpointer g_iface, gpointer iface_data);
