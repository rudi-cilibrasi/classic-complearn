AC_INIT(libcomplearn,1.1.8,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AC_GNU_SOURCE
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AM_MAINTAINER_MODE

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4], [
  EXTCPPFLAGS="$EXTCPPFLAGS $CHECK_CFLAGS"
  LIBS="$LIBS $CHECK_LIBS"
  ], [
  echo -n
]
  )

if test "x$CHECK_LIBS" = x ; then
  HAVECHECK=0
else
  HAVECHECK=1
fi

AC_PATH_PROG(DOXYGEN, doxygen, "")
if test x$DOXYGEN = x ; then
  HAVEDOXYGEN=0
else
  HAVEDOXYGEN=1
fi

AC_PATH_PROG(GOB2, gob2, "")
if test x$GOB2 = x ; then
  HAVEGOB2=0
else
  HAVEGOB2=1
fi

AC_ARG_ENABLE(openmp,
[  --enable-openmp  Enable OpenMP to support multicore processors
],
[case "${enableval}" in
   yes) ua_openmp=true ;;
   no) ua_openmp=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-openmp) ;;
esac], [ua_openmp=false])

AX_OPENMP([ua_foundopenmp=true],[ua_foundopenmp=false])
if test x$ua_openmp = xmaybe ; then
  ua_openmp=$ua_foundopenmp
fi

if test x$ua_openmp = xtrue ; then
  if test x$ua_foundopenmp = xtrue ; then
    CFLAGS="$CFLAGS $OPENMP_CFLAGS"
    PRIVATECFLAGS="$OPENMP_CFLAGS -DOPENMP_ENABLED=1 -DREENTRANT"
    CPPFLAGS="$CPPFLAGS -DREENTRANT -DOPENMP_ENABLED=1"
    OPENMPLIBS=`pkg-config --libs gthread-2.0`
    LIBS="$LIBS $OPENMPLIBS"
  else
    echo "Cannot find OpenMP library, switching to single-threaded variant."
    echo "Please reconsider openmp support for more convenient multicore use."
    CPPFLAGS="$CPPFLAGS -DOPENMP_ENABLED=0"
    PRIVATECFLAGS="-DOPENMP_ENABLED=0"
  fi
else
    CPPFLAGS="$CPPFLAGS -DOPENMP_ENABLED=0"
    PRIVATECFLAGS="-DOPENMP_ENABLED=0"
fi

AM_PATH_GSL(0.8.0, [
  CFLAGS="$CFLAGS $GSL_CFLAGS"
  LIBS="$LIBS $GSL_LIBS"
  ], [
    AC_MSG_ERROR(must have gsl >= 0.8.0 installed)
] )

AM_PATH_XML2(2.4.0, [
  CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
  LIBS="$LIBS $XML_LIBS"
], [
    AC_MSG_ERROR(must have lixbml2 >= 2.4.0 installed)
] )

AM_PATH_GLIB_2_0(2.0.0, [
  CFLAGS="$CFLAGS $GLIB_CFLAGS"
  LIBS="$LIBS $GLIB_LIBS"
  ], [
    AC_MSG_ERROR(must have glib >= 2.0.0 and gobject installed)
] , [ gobject gmodule ] )

CHECK_ZLIB()

if test x$zlib_cv_libz = xno -o x$zlib_cv_zlib_h = xno ; then
    AC_MSG_ERROR(must have libz (zlib) installed)
fi

AC_CHECK_HEADERS([bzlib.h], [
  AC_CHECK_LIB(bz2, BZ2_bzBuffToBuffCompress, [LIBS="$LIBS -lbz2"
                                               fbz2_lib=true],
               [ AC_MSG_ERROR([bzlib.h header found but no bz2 library]) ]) ])

AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([errno.h])

PRIVATELIBS="$LIBS -ldl"
LIBS=""

if test x$ua_openmp = xtrue ; then
  GOMPSTR="-gomp"
  GOMPFLAGS="-fopenmp"
  cp $srcdir/complearn.pc.in complearn$GOMPSTR.pc.in
else
  GOMPSTR=""
  GOMPFLAGS=""
fi

if (uname) | grep -i darwin 2>/dev/null 1>/dev/null ; then
  SONAMEOPTS=""
else
  SONAMEOPTS="-Wl,-soname -Wl,libcomplearn${GOMPSTR}-1.0${shrext_cmds}.0"
fi
libcomplearn_major=`echo "$VERSION" | $AWK -F. '{print $1}'`
libcomplearn_minor=`echo "$VERSION" | $AWK -F. '{print $2}'`
libcomplearn_patch=[`echo "$VERSION" | $AWK '-F[.~]' '{print $3}'`]
libcomplearn_release="$libcomplearn_major.$libcomplearn_minor"
libcomplearn_version=`expr $libcomplearn_major + $libcomplearn_minor`":$libcomplearn_patch:$libcomplearn_minor"
AC_ARG_ENABLE(rpath-link,
[  --enable-rpath-link  Enable rpath-link to support custom library development
],
[case "${enableval}" in
   yes) ua_rpath=true ;;
   no) ua_rpath=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-rpath-link) ;;
esac], [ua_rpath=false])
AM_CONDITIONAL([WANT_RPATH], [test x$ua_rpath = xtrue])
AM_CONDITIONAL([FOUND_CHECK], [test x$HAVECHECK = x1])
AM_CONDITIONAL([FOUND_GOB2], [test x$HAVEGOB2 = x1])
AM_CONDITIONAL([FOUND_DOXYGEN], [test x$HAVEDOXYGEN = x1])
AM_CONDITIONAL([USING_GOMP], [test x$ua_openmp = xtrue])

SET_SONAME_VERSIONS()

AC_SUBST(PRIVATELIBS)
AC_SUBST(PRIVATECFLAGS)
AC_SUBST(SONAMEOPTS)
AC_SUBST(GOMPSTR)
AC_SUBST(GOMPFLAGS)
AC_SUBST(GOB2)
AC_SUBST(DOXYGEN)
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16.1])
AM_CONFIG_HEADER(src/privconf.h)

AC_OUTPUT([complearn.pc complearn-gomp.pc Makefile src/Makefile tests/Makefile src/complearn-config.c  po/Makefile.in Doxyfile])
