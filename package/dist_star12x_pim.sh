#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> [ln|cp]"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

CPLN="cp -f"
if [ -z "$2" ]; then
	CPLN="cp -f"
elif test $2 = "ln"; then
	CPLN="ln -s"
elif test $2 = "cp"; then
	CPLN="cp -f"
else
	Usage
	exit -1
fi

DEST_DIR=$1
UNISIM_TOOLS_DIR=${MY_DIR}/../unisim_tools
UNISIM_LIB_DIR=${MY_DIR}/../unisim_lib
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/s12x

STAR12X_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-star12x-${STAR12X_VERSION}

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser_defs.hh \
riscgenerator.hh \
specialization.hh \
variable.hh \
bitfield.hh \
comment.hh \
fwd.hh \
main.hh \
product.hh \
scanner.hh \
strtools.hh \
vect.hh \
ciscgenerator.hh \
conststr.hh \
generator.hh \
operation.hh \
referencecounting.hh \
sourcecode.hh \
subdecoder.hh"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser_tokens.hh"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
parser.yy \
scanner.ll \
action.cc \
bitfield.cc \
cli.cc \
comment.cc \
conststr.cc \
isa.cc \
main.cc \
operation.cc \
product.cc \
referencecounting.cc \
sourcecode.cc \
strtools.cc \
variable.cc \
generator.cc \
riscgenerator.cc \
ciscgenerator.cc \
subdecoder.cc \
specialization.cc \
errtools.cc"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4"

GENISSLIB_EXTERNAL_HEADERS="\
cassert \
cctype \
cerrno \
cstdarg \
cstdio \
cstdlib \
cstring \
fstream \
inttypes.h \
iosfwd \
iostream \
limits \
map \
memory \
ostream \
unistd.h \
vector"

UNISIM_LIB_STAR12X_SOURCE_FILES="\
unisim/kernel/service/service.cc \
unisim/kernel/service/xml_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/kernel/api/api.cc \
unisim/kernel/debug/debug.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/data_object.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/debug/blob/blob32.cc \
unisim/util/debug/blob/blob64.cc \
unisim/util/debug/blob/section32.cc \
unisim/util/debug/blob/section64.cc \
unisim/util/debug/blob/segment32.cc \
unisim/util/debug/blob/segment64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/util/debug/coff_symtab/coff_symtab64.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/loader/coff_loader/coff_loader64.cc \
unisim/util/endian/endian.cc \
unisim/util/lexer/lexer.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/debug/debugger/debugger64.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/profiling/addr_profiler/profiler64.cc \
unisim/service/telnet/telnet.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/tee/registers/registers_tee.cc \
unisim/service/tee/memory_import_export/memory_import_export_tee.cc \
unisim/service/tee/memory_access_reporting/tee_64.cc \
unisim/service/tee/memory_access_reporting/tee_32.cc \
unisim/component/cxx/processor/hcs12x/xgate.cc \
unisim/component/cxx/processor/hcs12x/ccr.cc \
unisim/component/cxx/processor/hcs12x/cpu.cc \
unisim/component/cxx/processor/hcs12x/exception.cc \
unisim/component/cxx/processor/hcs12x/mmc.cc \
unisim/component/tlm2/processor/hcs12x/hcs12x.cc \
unisim/component/tlm2/processor/hcs12x/xint.cc \
unisim/component/tlm2/processor/hcs12x/s12xmmc.cc \
unisim/component/tlm2/processor/hcs12x/pwm.cc \
unisim/component/tlm2/processor/hcs12x/atd10b.cc \
unisim/component/tlm2/processor/hcs12x/crg.cc \
unisim/component/tlm2/processor/hcs12x/ect.cc \
unisim/component/tlm2/processor/hcs12x/s12xeetx.cc \
unisim/component/tlm2/processor/hcs12x/tlm_types.cc \
unisim/component/tlm2/processor/hcs12x/s12xgate.cc \
unisim/component/tlm2/processor/hcs12x/s12pit24b.cc \
unisim/component/tlm2/processor/hcs12x/s12sci.cc \
unisim/component/tlm2/processor/hcs12x/s12spi.cc \
unisim/component/tlm2/interconnect/generic_router/router.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/service/pim/pim.cc \
unisim/service/pim/pim_server_32.cc \
unisim/service/pim/pim_server_64.cc \
unisim/service/pim/pim_server.cc \
unisim/service/pim/pim_thread.cc \
unisim/service/pim/network/BlockingQueue.cpp \
unisim/service/pim/network/GenericThread.cpp \
unisim/service/pim/network/SocketClientThread.cpp \
unisim/service/pim/network/SocketServerThread.cpp \
unisim/service/pim/network/SocketThread.cpp"


