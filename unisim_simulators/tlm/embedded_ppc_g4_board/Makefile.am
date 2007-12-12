sharedir = $(prefix)/share/unisim_simulators/tlm/embedded_ppc_g4_board
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = embedded_ppc_g4_board embedded_ppc_g4_board-debug
embedded_ppc_g4_board_SOURCES = main.cc
embedded_ppc_g4_board_debug_SOURCES = main.cc
embedded_ppc_g4_board_debug_CPPFLAGS = -DEMBEDDED_PPC_G4_BOARD_DEBUG

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
