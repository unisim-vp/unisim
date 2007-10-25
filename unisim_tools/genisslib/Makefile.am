BUILT_SOURCES = scanner.cc parser.cc parser.h
CLEANFILES = scanner.cc parser.cc parser.h
AM_YFLAGS = -d -p yy
AM_LFLAGS = -l

noinst_HEADERS = \
	action.hh \
	conststr.hh \
	operation.hh \
	scanner.hh \
	vect.hh \
	bitfield.hh \
	fwd.hh \
	parser.hh \
	sourcecode.hh \
	yystype.hh \
	cli.hh \
	isa.hh \
	product.hh \
	strtools.hh \
	comment.hh \
	main.hh \
	referencecounting.hh \
	variable.hh

bin_PROGRAMS = genisslib
genisslib_SOURCES = parser.yy scanner.ll action.cc bitfield.cc cli.cc \
		comment.cc conststr.cc isa.cc main.cc operation.cc product.cc \
		referencecounting.cc sourcecode.cc strtools.cc variable.cc

ROOT_DIR = ..
include $(top_srcdir)/../m4/unisim.am