UNISIM_LIB_STAR12X_ISA_FILES="\
unisim/component/cxx/processor/hcs12x/addition_substraction.isa \
unisim/component/cxx/processor/hcs12x/bit.isa \
unisim/component/cxx/processor/hcs12x/boolean-logic.isa \
unisim/component/cxx/processor/hcs12x/branch.isa \
unisim/component/cxx/processor/hcs12x/compare_test.isa \
unisim/component/cxx/processor/hcs12x/decrement_increment.isa \
unisim/component/cxx/processor/hcs12x/fuzzy-logic.isa \
unisim/component/cxx/processor/hcs12x/hcs12x.isa \
unisim/component/cxx/processor/hcs12x/load_store.isa \
unisim/component/cxx/processor/hcs12x/move.isa \
unisim/component/cxx/processor/hcs12x/multiplication_division.isa \
unisim/component/cxx/processor/hcs12x/others.isa \
unisim/component/cxx/processor/hcs12x/stacking.isa \
unisim/component/cxx/processor/hcs12x/transfer_exchange.isa"

UNISIM_LIB_XB_ISA_FILES="\
unisim/component/cxx/processor/hcs12x/xb.isa"

UNISIM_LIB_S12XGATE_ISA_FILES="\
unisim/component/cxx/processor/hcs12x/s12xgate.isa \
"

UNISIM_LIB_STAR12X_HEADER_FILES="${UNISIM_LIB_STAR12X_ISA_FILES} ${UNISIM_LIB_XB_ISA_FILES} ${UNISIM_LIB_S12XGATE_ISA_FILES} \
unisim/kernel/service/service.hh \
unisim/kernel/service/xml_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/api/api.hh \
unisim/kernel/debug/debug.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/cfa.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/event.hh \
unisim/util/endian/endian.hh \
unisim/util/debug/blob/blob.hh \
unisim/util/debug/blob/section.hh \
unisim/util/debug/blob/segment.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/singleton/singleton.hh \
unisim/util/xml/xml.hh \
unisim/util/likely/likely.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/char_io.hh \
unisim/service/interfaces/debug_control.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/service/tee/registers/registers_tee.hh \
unisim/service/tee/memory_import_export/memory_import_export_tee.hh \
unisim/service/tee/memory_access_reporting/tee.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
unisim/service/interfaces/profiling.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/telnet/telnet.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/hcs12x/cpu.hh \
unisim/component/cxx/processor/hcs12x/mmc.hh \
unisim/component/cxx/processor/hcs12x/ccr.hh \
unisim/component/cxx/processor/hcs12x/exception.hh \
unisim/component/cxx/processor/hcs12x/types.hh \
unisim/component/cxx/processor/hcs12x/concatenated_register.hh \
unisim/component/cxx/processor/hcs12x/config.hh \
unisim/component/cxx/processor/hcs12x/xgate.hh \
unisim/component/tlm2/processor/hcs12x/s12xgate.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/processor/hcs12x/hcs12x.hh \
unisim/component/tlm2/processor/hcs12x/xint.hh \
unisim/component/tlm2/processor/hcs12x/tlm_types.hh \
unisim/component/tlm2/processor/hcs12x/s12xmmc.hh \
unisim/component/tlm2/processor/hcs12x/pwm.hh \
unisim/component/tlm2/processor/hcs12x/atd10b.hh \
unisim/component/tlm2/processor/hcs12x/crg.hh \
unisim/component/tlm2/processor/hcs12x/ect.hh \
unisim/component/tlm2/processor/hcs12x/s12xeetx.hh \
unisim/component/tlm2/processor/hcs12x/s12pit24b.hh \
unisim/component/tlm2/processor/hcs12x/s12sci.hh \
unisim/component/cxx/processor/hcs12x/s12mpu_if.hh \
unisim/component/tlm2/processor/hcs12x/s12spi.hh \
unisim/service/pim/pim.hh \
unisim/service/pim/pim_server.hh \
unisim/service/pim/pim_thread.hh \
unisim/util/converter/convert.hh \
unisim/service/pim/network/BlockingQueue.hpp \
unisim/service/pim/network/BlockingQueue.tcc \
unisim/service/pim/network/GenericThread.hpp \
unisim/service/pim/network/SocketClientThread.hpp \
unisim/service/pim/network/SocketServerThread.hpp \
unisim/service/pim/network/SocketThread.hpp"

