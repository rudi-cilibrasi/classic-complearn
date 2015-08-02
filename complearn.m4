# Configure paths for COMPLEARN
# Raph Levien 98-11-18
# stolen from Manish Singh    98-9-30
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_COMPLEARN([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for COMPLEARN, and define COMPLEARN_CFLAGS and COMPLEARN_LIBS
dnl
AC_DEFUN([AM_PATH_COMPLEARN],
[dnl 
dnl Get the cflags and libraries from the complearn-config script
dnl
AC_ARG_WITH(complearn-prefix,[  --with-complearn-prefix=PFX   Prefix where COMPLEARN is installed (optional)],
            complearn_prefix="$withval", complearn_prefix="")
AC_ARG_WITH(complearn-exec-prefix,[  --with-complearn-exec-prefix=PFX Exec prefix where COMPLEARN is installed (optional)],
            complearn_exec_prefix="$withval", complearn_exec_prefix="")
AC_ARG_ENABLE(complearntest, [  --disable-complearntest       Do not try to compile and run a test COMPLEARN program],
		    , enable_complearntest=yes)

  if test x$complearn_exec_prefix != x ; then
     complearn_args="$complearn_args --exec-prefix"
     if test x${COMPLEARN_CONFIG+set} != xset ; then
        COMPLEARN_CONFIG=$complearn_exec_prefix/bin/complearn-config
     fi
  fi
  if test x$complearn_prefix != x ; then
     complearn_args="$complearn_args --prefix"
     if test x${COMPLEARN_CONFIG+set} != xset ; then
        COMPLEARN_CONFIG=$complearn_prefix/bin/complearn-config
     fi
  fi

  AC_PATH_PROG(COMPLEARN_CONFIG, complearn-config, no)
  min_complearn_version=ifelse([$1], ,0.2.5,$1)
  AC_MSG_CHECKING(for COMPLEARN - version >= $min_complearn_version)
  no_complearn=""
  if test "$COMPLEARN_CONFIG" = "no" ; then
    no_complearn=yes
  else
    COMPLEARN_CFLAGS=`$COMPLEARN_CONFIG $complearnconf_args --cflags`
    COMPLEARN_LIBS=`$COMPLEARN_CONFIG $complearnconf_args --libs`

    complearn_major_version=`$COMPLEARN_CONFIG $complearn_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    complearn_minor_version=`$COMPLEARN_CONFIG $complearn_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    complearn_micro_version=`$COMPLEARN_CONFIG $complearn_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_complearntest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
dnl      CFLAGS="$CFLAGS $COMPLEARN_CFLAGS"
dnl      LIBS="$LIBS $COMPLEARN_LIBS"
dnl
dnl Now check if the installed COMPLEARN is sufficiently new. (Also sanity
dnl checks the results of complearn-config to some extent
dnl
      rm -f conf.complearntest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *) malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.complearntest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_complearn_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_complearn_version");
     exit(1);
   }

   if (($complearn_major_version > major) ||
      (($complearn_major_version == major) && ($complearn_minor_version > minor)) ||
      (($complearn_major_version == major) && ($complearn_minor_version == minor) && ($complearn_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'complearn-config --version' returned %d.%d.%d, but the minimum version\n", $complearn_major_version, $complearn_minor_version, $complearn_micro_version);
      printf("*** of COMPLEARN required is %d.%d.%d. If complearn-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If complearn-config was wrong, set the environment variable COMPLEARN_CONFIG\n");
      printf("*** to point to the correct copy of complearn-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_complearn=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_complearn" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$COMPLEARN_CONFIG" = "no" ; then
       echo "*** The complearn-config script installed by COMPLEARN could not be found"
       echo "*** If COMPLEARN was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the COMPLEARN_CONFIG environment variable to the"
       echo "*** full path to complearn-config."
     else
       if test -f conf.complearntest ; then
        :
       else
          echo "*** Could not run COMPLEARN test program, checking why..."
          CFLAGS="$CFLAGS $COMPLEARN_CFLAGS"
          LIBS="$LIBS $COMPLEARN_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include <complearn/complearn.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding COMPLEARN or finding the wrong"
          echo "*** version of COMPLEARN. If it is not finding COMPLEARN, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means COMPLEARN was incorrectly installed"
          echo "*** or that you have moved COMPLEARN since it was installed. In the latter case, you"
          echo "*** may want to edit the complearn-config script: $COMPLEARN_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     COMPLEARN_CFLAGS=""
     COMPLEARN_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(COMPLEARN_CFLAGS)
  AC_SUBST(COMPLEARN_LIBS)
  rm -f conf.complearntest
])
