BUILT_SOURCES = parser.h
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l

bin_PROGRAMS = genisslib
genisslib_SOURCES = parser.yy scanner.ll action.cc bitfield.cc cli.cc \
		comment.cc conststr.cc isa.cc main.cc operation.cc product.cc \
		referencecounting.cc sourcecode.cc strtools.cc variable.cc

ROOT_DIR = ..
include $(top_srcdir)/../m4/unisim.am