UNISIM_LIB_STAR12X_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/debug/data_object.tcc \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/blob/section.tcc \
unisim/util/debug/blob/blob.tcc \
unisim/util/debug/blob/segment.tcc \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/debug/coff_symtab/coff_symtab.tcc \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/loader/coff_loader/ti/ti.tcc \
unisim/util/dictionary/dictionary.tcc \
unisim/util/lexer/lexer.tcc \
unisim/util/parser/parser.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/tee/memory_access_reporting/tee.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/processor/hcs12x/pwm.tcc \
unisim/component/tlm2/processor/hcs12x/atd10b.tcc \
unisim/component/tlm2/processor/hcs12x/s12xeetx.tcc \
unisim/component/tlm2/processor/hcs12x/s12pit24b.tcc \
unisim/service/pim/pim_server.tcc "

UNISIM_LIB_STAR12X_M4_FILES="\
m4/cxxabi.m4 \
m4/endian.m4 \
m4/times.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/tlm20.m4 \
m4/with_boost.m4 \
m4/check_lib.m4 \
m4/rtbcob.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_STAR12X_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_hcs12x.xml \
unisim/util/debug/dwarf/68hc12_dwarf_register_number_mapping.xml \
"

STAR12X_EXTERNAL_HEADERS="\
assert.h \
errno.h \
fcntl.h \
fstream \
getopt.h \
inttypes.h \
iosfwd \
iostream \
list \
map \
ostream \
queue \
signal.h \
sstream \
stdarg.h \
stdexcept \
stdio.h \
stdlib.h \
string \
string.h \
sys/stat.h \
sys/types.h \
unistd.h \
vector \
"

UNISIM_SIMULATORS_STAR12X_SOURCE_FILES="\
main_pim.cc \
simulator.cc \
atd_pwm_stub.cc \
xml_atd_pwm_stub.cc \
rtb_unisim.cc \
"
UNISIM_SIMULATORS_STAR12X_HEADER_FILES="\
simulator.hh \
atd_pwm_stub.hh \
xml_atd_pwm_stub.hh \
rtb_unisim.hh \
"

UNISIM_SIMULATORS_STAR12X_TEMPLATE_FILES=

UNISIM_SIMULATORS_STAR12X_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico \
"

UNISIM_SIMULATORS_STAR12X_CONFIG_FILES="\
default_config.xml \
ATD.xml \
ATD.xls \
pwm.xls \
pim.xml \
"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_star12x_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/star12x

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/genisslib/${file}" -nt "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/genisslib/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${CPLN} "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_STAR12X_FILES="${UNISIM_LIB_STAR12X_SOURCE_FILES} ${UNISIM_LIB_STAR12X_HEADER_FILES} ${UNISIM_LIB_STAR12X_TEMPLATE_FILES} ${UNISIM_LIB_STAR12X_DATA_FILES}"

for file in ${UNISIM_LIB_STAR12X_FILES}; do
	mkdir -p "${DEST_DIR}/star12x/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/star12x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/star12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/star12x/${file}"
		${CPLN} "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/star12x/${file}" || exit
	fi
done

UNISIM_SIMULATORS_STAR12X_FILES="${UNISIM_SIMULATORS_STAR12X_SOURCE_FILES} ${UNISIM_SIMULATORS_STAR12X_HEADER_FILES} ${UNISIM_SIMULATORS_STAR12X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_STAR12X_DATA_FILES} ${UNISIM_SIMULATORS_STAR12X_CONFIG_FILES}"

