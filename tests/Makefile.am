
if FOUND_CHECK
TESTS = check_complearn

check_PROGRAMS = check_complearn
check_complearn_SOURCES = check_complearn.c check_ncd.c check_environment.c check_util.c check_cloutput.c suite.h
check_complearn_CFLAGS = @CHECK_CFLAGS@ -I../src
check_complearn_LDADD = @CHECK_LIBS@ -lcomplearn -L../src
endif
