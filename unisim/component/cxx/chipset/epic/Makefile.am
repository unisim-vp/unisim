libdir = $(prefix)/lib/chipsets/mpc107/epic
includedir = $(prefix)/include/chipsets/mpc107/epic

include_HEADERS = epic.hh epic.tpp \
	register.hh inservice_reg.hh

INCLUDES = -I$(top_srcdir)/src

lib_LIBRARIES = libepic.a
libepic_a_SOURCES = epic.cpp epic_debug.cpp \
	register.cpp inservice_reg.cpp
