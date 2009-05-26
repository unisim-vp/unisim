sharedir = $(prefix)/share/unisim_lib
SUBDIRS = unisim
INCLUDE_SUBDIRS = unisim

dist_share_DATA = COPYING

EXTRA_DIST = \
m4/acinclude.m4 \
m4/aminclude.am \
m4/boost_array.m4 \
m4/boost_bind.m4 \
m4/boost_function.m4 \
m4/boost_graph.m4 \
m4/boost_integer.m4 \
m4/boost_mem_fn.m4 \
m4/boost_thread.m4 \
m4/bsd_sockets.m4 \
m4/cacti.m4 \
m4/curses.m4 \
m4/cxxabi.m4 \
m4/libedit.m4 \
m4/libxml2.m4 \
m4/pthread.m4 \
m4/sdl.m4 \
m4/systemc.m4 \
m4/termcap.m4 \
m4/tlm20.m4 \
m4/unisim.am \
m4/unisim.m4 \
m4/with_boost.m4 \
m4/zlib.m4

uninstall-hook:
	(rmdir $(sharedir) || true)

include m4/aminclude.am