for file in ${UNISIM_SIMULATORS_STAR12X_FILES}; do
	mkdir -p "${DEST_DIR}/star12x/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/star12x/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/star12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/star12x/${file}"
		${CPLN} "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/star12x/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_STAR12X_DATA_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/${file}"
		${CPLN} "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/star12x/config
mkdir -p ${DEST_DIR}/star12x/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/${file}" -nt  "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/${file} ==> ${DEST_DIR}/genisslib/${file}"
		${CPLN} "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_STAR12X_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/star12x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/star12x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/star12x/${file}"
		${CPLN} "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/star12x/${file}" || exit
		has_to_build_star12x_configure=yes
	fi
done

# Top level

echo "This package contains GenISSLib, an instruction set simulator generator, and a star12x instruction set simulator." > "${DEST_DIR}/README"
echo "See INSTALL for installation instructions." >> "${DEST_DIR}/README"

echo "INSTALLATION" > "${DEST_DIR}/INSTALL"
echo "------------" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Requirements:" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bash (bash-3.0-19.3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU make (make-3.80-6.EL4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU autoconf (autoconf-2.59-5 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU automake (automake-1.9.2-3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU flex (flex-2.5.4a-33 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bison (bison-1.875c-2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - boost development package (boost-devel-1.32.0-1.rhel4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - libxml2 development package (libxml2-devel-2.6.16-6 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - zlib development package (zlib-devel-1.2.1.2-1.2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Building instructions:" >> "${DEST_DIR}/INSTALL"
echo "  $ ./configure" >> "${DEST_DIR}/INSTALL"
echo "  $ make" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Installing (optional):" >> "${DEST_DIR}/INSTALL"
echo "  $ make install" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${CONFIGURE_AC}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${MAKEFILE_AM}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM star12x Standalone simulator], [${STAR12X_VERSION}], [Reda Nouacer <reda.nouacer@cea.fr>], [unisim-star12x])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([star12x])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib star12x" > "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"


if [ ! -e "${GENISSLIB_CONFIGURE_AC}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_CONFIGURE_AC}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_MAKEFILE_AM}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LEXER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PARSER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${GENISSLIB_CONFIGURE_AC}"

	AM_GENISSLIB_VERSION=`printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g'`
	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES} parser.h" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"

# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
   	echo "\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc" >> "${GENISSLIB_MAKEFILE_AM}"
    	printf "\tif test -f \"\$(top_builddir)/parser.h\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    	printf "\t\tcp -f \"\$(top_builddir)/parser.h\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    	printf "\telif test -f \"\$(top_builddir)/parser.hh\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    	printf "\t\tcp -f \"\$(top_builddir)/parser.hh\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    	printf "\tfi\n" >> "${GENISSLIB_MAKEFILE_AM}"

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# star12x

STAR12X_CONFIGURE_AC="${DEST_DIR}/star12x/configure.ac"
STAR12X_MAKEFILE_AM="${DEST_DIR}/star12x/Makefile.am"


if [ ! -e "${STAR12X_CONFIGURE_AC}" ]; then
	has_to_build_star12x_configure=yes
else
	if [ "$0" -nt "${STAR12X_CONFIGURE_AC}" ]; then
		has_to_build_star12x_configure=yes
	fi
fi

if [ ! -e "${STAR12X_MAKEFILE_AM}" ]; then
	has_to_build_star12x_configure=yes
else
	if [ "$0" -nt "${STAR12X_MAKEFILE_AM}" ]; then
		has_to_build_star12x_configure=yes
	fi
fi

