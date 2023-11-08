lib_LTLIBRARIES = libmint.la

ACLOCAL_AMFLAGS = '-Im4'
libmint_la_CFLAGS = '-I$(top_srcdir)/include' -DLOG_LEVEL=4 -ggdb
libmint_la_LDFLAGS = -lpthread

libmint_la_SOURCES = \
	src/mint.c \
	src/runtime.c \
	src/queue.c \
	src/coroutine.c \
	src/context.S \
	src/memcache.c


TESTS = simple pthread_check
check_PROGRAMS = simple pthread_check

pthread_check_SOURCES = test/pthread-test.c
pthread_check_CFLAGS = '-I$(top_srcdir)/include' -DLOG_LEVEL=4 -ggdb
pthread_check_LDFLAGS = -lpthread

simple_SOURCES = test/first-test.c
simple_CFLAGS = '-I$(top_srcdir)/include' -DLOG_LEVEL=4 -ggdb
simple_LDADD = $(top_srcdir)/build/libmint.la
