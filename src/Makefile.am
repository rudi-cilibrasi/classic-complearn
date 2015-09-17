
bin_PROGRAMS = ncd anycompress anydecompress complearn-config
ACLOCAL_AMFLAGS = -I m4

if USING_GOMP
EXELDFLAGS = -lcomplearn-gomp @PRIVATELIBS@
lib_LTLIBRARIES   = libcomplearn-gomp.la
else
EXELDFLAGS = -lcomplearn @PRIVATELIBS@
lib_LTLIBRARIES   = libcomplearn.la
endif

if WANT_RPATH
RPATHFLAGS= -rpath @abs_srcdir@/.libs
else
RPATHFLAGS=
endif

GLOBCPPFLAGS= -I@abs_srcdir@ -I@abs_srcdir@/complearn -g

complearn_config_SOURCES = complearn-config.c
complearn_config_LDADD = $(EXELDFLAGS)
complearn_config_LDFLAGS = $(RPATHFLAGS)
complearn_config_CPPFLAGS = $(GLOBCPPFLAGS)

ncd_SOURCES = ncd.c
ncd_LDADD = $(EXELDFLAGS)
ncd_LDFLAGS = $(RPATHFLAGS)
ncd_CPPFLAGS = $(GLOBCPPFLAGS)

anycompress_SOURCES = anycompress.c
anycompress_LDADD = $(EXELDFLAGS)
anycompress_LDFLAGS = $(RPATHFLAGS)
anycompress_CPPFLAGS = $(GLOBCPPFLAGS)

anydecompress_SOURCES = anydecompress.c
anydecompress_LDADD = $(EXELDFLAGS)
anydecompress_LDFLAGS = $(RPATHFLAGS)
anydecompress_CPPFLAGS = $(GLOBCPPFLAGS)

GOBCS = \
complearn-compressor-driver.c \
complearn-environment.c \
complearn-external-compressor.c \
complearn-external-file-compressor.c \
complearn-ncd.c \
complearn-rcbzip.c \
complearn-rcbzlib.c \
complearn-rcgzip.c \
complearn-rczlib.c \
complearn-real-compressor-adaptor.c \
complearn-vcblocksort.c

GOBS = \
complearn-compressor-driver.gob \
complearn-environment.gob \
complearn-external-compressor.gob \
complearn-external-file-compressor.gob \
complearn-ncd.gob \
complearn-rcbzip.gob \
complearn-rcbzlib.gob \
complearn-rcgzip.gob \
complearn-rczlib.gob \
complearn-real-compressor-adaptor.gob \
complearn-vcblocksort.gob

GOBSTAMPS = \
complearn-compressor-driver.stamp \
complearn-environment.stamp \
complearn-external-compressor.stamp \
complearn-external-file-compressor.stamp \
complearn-ncd.stamp \
complearn-rcbzip.stamp \
complearn-rcbzlib.stamp \
complearn-rcgzip.stamp \
complearn-rczlib.stamp \
complearn-real-compressor-adaptor.stamp \
complearn-vcblocksort.stamp

GOBHS = \
complearn/complearn-compressor-driver.h \
complearn/complearn-compressor-driver-private.h \
complearn/complearn-environment.h \
complearn/complearn-environment-private.h \
complearn/complearn-external-compressor.h \
complearn/complearn-external-compressor-private.h \
complearn/complearn-external-file-compressor.h \
complearn/complearn-external-file-compressor-private.h \
complearn/complearn-ncd.h \
complearn/complearn-ncd-private.h \
complearn/complearn-rcbzip.h \
complearn/complearn-rcbzip-private.h \
complearn/complearn-rcbzlib.h \
complearn/complearn-rcbzlib-private.h \
complearn/complearn-rcgzip.h \
complearn/complearn-rcgzip-private.h \
complearn/complearn-rczlib.h \
complearn/complearn-rczlib-private.h \
complearn/complearn-real-compressor-adaptor.h \
complearn/complearn-real-compressor-adaptor-private.h \
complearn/complearn-vcblocksort.h \
complearn/complearn-vcblocksort-private.h

BASESOURCEC = \
clconfig.c \
cloutput.c \
qsufsort.c \
clnexus.c \
real-compressor.c \
uncd.c \
util.c

BASEHEADERS = \
complearn/complearn.h \
complearn/cloutput.h \
complearn/newcomplearn.h \
complearn/clconfig.h \
complearn/real-compressor.h \
complearn/util.h

if USING_GOMP
libcomplearn_gomp_la_SOURCES = $(GOBCS) $(BASESOURCEC)
libcomplearn_gomp_la_LDFLAGS= -version-info @libcomplearn_version@ @SONAMEOPTS@ @PRIVATELIBS@
libcomplearn_gomp_la_CPPFLAGS = $(GLOBCPPFLAGS)
else
libcomplearn_la_SOURCES = $(GOBCS) $(BASESOURCEC)
libcomplearn_la_LDFLAGS= -version-info @libcomplearn_version@ @SONAMEOPTS@ @PRIVATELIBS@
libcomplearn_la_CPPFLAGS = $(GLOBCPPFLAGS)
endif

lib_includedir=$(includedir)/complearn-1.0/complearn
lib_nobase_includedir=$(includedir)/complearn-1.0
lib_include_HEADERS= $(BASEHEADERS) $(GOBHS)
lib_nobase_include_HEADERS=complearn.h

if FOUND_DOXYGEN
autodox: $(GOBCS) $(GOBHS)
	cd @abs_srcdir@/.. && $(DOXYGEN) Doxyfile
endif

if FOUND_GOB2
$(GOBHS): $(GOBSTAMPS)

$(GOBCS): $(GOBSTAMPS)

BUILT_SOURCES = $(GOBCS) $(GOBSTAMPS)

.gob.stamp:
	$(GOB2) --always-private-header --no-touch $< -o @abs_srcdir@/complearn ; mv -f @abs_srcdir@/complearn/*.c . ; touch $@

endif

maintainer-clean-local:
	$(RM) -r ../doc/autodox

MAINTAINERCLEANFILES = $(GOBCS) $(GOBHS) $(GOBSTAMPS)
EXTRA_DIST = $(GOBCS) $(GOBHS) $(GOBS)

GOBSOURCES = @GOBSOURCES@

