#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> <unisim_lib directory> <unisim_simulators directory>"
}

if test "x$1" = x || test "x$2" = x || test "x$3" = x; then
	Usage
	exit
fi

HERE=`pwd`
DEST_DIR=$1
UNISIM_LIB_DIR=$2
UNISIM_SIMULATORS_DIR=$3/cxx/tms320c3x

UNISIM_LIB_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/debug/debug.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/endian/endian.cc \
unisim/service/loader/coff_loader/coff_loader.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/os/ti_c_io/ti_c_io_32.cc \
unisim/service/os/ti_c_io/ti_c_io_64.cc \
unisim/service/time/host_time/time.cc \
unisim/component/cxx/processor/tms320/cpu.cc \
unisim/component/cxx/processor/tms320/cpu_tms320vc33.cc \
unisim/component/cxx/processor/tms320/cpu_tms320vc33_debug.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc"

UNISIM_LIB_HEADER_FILES="\
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/debug/debug.hh \
unisim/util/xml/xml.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/endian/endian.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/hash_table/hash_table.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/coff_loader/ti/ti.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/os/ti_c_io/ti_c_io.hh \
unisim/service/time/host_time/time.hh \
unisim/component/cxx/processor/tms320/config.hh \
unisim/component/cxx/processor/tms320/cpu.hh \
unisim/component/cxx/processor/tms320/isa_tms320.hh \
unisim/component/cxx/processor/tms320/exception.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/tms320/isa/2op.isa \
unisim/component/cxx/processor/tms320/isa/3op.isa \
unisim/component/cxx/processor/tms320/isa/actions_dec.isa \
unisim/component/cxx/processor/tms320/isa/control.isa \
unisim/component/cxx/processor/tms320/isa/interlock.isa \
unisim/component/cxx/processor/tms320/isa/load_store.isa \
unisim/component/cxx/processor/tms320/isa/parallel.isa \
unisim/component/cxx/processor/tms320/isa/power.isa \
unisim/component/cxx/processor/tms320/isa/tms320.isa"


UNISIM_LIB_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/coff_loader/ti/ti.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/os/ti_c_io/ti_c_io.tcc \
unisim/component/cxx/processor/tms320/cpu.tcc \
unisim/component/cxx/processor/tms320/exception.tcc \
unisim/component/cxx/processor/tms320/isa_tms320.tcc \
unisim/component/cxx/memory/ram/memory.tcc"

UNISIM_LIB_M4_FILES="\
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4"

UNISIM_SIMULATORS_SOURCE_FILES=main.cc
UNISIM_SIMULATORS_HEADER_FILES=
UNISIM_SIMULATORS_TEMPLATE_FILES=
UNISIM_SIMULATORS_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

has_to_build_configure=no

UNISIM_LIB_FILES="${UNISIM_LIB_SOURCE_FILES} ${UNISIM_LIB_HEADER_FILES} ${UNISIM_LIB_TEMPLATE_FILES}"

for file in ${UNISIM_LIB_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "Copying ${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/${file}"
	fi
done

UNISIM_SIMULATORS_FILES="${UNISIM_SIMULATORS_SOURCE_FILES} ${UNISIM_SIMULATORS_HEADER_FILES} ${UNISIM_SIMULATORS_TEMPLATE_FILES} ${UNISIM_SIMULATORS_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "Copying ${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}"
		has_to_build_configure=yes
	fi
done

mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/m4

for file in ${UNISIM_LIB_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/m4/`basename ${file}`" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/m4/`basename ${file}`" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "Copying `basename ${file}`"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/m4/`basename ${file}`"
	fi
done

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM TMS320C3X C++ simulator], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [tms320c3x])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${MAKEFILE_AM}"
	echo "INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${MAKEFILE_AM}"
	echo "bin_PROGRAMS = tms320c3x" >> "${MAKEFILE_AM}"
	echo "tms320c3x_SOURCES = ${UNISIM_LIB_SOURCE_FILES} ${UNISIM_SIMULATORS_SOURCE_FILES}" >> "${MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_LIB_HEADER_FILES} ${UNISIM_LIB_TEMPLATE_FILES} ${UNISIM_SIMULATORS_HEADER_FILES} ${UNISIM_SIMULATORS_TEMPLATE_FILES}" >> "${MAKEFILE_AM}"
	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal -I m4 && autoconf --force && autoheader && automake -a"
fi

echo "Distribution is up-to-date"
