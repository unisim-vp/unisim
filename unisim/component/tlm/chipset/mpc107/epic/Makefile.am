includedir = $(prefix)/include/tlm/chipsets/mpc107/epic
include_HEADERS = epic.hh epic.tpp timer.hh


INCLUDES = -I$(top_srcdir)/src
libdir = $(prefix)/lib/tlm/chipsets/mpc107/epic

lib_LIBRARIES = libepic.a
libepic_a_SOURCES = epic.cpp epic_debug.cpp timer.cpp
libepic_a_CXXFLAGS = -finline-functions -DSC_INCLUDE_DYNAMIC_PROCESSES
