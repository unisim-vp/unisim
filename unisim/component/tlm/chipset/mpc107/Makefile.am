DIST_SUBDIRS = epic
SUBDIRS = epic

includedir = $(prefix)/include/tlm/chipsets/mpc107
include_HEADERS = mpc107.hh mpc107.tpp


INCLUDES = -I$(top_srcdir)/src
libdir = $(prefix)/lib/tlm/chipsets/mpc107

lib_LIBRARIES = libmpc107.a
libmpc107_a_SOURCES = mpc107.cpp mpc107_debug.cpp
libmpc107_a_CXXFLAGS = -finline-functions -DSC_INCLUDE_DYNAMIC_PROCESSES
