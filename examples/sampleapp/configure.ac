
AC_INIT(librtest,0.4.9,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/rtest.c)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AM_MAINTAINER_MODE

AM_PATH_COMPLEARN(0.9.9, [
  CFLAGS="$CFLAGS $COMPLEARN_CFLAGS"
  LIBS="$LIBS $COMPLEARN_LIBS"
  ], [
    AC_MSG_ERROR(must have complearn >= 0.9.9 installed)
] )

AM_PATH_GSL(0.8.0, [
  CFLAGS="$CFLAGS $GSL_CFLAGS"
  LIBS="$LIBS $GSL_LIBS"
  ], [
    AC_MSG_ERROR(must have gsl >= 0.8.0 installed)
] )

AM_PATH_GLIB_2_0(2.0.0, [
  CFLAGS="$CFLAGS $GLIB_CFLAGS"
  LIBS="$LIBS $GLIB_LIBS"
  ], [
    AC_MSG_ERROR(must have glib >= 2.0.0 and gobject installed)
] , [ gobject gmodule ] )

PRIVATELIBS="$LIBS"

AC_SUBST(PRIVATELIBS)
AC_SUBST(COMPLEARN_CFLAGS)
AC_SUBST(COMPLEARN_LIBS)

AC_OUTPUT([ Makefile src/Makefile ])
