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
	src/memcache.c \
	src/utils.c


TESTS = simple
check_PROGRAMS = simple
simple_SOURCES = test/first-test.c
simple_CFLAGS = '-I$(top_srcdir)/include' -DLOG_LEVEL=4
simple_LDADD = $(top_srcdir)/libmint.la
