#include <stdio.h>
#include <complearn.h>

int main(int argc, char **argv)
{
  CompLearnEnvironment *cle = NULL;
  g_type_init();
  cle = complearn_environment_top();
  /* Just try to load top environment; a nonzero value is success */
  printf("%p\n", cle);
  return 0;
}

