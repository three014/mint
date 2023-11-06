lib_LTLIBRARIES = libmint.la

ACLOCAL_AMFLAGS = '-Im4'
libmint_la_CFLAGS = '-I$(top_srcdir)/include' '-lpthread'

libmint_la_SOURCES = \
	src/mint.c \
	src/runtime.c \
	src/queue.c \
	src/coroutine.c \
	src/context.S \
	src/memcache.c
