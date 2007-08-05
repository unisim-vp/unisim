libdir = $(prefix)/lib/chipsets/mpc107/dma
includedir = $(prefix)/include/chipsets/mpc107/dma

include_HEADERS = dma.hh dma_client_interface.hh \
	buffer.hh register.hh \
	dma.tpp buffer.tpp

INCLUDES = -I$(top_srcdir)/src

lib_LIBRARIES = libmpc107_dma.a
libmpc107_dma_a_SOURCES = dma.cpp dma_debug.cpp \
	buffer.cpp \
	register.cpp
