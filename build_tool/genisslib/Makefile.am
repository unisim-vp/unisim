BUILT_SOURCES = yy.h
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l -o$(LEX_OUTPUT_ROOT).c

bin_PROGRAMS = genisslib
include_HEADERS = genisslib.h
genisslib_SOURCES = genisslib.c yy.y scanner.l

ROOT_DIR = ..
include $(top_srcdir)/../m4/unisim.am
