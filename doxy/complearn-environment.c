#include <glib.h>
/** \file complearn-environment.c */

/**
 * \struct GObject
 *
 * This data structure is fully documented at
 * http://library.gnome.org/devel/gobject/unstable/index.html
 * \sa http://library.gnome.org/devel/gobject/unstable/index.html
 **/
struct GObject {
};

/**
 * \struct GType
 *
 * This data structure is fully documented at
 * http://library.gnome.org/devel/gobject/unstable/gobject-Type-Information.html
 * \sa http://library.gnome.org/devel/gobject/unstable/gobject-Type-Information.html
 **/
struct GType {
};
/**
 * \struct CompLearnEnvironment
 *
 * Top-level singleton class that is instantiated on library initialization.
 * This class is only constructed once per process and is usually made via
 * the complearn_environment_top() function from the library user perspective.
 *
 **/
struct CompLearnEnvironment {
};

/** CompLearn library initialization function.
 * This function is usually called automatically, so is not necessary for
 * normal use of the CompLearn library.
 **/
void  complearn_init (void);

GType   complearn_environment_get_type (void);

/**
 * Constructs a new (singleton) CompLearnEnvironment instance.
 * This function is not normally necessary for the user to call.
 * Instead, use complearn_environment_top() to access the singleton
 * instance.
 *
 * \return pointer to the CompLearnEnvironment instance.
 **/
GObject *   complearn_environment_new (void);
/**
 * Retrieve the current module search directory path.
 * This attribute is a character pointer.
 * \param env pointer to singleton CompLearnEnvironment * instance
 * \return gchar * representing search path
 **/
gchar *   complearn_environment_get_module_path (CompLearnEnvironment *env);

/**
 * Adjust the module search directory path.
 * This attribute is a character pointer.
 * \param env pointer to singleton CompLearnEnvironment * instance
 * \param val gchar * representing new module search path
 * \return nothing
 **/
void  complearn_environment_set_module_path (CompLearnEnvironment *env, gchar *val);
gboolean  complearn_environment_get_verbose (CompLearnEnvironment *env);
void  complearn_environment_set_verbose (CompLearnEnvironment *env, gboolean val);
gint  complearn_environment_get_nice_level (CompLearnEnvironment *env);
void  complearn_environment_set_nice_level (CompLearnEnvironment *env, gint val);

/**
 * Create or retrieve the global CompLearnEnvironment singleton instance.
 * This is the normal way to initialize the CompLearn library.
 * There is only one (singleton) instance of a CompLearnEnvironment object
 * in any process.
 *
 * \return pointer to the CompLearnEnvironment instance.
 **/
CompLearnEnvironment *  complearn_environment_top (void);

/*
char *  complearn_environment_version (void);
CompLearnRealCompressor *   complearn_environment_load_compressor_named (const char *name);
GObject *   complearn_environment_get_nameable (const char *name);
void  complearn_environment_register_nameable (const char *name, GObject *val);
void  complearn_environment_register_compressor_and_nameable (GObject *obj);
void  complearn_environment_register_compressor (CompLearnRealCompressor *rc);
int   complearn_environment_compressor_count (void);
char **   complearn_environment_compressor_list (void);
GString *   complearn_environment_lib_version (void);
void  complearn_environment_make_home_config_file (void);
char *  complearn_environment_get_home_config_filename (void);
char **   complearn_environment_get_config_filename_list (void);
void  complearn_environment_apply_configs (gboolean env_part, gboolean other_part);
void  complearn_environment_show_properties (void);
char *  complearn_environment_find_name (GObject *obj);
void  complearn_environment_register_property (GObject *obj, GParamSpec *gps, GValue *v);
gboolean  complearn_environment_check_homedir_subdir (void);
void  complearn_environment_glog_func (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);
void  complearn_environment_handle_log_domain (const gchar *log_domain);
void  complearn_environment_standard_initialization (void);
void  complearn_environment_adjust_parameter (GObject *obj, const char *kname, const char *val);
void  complearn_environment_apply_keyfile (GKeyFile *kf, GObject *obj);
void  complearn_environment_print_version (void);
void  complearn_environment_set_verbose_mode (void);
GString *   complearn_environment_convert_to_hex (GString *inp);
*/