if [ "${has_to_build_star12x_configure}" = "yes" ]; then
	echo "Generating star12x configure.ac"
	echo "AC_INIT([UNISIM star12x C++ simulator], [${STAR12X_VERSION}], [Reda Nouacer <reda.nouacer@cea.fr>], [star12x_cxx])" > "${STAR12X_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${STAR12X_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${STAR12X_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PTHREAD(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_RTBCOB(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${STAR12X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TLM20" >> "${STAR12X_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-star12x-${STAR12X_VERSION}\"], [path of shared data relative to bin directory])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${STAR12X_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${STAR12X_CONFIGURE_AC}"

	AM_STAR12X_VERSION=`printf ${STAR12X_VERSION} | sed -e 's/\./_/g'`
	echo "Generating star12x Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(top_srcdir)/m4" > "${STAR12X_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${STAR12X_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libstar12x-${STAR12X_VERSION}.a" >> "${STAR12X_MAKEFILE_AM}"
	echo "nodist_libstar12x_${AM_STAR12X_VERSION}_a_SOURCES = unisim/component/cxx/processor/hcs12x/xb.cc unisim/component/cxx/processor/hcs12x/hcs12x.cc unisim/component/cxx/processor/hcs12x/s12xgate.cc" >> "${STAR12X_MAKEFILE_AM}"
	echo "libstar12x_${AM_STAR12X_VERSION}_a_SOURCES = ${UNISIM_LIB_STAR12X_SOURCE_FILES}" >> "${STAR12X_MAKEFILE_AM}" # unisim/component/cxx/processor/hcs12x/xb.cc unisim/component/cxx/processor/hcs12x/hcs12x.cc" >> "${STAR12X_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-star12x-${STAR12X_VERSION}" >> "${STAR12X_MAKEFILE_AM}"
	echo "unisim_star12x_${AM_STAR12X_VERSION}_SOURCES = ${UNISIM_SIMULATORS_STAR12X_SOURCE_FILES}" >> "${STAR12X_MAKEFILE_AM}"
	echo "unisim_star12x_${AM_STAR12X_VERSION}_LDADD = libstar12x-${STAR12X_VERSION}.a" >> "${STAR12X_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_STAR12X_HEADER_FILES} ${UNISIM_LIB_STAR12X_HEADER_FILES} ${UNISIM_LIB_STAR12X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_STAR12X_HEADER_FILES} ${UNISIM_SIMULATORS_STAR12X_TEMPLATE_FILES}" >> "${STAR12X_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_STAR12X_M4_FILES}" >> "${STAR12X_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-star12x-${STAR12X_VERSION}" >> "${STAR12X_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_STAR12X_DATA_FILES} ${UNISIM_SIMULATORS_STAR12X_DATA_FILES} ${UNISIM_SIMULATORS_STAR12X_CONFIG_FILES}" >> "${STAR12X_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${STAR12X_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.cc: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb.hh: \${UNISIM_LIB_XB_ISA_FILES}" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t" >> "${STAR12X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/hcs12x; \$(GENISSLIB_PATH) -o xb -w 32 -I . xb.isa" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.cc: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/hcs12x.hh: ${UNISIM_LIB_STAR12X_ISA_FILES} \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/xb_sub.isa" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t" >> "${STAR12X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/hcs12x; \$(GENISSLIB_PATH) -o hcs12x -w 32 -I . hcs12x.isa" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.cc: \$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh" >> "${STAR12X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/hcs12x/s12xgate.hh: \${UNISIM_LIB_S12XGATE_ISA_FILES}" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t" >> "${STAR12X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/hcs12x; \$(GENISSLIB_PATH) -o s12xgate -w 32 -I . s12xgate.isa" >> "${STAR12X_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${STAR12X_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${STAR12X_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\`basename \$\${PROGRAM}\`\"; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t(cd '\$(top_builddir)/bin' && cp -f \"\$(abs_top_builddir)/\$\${PROGRAM}\" \`basename \"\$\${PROGRAM}\"\`); \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tdone\n" >> "${STAR12X_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tfi\n" >> "${STAR12X_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-star12x-${STAR12X_VERSION}/\`basename \$\${SHARED_DATA}\`\"; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-star12x-${STAR12X_VERSION}'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t(cd '\$(top_builddir)/share/unisim-star12x-${STAR12X_VERSION}' && cp -f \"\$(abs_top_builddir)/\$\${SHARED_DATA}\" \`basename \"\$\${SHARED_DATA}\"\`); \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tdone\n" >> "${STAR12X_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${STAR12X_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${STAR12X_MAKEFILE_AM}"
	printf "\tfi\n" >> "${STAR12X_MAKEFILE_AM}"

	echo "Building star12x configure"
	${SHELL} -c "cd ${DEST_DIR}/star12x && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
