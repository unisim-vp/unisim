sharedir = $(prefix)/share/unisim_simulators/tlm2/s12x
INCLUDES=-I$(top_srcdir)/../../.. -I$(top_builddir)/../../..
bin_PROGRAMS = s12xemu
s12xemu_SOURCES = main.cc atd_pwm_stub.cc xml_atd_pwm_stub.cc
include_HEADERS = atd_pwm_stub.hh xml_atd_pwm_stub.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(sharedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
