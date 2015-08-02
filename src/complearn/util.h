#ifndef __UTIL_H
#define __UTIL_H

#include <glib.h>
#include <gsl/gsl_matrix.h>

struct _CLArgumentEntry {
  GString *block;
  char *label;
};

struct _LabeledMatrix {
  gsl_matrix *mat;
  char **labels1;
  char **labels2;
};

typedef struct _LabeledMatrix LabeledMatrix ;
typedef struct _CLArgumentEntry CLArgumentEntry ;

char *complearn_make_temp_dir_name(void);
char *complearn_make_temp_dir(void);
int make_directory(const char *name);
int complearn_make_directory_if_necessary(const char *name);
GString *complearn_read_whole_file(const char *fname);
int complearn_count_strings(const char * const * str);
int complearn_remove_directory_recursively(const char *dirname, int f_err_out);
GString *complearn_read_whole_file_ptr(FILE *fp);
GString *complearn_read_whole_file_io(GIOChannel *gio);
GSList *complearn_read_directory_of_files(const char *dirname);
GSList *complearn_read_list_of_files(const char *filename);
GSList *complearn_read_list_of_strings(const char *filename);
gsl_matrix *complearn_average_matrix(gsl_matrix *a);
gsl_matrix *complearn_svd_project(gsl_matrix *a);
CLArgumentEntry *complearn_new_arg(GString *block, const char *label);
int complearn_get_pid(void);
char *complearn_get_hostname();
char ** complearn_dupe_strings(const char * const * str);
char ** complearn_fix_labels(char **inp);
char *complearn_make_temp_file_name(void);
char *complearn_chdir(const char *newdir);
int complearn_write_file(const char *fname, const GString *f);
void complearn_mrbreaker(void);


#endif
