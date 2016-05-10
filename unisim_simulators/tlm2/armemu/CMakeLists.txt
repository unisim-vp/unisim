cmake_minimum_required ( VERSION 2.8 )
project ( unisim_armemu )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
# include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( FindPythonInterp )
include ( FindPackageHandleStandardArgs )
message ( "The following python executable `${PYTHON_EXECUTABLE}' will be used
to create a python module of the UNISIM armemu simulator. If you want to use
another python version please launch cmake with the following option:
-DPYTHON_EXECUTABLE=<path to the python interpreter>" )

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 6 )
set ( SIM_VERSION_PATCH 1 )
set ( SIM_VERSION
  "${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH}" )
set ( SIM_SOVERSION ${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR} )
set ( SIM_VERSION_CODENAME "Triumphalis Tarraco" )
set ( SIM_AUTHOR "Daniel Gracia Perez (daniel.gracia-perez@cea.fr)" )
set ( SIM_PROGRAM_NAME "UNISIM ARMEmu" )
set ( SIM_COPYRIGHT "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique" )
set ( SIM_LICENSE "BSD (See file COPYING)" )
set ( SIM_DESCRIPTION "UNISIM ARMv5 User Level Simulator" )
set ( SIM_PYTHON_MODULE_NAME "armemu" )
set ( SIM_PYTHON_IMPORT_NAME "armemu${SIM_VERSION_MAJOR}${SIM_VERSION_MINOR}${SIM_VERSION_PATCH}")

# all the libraries need to be compiled with -fPIC otherwise dynamic linking
#   might not work properly
get_property ( FLAGS
  VARIABLE
  PROPERTY CMAKE_CXX_FLAGS )
string ( REGEX MATCH "-fPIC"
  FPIC_MATCH "${FLAGS}" )
if ( FPIC_MATCH )
else ( FPIC_MATCH )
  set ( CMAKE_CXX_FLAGS "${FLAGS} -fPIC" CACHE STRING "CMAKE_CXX_FLAGS" )
endif ( FPIC_MATCH )
get_property ( FLAGS
  VARIABLE
  PROPERTY CMAKE_C_FLAGS )
string ( REGEX MATCH "-fPIC"
  FPIC_MATCH "${FLAGS}" )
if ( FPIC_MATCH )
else ( FPIC_MATCH )
  set ( CMAKE_C_FLAGS "${FLAGS} -fPIC" CACHE STRING "CMAKE_C_FLAGS" )
endif ( FPIC_MATCH )

if ( UNIX AND NOT APPLE )
  find_file ( LSB_RELEASE_FILE lsb-release /etc )
  if ( LSB_RELEASE_FILE )
    file ( STRINGS /etc/lsb-release DISTRIB_ID REGEX "DISTRIB_ID=(.*)$" )
    if ( DISTRIB_ID )
      string ( REGEX REPLACE "DISTRIB_ID=" "" DISTRIB_ID ${DISTRIB_ID} )
    endif ( DISTRIB_ID )
    file ( STRINGS /etc/lsb-release DISTRIB_RELEASE REGEX "DISTRIB_RELEASE=(.*)$" )
    if ( DISTRIB_RELEASE )
      string ( REGEX REPLACE "DISTRIB_RELEASE=" "" DISTRIB_RELEASE ${DISTRIB_RELEASE} )
    endif ( DISTRIB_RELEASE )
  endif ( LSB_RELEASE_FILE )
  message ( "**** Release = ${DISTRIB_ID}${DISTRIB_RELEASE}" )
endif ( UNIX AND NOT APPLE )

option (
  BUILD_SIM_PIM_SUPPORT
  "Set this option if you want to compile the simulator for use with unisim-workbench."
  )
option (
  BUILD_ARMEMU_PYTHON_MODULE
  "Set this option if you want to compile a module of the unisim armemu simulator. Note that it has been developed under python3.1."
  )
option (
  BUILD_ARMEMU_POWER_SUPPORT
  "Set this option if you want to compile the unisim armemu simulator with power measurements support. Note that the CACTI_4_2_INCLUDE_DIR and CACTI_4_2_LIBRARY cmake variables might be required."
  )

# CACTI 4.2 configuration

set ( with_cacti_4_2 "" CACHE FILEPATH "Path to Cacti 4.2 installation." )

if ( with_cacti_4_2 )
  SET ( CACTI_4_2_INCLUDE_HINT ${with_cacti_4_2}/include )
  SET ( CACTI_4_2_LIB_HINT ${with_cacti_4_2}/lib )
else ( with_cacti_4_2 )
  SET ( CACTI_4_2_INCLUDE_HINT /usr/include )
  SET ( CACTI_4_2_LIB_HINT /usr/lib )
endif ( with_cacti_4_2 )

find_path ( CACTI_4_2_INCLUDE_DIR
    cacti4_2.hh
  HINTS
   ${CACTI_4_2_INCLUDE_HINT}
  )

find_library ( CACTI_4_2_LIBRARY
  NAMES
    cacti4_2
  HINTS
   ${CACTI_4_2_LIB_HINT}
  )

find_package_handle_standard_args (
  cacti4_2 
  DEFAULT_MSG 
  CACTI_4_2_LIBRARY 
  CACTI_4_2_INCLUDE_DIR )
mark_as_advanced ( CACTI_4_2_LIBRARY CACTI_4_2_INCLUDE_DIR )
if ( NOT CACTI4_2_FOUND )
  if ( with_cacti_4_2 )
    message ( FATAL_ERROR
      "Could not find the cacti 4.2 library at the defined path"
      " (${with_cacti_4_2}). Please check that the cacti library was"
      " correctly installed.")
  else ( with_cacti_4_2 )
    message (
      "Compiling the unisim::service::power service without cacti"
      " support. To provide full power/consumption support,"
      " install cacti 4.2 and indicate the"
      " path with '-Dwith_cacti_4_2=<path to the cacti library>'.")
    set ( UNISIM__SERVICE__POWER__SUPPORT FALSE PARENT_SCOPE) 
  endif ( with_cacti_4_2 )
else ( NOT CACTI4_2_FOUND )
  set ( UNISIM__SERVICE__POWER__SUPPORT TRUE PARENT_SCOPE)
endif ( NOT CACTI4_2_FOUND )

if ( CACTI4_2_FOUND )
  set ( HAVE_CACTI4_2 TRUE )
  include_directories ( ${CACTI_4_2_INCLUDE_DIR} )
else ( CACTI4_2_FOUND )
  set ( HAVE_CACTI4_2 FALSE )
endif ( CACTI4_2_FOUND )

# END CACTI 4.2 configuration

set ( ARMEMU_EXEC_NAME unisim-armemu-${SIM_VERSION} )
set ( ARMEMU_LIB_NAME unisim-armemu )
set ( ARMEMU_EXEC_TARGET ${ARMEMU_EXEC_NAME}-exe )
set ( ARMEMU_LIB_TARGET ${ARMEMU_LIB_NAME}-lib )
set ( RELATIVE_SHARE_DIRECTORY "share/${ARMEMU_EXEC_NAME}" )
set ( SHARE_DIRECTORY "${CMAKE_BINARY_DIR}/${RELATIVE_SHARE_DIRECTORY}" )
set_property (
  GLOBAL
  PROPERTY
  UNISIM_KERNEL_BIN_TO_SHARED_DATA_PATH "../share/${ARMEMU_EXEC_NAME}" )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS
  *.cc *.tcc *.hh
  CACHE
  STRING "Extensions of the source files"
  )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/bin
  )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}-${SIM_VERSION_MAJOR}.${SIM_VERSION_MINOR}.${SIM_VERSION_PATCH}
  )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/lib
  )

set ( CXX_INCLUDES
  cassert cctype cerrno cstdarg cstdio cstdlib cstring
  fstream iosfwd iostream limits list map memory ostream queue sstream stdexcept
  string typeinfo vector )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
  check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
  if ( NOT HAVE_${CXX_INCLUDE} )
    message ( SEND_ERROR
      "Could not find C++ header \"${CXX_INCLUDE}\"."
      )
  endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES
  errno fcntl getopt inttypes pthread signal stdint stdlib string sys/types
  sys/stat time unistd )
if ( WIN32 )
	list ( APPEND C_INCLUDES windows process )
  if ( BUILD_SIM_PIM_SUPPORT )
    list ( APPEND C_INCLUDES winsock2 )
  endif ( BUILD_SIM_PIM_SUPPORT )
else ( WIN32 )
	list ( APPEND C_INCLUDES sys/times )
  if ( BUILD_SIM_PIM_SUPPORT )
    list ( APPEND C_INCLUDES sys/socket netinet/in netinet/tcp netdb sys/times )
  endif ( BUILD_SIM_PIM_SUPPORT )
endif ( WIN32 )
foreach ( C_INCLUDE ${C_INCLUDES} )
  check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
  if ( NOT HAVE_${C_INCLUDE}_H )
    message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
  endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( UNISIM_LIB_PATH ${CMAKE_SOURCE_DIR} )

################################################################################
################################################################################
# Unisim util configuration and sources                                  START #

set ( UTIL_ENDIAN_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/endian )
set ( UTIL_ENDIAN_SRCS
  ${UTIL_ENDIAN_SRCS_PATH}/endian.hh )

set ( UTIL_ARITHMETIC_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/arithmetic )
set ( UTIL_ARITHMETIC_SRCS
  ${UTIL_ARITHMETIC_SRCS_PATH}/arithmetic.hh )

set ( UTIL_RANDOM_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/random )
set ( UTIL_RANDOM_SRCS
  ${UTIL_RANDOM_SRCS_PATH}/random.hh
  ${UTIL_RANDOM_SRCS_PATH}/random.cc )

set ( UTIL_DEBUG_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/debug )
set ( UTIL_DEBUG_SRCS
  ${UTIL_DEBUG_SRCS_PATH}/breakpoint.hh
  ${UTIL_DEBUG_SRCS_PATH}/breakpoint_registry.hh
  ${UTIL_DEBUG_SRCS_PATH}/breakpoint_registry.tcc
  ${UTIL_DEBUG_SRCS_PATH}/breakpoint_registry_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/breakpoint_registry_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/profile.hh
  ${UTIL_DEBUG_SRCS_PATH}/profile.tcc
  ${UTIL_DEBUG_SRCS_PATH}/profile_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/profile_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/simple_register.hh
  ${UTIL_DEBUG_SRCS_PATH}/stmt.hh
  ${UTIL_DEBUG_SRCS_PATH}/stmt.tcc
  ${UTIL_DEBUG_SRCS_PATH}/stmt_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/stmt_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/symbol.hh
  ${UTIL_DEBUG_SRCS_PATH}/symbol.tcc
  ${UTIL_DEBUG_SRCS_PATH}/symbol_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/symbol_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/symbol_table.hh
  ${UTIL_DEBUG_SRCS_PATH}/symbol_table.tcc
  ${UTIL_DEBUG_SRCS_PATH}/symbol_table_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/symbol_table_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/memory_access_type.hh
  ${UTIL_DEBUG_SRCS_PATH}/watchpoint.hh
  ${UTIL_DEBUG_SRCS_PATH}/watchpoint_registry.hh
  ${UTIL_DEBUG_SRCS_PATH}/watchpoint_registry.tcc
  ${UTIL_DEBUG_SRCS_PATH}/watchpoint_registry_32.cc
  ${UTIL_DEBUG_SRCS_PATH}/watchpoint_registry_64.cc
  ${UTIL_DEBUG_SRCS_PATH}/register.hh
)

set ( UTIL_DEBUG_BLOB_SRCS_PATH
  ${UTIL_DEBUG_SRCS_PATH}/blob )
set ( UTIL_DEBUG_BLOB_SRCS
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/blob.hh
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/section.hh
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/segment.hh
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/blob.tcc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/section.tcc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/segment.tcc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/blob32.cc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/blob64.cc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/section32.cc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/section64.cc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/segment32.cc
  ${UTIL_DEBUG_BLOB_SRCS_PATH}/segment64.cc )

set ( UTIL_DEBUG_DWARF_SRCS_PATH
  ${UTIL_DEBUG_SRCS_PATH}/dwarf )
set ( UTIL_DEBUG_DWARF_SRCS
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/abbrev.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/abbrev.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/addr_range.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/addr_range.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/attr.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/attr.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/attr.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/call_frame_prog.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/call_frame_prog.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/call_frame_vm.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/call_frame_vm.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/cie.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/cie.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/class.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/class.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/cu.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/cu.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/die.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/die.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/dwarf.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/dwarf.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/dwarf32.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/dwarf64.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/encoding.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/encoding.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/expr_vm.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/expr_vm.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/fde.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/fde.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/filename.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/filename.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/fmt.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/fwd.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/leb128.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/leb128.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/loc.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/loc.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/macinfo.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/macinfo.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/ml.cc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/ml.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/pub.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/pub.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/range.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/range.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/stmt_prog.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/stmt_prog.tcc
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/stmt_vm.hh
  ${UTIL_DEBUG_DWARF_SRCS_PATH}/stmt_vm.tcc )

set ( UTIL_DEBUG_ELF_SYMTAB_SRCS_PATH
  ${UTIL_DEBUG_SRCS_PATH}/elf_symtab )
set ( UTIL_DEBUG_ELF_SYMTAB_SRCS
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS_PATH}/elf_symtab.hh
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS_PATH}/elf_symtab.tcc
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS_PATH}/elf_symtab32.cc
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS_PATH}/elf_symtab64.cc )

set ( UTIL_LOADER_ELF_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/loader/elf_loader )
set ( UTIL_LOADER_ELF_SRCS
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf32.h
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf32_loader.cc
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf32_loader.hh
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf64.h
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf64_loader.cc
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf64_loader.hh
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf_common.h
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf_loader.hh
  ${UTIL_LOADER_ELF_SRCS_PATH}/elf_loader.tcc )

set ( UTIL_OS_LINUX_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/os/linux )
set ( UTIL_OS_LINUX_SRCS
  ${UTIL_OS_LINUX_SRCS_PATH}/linux.cc
  ${UTIL_OS_LINUX_SRCS_PATH}/linux.hh
  ${UTIL_OS_LINUX_SRCS_PATH}/linux.tcc
  ${UTIL_OS_LINUX_SRCS_PATH}/arm.hh
  ${UTIL_OS_LINUX_SRCS_PATH}/ppc.hh
  ${UTIL_OS_LINUX_SRCS_PATH}/calls.tcc
  ${UTIL_OS_LINUX_SRCS_PATH}/environment.hh
  ${UTIL_OS_LINUX_SRCS_PATH}/environment.cc
  ${UTIL_OS_LINUX_SRCS_PATH}/aux_table.hh
  ${UTIL_OS_LINUX_SRCS_PATH}/files_flags.hh )

set ( UTIL_XML_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/xml )
set ( UTIL_XML_SRCS
  ${UTIL_XML_SRCS_PATH}/xml.hh
  ${UTIL_XML_SRCS_PATH}/xml.cc
  )

# Unisim util configuration and sources                                    END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim services configuration and sources                              START #

set ( SERVICE_OS_LINUX_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/os/os_linux )
set ( SERVICE_OS_LINUX_SRCS
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.cc
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.hh
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.tcc )

set ( SERVICE_OS_TIME_HOST_TIME_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/time/host_time )

set ( SERVICE_OS_TIME_HOST_TIME_SRCS
  ${SERVICE_OS_TIME_HOST_TIME_SRCS_PATH}/time.hh
  ${SERVICE_OS_TIME_HOST_TIME_SRCS_PATH}/time.cc
  )

set ( SERVICE_OS_TIME_SC_TIME_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/time/sc_time )

set ( SERVICE_OS_TIME_SC_TIME_SRCS
  ${SERVICE_OS_TIME_SC_TIME_SRCS_PATH}/time.hh
  ${SERVICE_OS_TIME_SC_TIME_SRCS_PATH}/time.cc
  )

set_source_files_properties (
  ${SERVICE_OS_TIME_SC_TIME_SRCS_PATH}/time.cc
  PROPERTIES
    COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
  )

set ( SERVICE_TRAP_HANDLER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/trap_handler )

set ( SERVICE_TRAP_HANDLER_SRCS
  ${SERVICE_TRAP_HANDLER_SRCS_PATH}/trap_handler.hh
  ${SERVICE_TRAP_HANDLER_SRCS_PATH}/trap_handler.cc
  ${SERVICE_TRAP_HANDLER_SRCS_PATH}/trap_handler_identifier.hh
  ${SERVICE_TRAP_HANDLER_SRCS_PATH}/trap_handler_identifier.cc
  ${SERVICE_TRAP_HANDLER_SRCS_PATH}/trap_handler_identifier_interface.hh
  )

set ( SERVICE_DEBUG_GDB_SERVER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/debug/gdb_server )

set ( SERVICE_DEBUG_GDB_SERVER_SRCS
  ${SERVICE_DEBUG_GDB_SERVER_SRCS_PATH}/gdb_server.hh
  ${SERVICE_DEBUG_GDB_SERVER_SRCS_PATH}/gdb_server.tcc
  ${SERVICE_DEBUG_GDB_SERVER_SRCS_PATH}/gdb_server.cc
  ${SERVICE_DEBUG_GDB_SERVER_SRCS_PATH}/gdb_server_32.cc
  ${SERVICE_DEBUG_GDB_SERVER_SRCS_PATH}/gdb_server_64.cc
  )

set ( SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/debug/inline_debugger )

set ( SERVICE_DEBUG_INLINE_DEBUGGER_SRCS
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger.hh
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger.tcc
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger.cc
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger_32.cc
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger_64.cc
  )
set_source_files_properties (
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger_32.cc
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS_PATH}/inline_debugger_64.cc
  PROPERTIES
    COMPILE_DEFINITIONS HAVE_LIBEDIT
  )

set ( SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/debug/sim_debugger )
set ( SERVICE_DEBUG_SIM_DEBUGGER_SRCS
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH}/sim_debugger.hh
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH}/sim_debugger.tcc
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH}/sim_debugger.cc
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH}/sim_debugger_32.cc
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS_PATH}/sim_debugger_64.cc
  )

set ( SERVICE_PIM_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/pim )
set ( SERVICE_PIM_SRCS
  ${SERVICE_PIM_SRCS_PATH}/convert.hh
  ${SERVICE_PIM_SRCS_PATH}/pim_server_32.cc
  ${SERVICE_PIM_SRCS_PATH}/pim_server_64.cc
  ${SERVICE_PIM_SRCS_PATH}/pim_server.cc
  ${SERVICE_PIM_SRCS_PATH}/pim_server.hh
  ${SERVICE_PIM_SRCS_PATH}/pim_server.tcc
  ${SERVICE_PIM_SRCS_PATH}/pim_thread.cc
  ${SERVICE_PIM_SRCS_PATH}/pim_thread.hh
  ${SERVICE_PIM_SRCS_PATH}/pim.cc
  ${SERVICE_PIM_SRCS_PATH}/pim.hh
  )

set ( SERVICE_PIM_NETWORK_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/pim/network )
set ( SERVICE_PIM_NETWORK_SRCS
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/BlockingQueue.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/BlockingQueue.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/BlockingQueue.tcc
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/GenericThread.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/GenericThread.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketThread.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketThread.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketReader.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketReader.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketWriter.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketWriter.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketClientThread.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketClientThread.hpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketServerThread.cpp
  ${SERVICE_PIM_NETWORK_SRCS_PATH}/SocketServerThread.hpp
  )

set ( SERVICE_POWER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/power )
configure_file (
  ${SERVICE_POWER_SRCS_PATH}//config.hh.in 
  ${CMAKE_BINARY_DIR}/unisim/service/power/config.hh )
set ( SERVICE_POWER_SRCS
  ${SERVICE_POWER_SRCS_PATH}/cache_profile.hh
  ${SERVICE_POWER_SRCS_PATH}/cache_profile.cc
  ${SERVICE_POWER_SRCS_PATH}/cache_dynamic_energy.hh
  ${SERVICE_POWER_SRCS_PATH}/cache_dynamic_energy.cc
  ${SERVICE_POWER_SRCS_PATH}/cache_dynamic_power.hh
  ${SERVICE_POWER_SRCS_PATH}/cache_dynamic_power.cc
  ${SERVICE_POWER_SRCS_PATH}/cache_leakage_power.hh
  ${SERVICE_POWER_SRCS_PATH}/cache_leakage_power.cc
  ${SERVICE_POWER_SRCS_PATH}/cache_power_estimator.hh
  ${SERVICE_POWER_SRCS_PATH}/cache_power_estimator.cc
  ${CMAKE_BINARY_DIR}/unisim/service/power/config.hh
  )
set_source_files_properties (
  ${SERVICE_POWER_SRCS}
  PROPERTIES
    COMPILE_DEFINITIONS HAVE_CONFIG_HH
  )

# Unisim services configuration and sources                                END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim service interfaces configuration and sources                    START #

set ( SERVICE_INTERFACES_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/interfaces )
set ( SERVICE_INTERFACES_SRCS
  ${SERVICE_INTERFACES_SRCS_PATH}/memory.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/memory_injection.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/memory_access_reporting.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/debug_control.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/disassembly.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/linux_os.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/registers.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/symbol_table_lookup.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/trap_reporting.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/cache_power_estimator.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/power_mode.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/time.hh
  ${SERVICE_INTERFACES_SRCS_PATH}/stmt_lookup.hh
  )

# Unisim service interfaces configuration and sources                      END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim cxx components configuration and sources                        START #

set ( COMP_CXX_ARMEMU_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/component/cxx/processor/arm )
set ( COMP_CXX_ARMEMU_ISA_FILES
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/actions_dec.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/load_store_base.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/arm32_emu.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/load_store_multiple.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/branch.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/load_store_v4.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/constructors_dec.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/load_store_v5e.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/coprocessor.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/misc_arithmetic.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/coprocessor_v5e.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/multiply.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/data_processing.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/multiply_v5e.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/data_processing_v5e.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/profiling.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/dependency.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/semaphore.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/exception.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/specialization.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/execution_latency.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/status_register_access.isa
  ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/load_store.isa
  )

set_source_files_properties (
  ${COMP_CXX_ARMEMU_ISA_FILES}
  PROPERTIES
    LANGUAGE C
    HEADER_FILE_ONLY TRUE
  )

add_custom_command (
  OUTPUT
    ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.hh
    ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.tcc
  COMMAND
    cmake -E make_directory ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm
  COMMAND
    genisslib -o ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32 -w 32 -I ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32 ${COMP_CXX_ARMEMU_SRCS_PATH}/isa/arm32/arm32_emu.isa
  DEPENDS
    ${COMP_CXX_ARMEMU_ISA_FILES}
  COMMENT
    "Building ARM (ARM32) instruction set simulator"
  )

set ( COMP_CXX_ARMEMU_SRCS
  ${COMP_CXX_ARMEMU_SRCS_PATH}/masks.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/cpu.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/cpu.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/armemu/cpu.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/armemu/cpu.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/armemu/cache.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/armemu/cache.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/memory_op.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/memory_op.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/carry_overflow.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/carry_overflow.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_data_processing.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_data_processing.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_load_store.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_load_store.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_load_store_multiple.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_load_store_multiple.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_misc_load_store.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_misc_load_store.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_copro_load_store.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/decode_copro_load_store.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/disasm.hh
  ${COMP_CXX_ARMEMU_SRCS_PATH}/disasm.cc
  ${COMP_CXX_ARMEMU_SRCS_PATH}/armemu/isa_arm32.cc
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.hh
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.tcc
  )

set_source_files_properties (
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.hh
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.tcc
  PROPERTIES
    GENERATED TRUE )

add_custom_target ( COMP_CXX_ARM_GENERATED_FILES DEPENDS
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.hh
  ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/isa_arm32.tcc
  )

set ( COMP_CXX_MEMORY_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/component/cxx/memory/ram )

set ( COMP_CXX_MEMORY_SRCS
  ${COMP_CXX_MEMORY_SRCS_PATH}/memory.hh
  ${COMP_CXX_MEMORY_SRCS_PATH}/memory.tcc
  ${COMP_CXX_MEMORY_SRCS_PATH}/memory_32.cc
  ${COMP_CXX_MEMORY_SRCS_PATH}/memory_64.cc
	)

# Unisim cxx components configuration and sources                          END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim tlm2 components configuration and sources                        START #

set ( COMP_TLM2_ARMEMU_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/component/tlm2/processor/arm/armemu )

set ( COMP_TLM2_ARMEMU_SRCS
  ${COMP_TLM2_ARMEMU_SRCS_PATH}/armemu.hh
  ${COMP_TLM2_ARMEMU_SRCS_PATH}/armemu.cc
  )

set_source_files_properties (
  ${COMP_TLM2_ARMEMU_SRCS_PATH}/armemu.cc
  PROPERTIES
    COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
  )

set ( COMP_TLM2_MEMORY_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/component/tlm2/memory/ram )

set ( COMP_TLM2_MEMORY_SRCS
  ${COMP_TLM2_MEMORY_SRCS_PATH}/memory.hh
  ${COMP_TLM2_MEMORY_SRCS_PATH}/memory.tcc
  ${COMP_TLM2_MEMORY_SRCS_PATH}/memory.cc
  ${COMP_TLM2_MEMORY_SRCS_PATH}/memory_debug.cc
  )

set_source_files_properties (
  ${UNISIM_LIB_PATH}/memory.cc
  ${UNISIM_LIB_PATH}/memory_debug.cc
  PROPERTIES
    COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
  )

# Unisim tlm2 components configuration and sources                          END #
################################################################################
################################################################################

################################################################################
################################################################################
# GenISSLib configuration and sources                                    START #

INCLUDE ( FindCygwin )

FIND_PROGRAM ( LEX_EXECUTABLE
  NAMES
    flex
    lex
  PATH
    ${CYGWIN_INSTALL_PATH}/bin
    /bin
    /usr/bin
    /usr/local/bin
    /sbin
  )
MARK_AS_ADVANCED ( LEX_EXECUTABLE )

FIND_PROGRAM ( YACC_EXECUTABLE
  NAMES
    bison
    yacc
  PATH
    ${CYGWIN_INSTALL_PATH}/bin
    /bin
    /usr/bin
    /usr/local/bin
    /sbin
  )
MARK_AS_ADVANCED ( YACC_EXECUTABLE )

add_custom_command (
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
  COMMAND
    ${LEX_EXECUTABLE} # flex
      -l -o${CMAKE_CURRENT_BINARY_DIR}/scanner.cc ${CMAKE_SOURCE_DIR}/genisslib/scanner.ll
  DEPENDS
    ${CMAKE_SOURCE_DIR}/genisslib/scanner.ll
  COMMENT "Creating GenISSLib scanner"
  )

add_custom_command (
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/parser.h
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMMAND
    ${YACC_EXECUTABLE} -y -d -p yy -o${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_SOURCE_DIR}/genisslib/parser.yy
  COMMAND
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMMAND
    ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/parser.c
  DEPENDS
    ${CMAKE_SOURCE_DIR}/genisslib/parser.yy
  COMMENT "Creating GenISSLib parser"
  )

set ( GENISSLIB_SRCS
  ${CMAKE_SOURCE_DIR}/genisslib/action.hh
  ${CMAKE_SOURCE_DIR}/genisslib/conststr.hh
  ${CMAKE_SOURCE_DIR}/genisslib/main.hh
  ${CMAKE_SOURCE_DIR}/genisslib/riscgenerator.hh
  ${CMAKE_SOURCE_DIR}/genisslib/subdecoder.hh
  ${CMAKE_SOURCE_DIR}/genisslib/bitfield.hh
  ${CMAKE_SOURCE_DIR}/genisslib/errtools.hh
  ${CMAKE_SOURCE_DIR}/genisslib/operation.hh
  ${CMAKE_SOURCE_DIR}/genisslib/scanner.hh
  ${CMAKE_SOURCE_DIR}/genisslib/variable.hh
  ${CMAKE_SOURCE_DIR}/genisslib/ciscgenerator.hh
  ${CMAKE_SOURCE_DIR}/genisslib/fwd.hh
  ${CMAKE_SOURCE_DIR}/genisslib/parser.hh
  ${CMAKE_SOURCE_DIR}/genisslib/sourcecode.hh
  ${CMAKE_SOURCE_DIR}/genisslib/vect.hh
  ${CMAKE_SOURCE_DIR}/genisslib/cli.hh
  ${CMAKE_SOURCE_DIR}/genisslib/generator.hh
  ${CMAKE_SOURCE_DIR}/genisslib/product.hh
  ${CMAKE_SOURCE_DIR}/genisslib/specialization.hh
  ${CMAKE_SOURCE_DIR}/genisslib/comment.hh
  ${CMAKE_SOURCE_DIR}/genisslib/isa.hh
  ${CMAKE_SOURCE_DIR}/genisslib/referencecounting.hh
  ${CMAKE_SOURCE_DIR}/genisslib/strtools.hh
  ${CMAKE_SOURCE_DIR}/genisslib/action.cc
  ${CMAKE_SOURCE_DIR}/genisslib/comment.cc
  ${CMAKE_SOURCE_DIR}/genisslib/isa.cc
  ${CMAKE_SOURCE_DIR}/genisslib/referencecounting.cc
  ${CMAKE_SOURCE_DIR}/genisslib/strtools.cc
  ${CMAKE_SOURCE_DIR}/genisslib/bitfield.cc
  ${CMAKE_SOURCE_DIR}/genisslib/conststr.cc
  ${CMAKE_SOURCE_DIR}/genisslib/main.cc
  ${CMAKE_SOURCE_DIR}/genisslib/riscgenerator.cc
  ${CMAKE_SOURCE_DIR}/genisslib/subdecoder.cc
  ${CMAKE_SOURCE_DIR}/genisslib/ciscgenerator.cc
  ${CMAKE_SOURCE_DIR}/genisslib/errtools.cc
  ${CMAKE_SOURCE_DIR}/genisslib/operation.cc
  ${CMAKE_SOURCE_DIR}/genisslib/sourcecode.cc
  ${CMAKE_SOURCE_DIR}/genisslib/variable.cc
  ${CMAKE_SOURCE_DIR}/genisslib/cli.cc
  ${CMAKE_SOURCE_DIR}/genisslib/generator.cc
  ${CMAKE_SOURCE_DIR}/genisslib/product.cc
  ${CMAKE_SOURCE_DIR}/genisslib/specialization.cc
  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
  ${CMAKE_CURRENT_BINARY_DIR}/parser.h
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  )

set_source_files_properties (
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
    ${CMAKE_CURRENT_BINARY_DIR}/parser.h
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  PROPERTIES
    GENERATED TRUE )

include_directories ( ${CMAKE_SOURCE_DIR}/genisslib ${CMAKE_CURRENT_BINARY_DIR} )
add_executable ( genisslib ${GENISSLIB_SRCS} )

# GenISSLib configuration and sources                                      END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim kernel configuration and sources                                START #

configure_file (
  ${UNISIM_LIB_PATH}/unisim/kernel/service/config.hh.in
  ${CMAKE_BINARY_DIR}/unisim/kernel/service/config.hh
  )

include ( FindLibXml2 )
if ( NOT LIBXML2_FOUND )
  message ( SEND_ERROR "Could not find the libxml2 library." )
endif ( NOT LIBXML2_FOUND )

include ( FindBoost )
find_package ( Boost COMPONENTS graph )

include_directories (
  ${OSCI_SYSTEMC_INCLUDE_DIR} )
set ( KERNEL_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/kernel )
set ( KERNEL_SRCS
  ${KERNEL_SRCS_PATH}/service/service.hh
  ${KERNEL_SRCS_PATH}/service/service.cc
  ${KERNEL_SRCS_PATH}/service/xml_helper.hh
  ${KERNEL_SRCS_PATH}/service/xml_helper.cc
  ${KERNEL_SRCS_PATH}/api/api.hh
  ${KERNEL_SRCS_PATH}/api/api.cc
  ${CMAKE_BINARY_DIR}/unisim/kernel/service/config.hh
  ${KERNEL_SRCS_PATH}/logger/logger.hh
  ${KERNEL_SRCS_PATH}/logger/logger.cc
  ${KERNEL_SRCS_PATH}/logger/logger_server.hh
  ${KERNEL_SRCS_PATH}/logger/logger_server.cc
  ${KERNEL_SRCS_PATH}/debug/debug.hh
  ${KERNEL_SRCS_PATH}/debug/debug.cc
  #${KERNEL_SRCS_PATH}/tlm/tlm.hh
  #${KERNEL_SRCS_PATH}/tlm/tlm.cc
  ${KERNEL_SRCS_PATH}/tlm2/tlm.hh
  ${KERNEL_SRCS_PATH}/tlm2/tlm.cc
  )
set_source_files_properties ( ${KERNEL_SRCS_PATH}/service/service.cc
  PROPERTIES
  COMPILE_DEFINITIONS HAVE_UNISIM__KERNEL__SERVICE__CONFIG_HH
  )

# Unisim kernel configuration and sources                                  END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim api configuration and sources                                   START #

set ( API_DEBUG_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/api/debug )

set ( API_DEBUG_SRCS
  ${API_DEBUG_SRCS_PATH}/debug_api.hh
  ${API_DEBUG_SRCS_PATH}/debug_api.cc
  )

# Unisim api configuration and sources                                     END #
################################################################################
################################################################################

################################################################################
################################################################################
# Unisim uapi configuration and sources                                  START #

set ( API__SIM_EXEC_NAME ${SIM_EXEC_NAME} )
set ( API_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/uapi )
configure_file (
  ${API_SRCS_PATH}/config.h.in
  ${CMAKE_BINARY_DIR}/unisim/uapi/config.h
  )
set ( API_SRCS
  ${API_SRCS_PATH}/uapi.h
  ${API_SRCS_PATH}/simulator.c
  ${API_SRCS_PATH}/simulator.h
  ${API_SRCS_PATH}/simulator_priv.h
  ${API_SRCS_PATH}/variable.c
  ${API_SRCS_PATH}/variable.h
  ${API_SRCS_PATH}/variable_priv.h
  ${API_SRCS_PATH}/eapi.c
  ${API_SRCS_PATH}/eapi.h
  ${API_SRCS_PATH}/eapi_priv.h
  ${API_SRCS_PATH}/types.h
  ${API_SRCS_PATH}/debug/debug.h
  ${API_SRCS_PATH}/debug/debug.c
  )
set_source_files_properties ( ${API_SRCS}
  PROPERTIES
    LANGUAGE CXX
  )

# Unisim uapi configuration and sources                                    END #
################################################################################
################################################################################

set ( SIMULATOR_SRCS
  ${CMAKE_SOURCE_DIR}/simulator.cc
  ${CMAKE_SOURCE_DIR}/simulator.hh
  ${CMAKE_BINARY_DIR}/config.hh
  ${KERNEL_SRCS}
  ${SERVICE_INTERFACES_SRCS}
  ${SERVICE_OS_LINUX_SRCS}
  ${SERVICE_OS_TIME_HOST_TIME_SRCS}
  ${SERVICE_OS_TIME_SC_TIME_SRCS}
  ${SERVICE_TRAP_HANDLER_SRCS}
  ${SERVICE_DEBUG_GDB_SERVER_SRCS}
  ${SERVICE_DEBUG_INLINE_DEBUGGER_SRCS}
  ${SERVICE_DEBUG_SIM_DEBUGGER_SRCS}
  ${UTIL_ENDIAN_SRCS}
  ${UTIL_ARITHMETIC_SRCS}
  ${UTIL_RANDOM_SRCS}
  ${UTIL_DEBUG_SRCS}
  ${UTIL_DEBUG_BLOB_SRCS}
  ${UTIL_DEBUG_DWARF_SRCS}
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS}
  ${UTIL_LOADER_ELF_SRCS}
  ${UTIL_OS_LINUX_SRCS}
  ${UTIL_XML_SRCS}
  ${COMP_CXX_ARMEMU_SRCS}
  ${COMP_CXX_MEMORY_SRCS}
  ${COMP_TLM2_ARMEMU_SRCS}
  ${COMP_TLM2_MEMORY_SRCS}
  ${API_DEBUG_SRCS}
  )

set ( SIMULATOR_REQUIRED_LIBRARIES
  ${OSCI_SYSTEMC_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${LIBEDIT} )
if ( HAVE_CACTI4_2 )
  list ( APPEND SIMULATOR_REQUIRED_LIBRARIES
    ${CACTI_4_2_LIBRARY} )
endif ( HAVE_CACTI4_2 )

if ( BUILD_SIM_PIM_SUPPORT )
  list ( APPEND SIMULATOR_SRCS
    ${SERVICE_PIM_SRCS} ${SERVICE_PIM_NETWORK_SRCS} )
  if ( WIN32 )
    list ( APPEND SIMULATOR_REQUIRED_LIBRARIES
      wsock32 )
  endif ( WIN32 )
endif ( BUILD_SIM_PIM_SUPPORT )

if ( BUILD_ARMEMU_POWER_SUPPORT )
  list ( APPEND SIMULATOR_SRCS
    ${SERVICE_POWER_SRCS} )
endif ( BUILD_ARMEMU_POWER_SUPPORT )

#set ( armemu__libraries
  #${OSCI_SYSTEMC_LIBRARIES}
  #unisim__kernel
  ## unisim__component__cxx__processor__arm
  #unisim__component__tlm2__processor__arm__armemu
  ## unisim__component__tlm2__interrupt
  #unisim__component__tlm2__memory__ram
  #unisim__service__time__host_time
  #unisim__service__time__sc_time
  #unisim__service__loader__elf_loader
  #unisim__service__loader__linux_loader
  #unisim__service__os__linux_os__linux_os_32
  #unisim__service__debug__gdb_server
  #unisim__service__debug__inline_debugger
  #unisim__service__debug__sim_debugger
  #unisim__api__debug
  #unisim__service__trap_handler
  #)

#if ( BUILD_SIM_PIM_SUPPORT )
  #list ( APPEND armemu__libraries
    #unisim__service__pim unisim__service__pim__network )
#endif ( BUILD_SIM_PIM_SUPPORT )

#if ( BUILD_ARMEMU_POWER_SUPPORT )
  #list ( APPEND armemu__libraries
    #unisim__service__power )
#endif ( BUILD_ARMEMU_POWER_SUPPORT )
#if ( WIN32 )
  #list ( APPEND armemu__libraries
    #wsock32 )
#endif ( WIN32 )

#set ( armemu_subdirs
  #unisim/kernel
  #unisim/component/tlm2/processor/arm/armemu
  #unisim/component/tlm2/memory/ram
  #unisim/service/time/sc_time
  #unisim/service/time/host_time
  #unisim/service/debug/gdb_server
  #unisim/service/debug/inline_debugger
  #unisim/service/debug/sim_debugger
  #unisim/api/debug
  #unisim/service/loader/elf_loader
  #unisim/service/loader/linux_loader
  #unisim/service/os/linux_os/linux_os_32
  #unisim/service/trap_handler
  #unisim/service/power
  #unisim/service/pim
  #unisim/service/pim/network
  #)
#foreach ( dir ${armemu_subdirs} )
  #unisim_include ( ${dir} )
#endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${OSCI_SYSTEMC_INCLUDE_DIR}
  ${OSCI_TLM2_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  )

if ( BUILD_SIM_PIM_SUPPORT )
  set ( SIM_PIM_SUPPORT TRUE )
else ( BUILD_SIM_PIM_SUPPORT )
  set ( SIM_PIM_SUPPORT FALSE )
endif ( BUILD_SIM_PIM_SUPPORT )

if ( BUILD_ARMEMU_POWER_SUPPORT )
  set ( SIM_POWER_ESTIMATOR_SUPPORT TRUE )
else ( BUILD_ARMEMU_POWER_SUPPORT )
  set ( SIM_POWER_ESTIMATOR_SUPPORT FALSE )
endif ( BUILD_ARMEMU_POWER_SUPPORT )

add_custom_target ( armemu__share_license
  ALL
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/AUTHORS.txt
      ${SHARE_DIRECTORY}/AUTHORS.txt
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/COPYING.txt
      ${SHARE_DIRECTORY}/COPYING.txt
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/INSTALL.txt
      ${SHARE_DIRECTORY}/INSTALL.txt
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/NEWS.txt
      ${SHARE_DIRECTORY}/NEWS.txt
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/README.txt
      ${SHARE_DIRECTORY}/README.txt
  COMMENT
    "Copying license files."
  )

add_custom_target ( armemu__share_gdb
  ALL
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/gdb_server
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_arm_with_fpa.xml
      ${SHARE_DIRECTORY}/gdb_server/gdb_arm_with_fpa.xml
  COMMENT
    "Setting GDB resource files"
  )

add_custom_target ( armemu__share_test
  ALL
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/src
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/build
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/install
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/test/src/main.c
      ${SHARE_DIRECTORY}/test/src/main.c
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/test/src/CMakeLists.txt
      ${SHARE_DIRECTORY}/test/src/CMakeLists.txt
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/test/src/toolchain-armv5l.cmake
      ${SHARE_DIRECTORY}/test/src/toolchain-armv5l.cmake
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/test/test.armv5l
      ${SHARE_DIRECTORY}/test/install/test.armv5l
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/test/mibench
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/test/mibench/run.py
      ${SHARE_DIRECTORY}/test/mibench/run.py
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/share/test/mibench/automotive
      ${SHARE_DIRECTORY}/test/mibench/automotive
  COMMENT
    "Setting test program source files"
  )

add_custom_target ( armemu__share_logo
  ALL
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/logo
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.icns
      ${SHARE_DIRECTORY}/logo/logo_unisim.icns
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.png
      ${SHARE_DIRECTORY}/logo/logo_unisim.png
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim16.png
      ${SHARE_DIRECTORY}/logo/logo_unisim16.png
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim32.png
      ${SHARE_DIRECTORY}/logo/logo_unisim32.png
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim128.png
      ${SHARE_DIRECTORY}/logo/logo_unisim128.png
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/logo_unisim256.png
      ${SHARE_DIRECTORY}/logo/logo_unisim256.png
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/share/logo/unisim.ico
      ${SHARE_DIRECTORY}/logo/unisim.ico
  COMMENT
    "Copying icon files"
  )

add_custom_target ( armemu__share_python
  ALL
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/python
  # COMMAND
  # ${CMAKE_COMMAND} -E copy_if_different
  # 	${CMAKE_SOURCE_DIR}/share/python/demo.py
  # 	${SHARE_DIRECTORY}/python/demo.py
  COMMENT
    "Setting python demos."
  )

if ( APPLE )
  configure_file (
    ${CMAKE_SOURCE_DIR}/share/macosx/armemu-launcher.sh.in
    ${SHARE_DIRECTORY}/macosx/armemu-launcher.sh )
  add_custom_target ( armemu__share_macosx
    ALL
    COMMAND
      ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}/macosx
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/share/macosx/Info.plist
        ${SHARE_DIRECTORY}/macosx/Info.plist
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/share/macosx/armemu-launcher.scpt
        ${SHARE_DIRECTORY}/macosx/armemu-launcher.scpt
    COMMENT
      "Setting MacOSX application files."
    )
elseif ( UNIX )
  configure_file (
    ${CMAKE_SOURCE_DIR}/share/linux/unisim-armemu.desktop.in
    ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in )
  add_custom_target ( armemu__share_linux
    ALL
    COMMAND
      ${CMAKE_COMMAND} -E copy
        ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in
        ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
    COMMAND
      desktop-file-install --dir=${SHARE_DIRECTORY}/linux ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
    DEPENDS
      ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop.in
    COMMENT
      "Setting Linux desktop application files."
    )
endif ( APPLE )

set ( UNISIM_KERNEL_PYTHON_TO_SHARED_DATA_PATH "../../share/${ARMEMU_EXEC_NAME}" )
set ( PYTHON__ARMEMU__INCLUDE_DIRS
  "'${CMAKE_SOURCE_DIR}',
  '${CMAKE_BINARY_DIR}',
  '${OSCI_SYSTEMC_INCLUDE_DIR}',
  '${OSCI_TLM2_INCLUDE_DIR}'")
set ( PYTHON__ARMEMU__LIB_DIRS
  "'${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}'")
message ( STATUS "python armemu lib dirs = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
foreach ( lib ${OSCI_SYSTEMC_LIBRARIES} )
  get_filename_component( lib_path ${lib}
    PATH )
  set ( PYTHON__ARMEMU__LIB_DIRS
    "${PYTHON__ARMEMU__LIB_DIRS},
    '${lib_path}'")
endforeach ( lib ${OSCI_SYSTEMC_LIBRARIES} )
set ( PYTHON__ARMEMU__LIBS
  "'simulator',
  'xml2',
  'systemc'")
  #'unisim__component__tlm2__memory__ram',
  #'unisim__component__tlm2__processor__arm__armemu',
  #'unisim__component__cxx__processor__arm__armemu',
  #'unisim__component__cxx__memory__ram',
  #'unisim__service__time__host_time',
  #'unisim__service__time__sc_time',
  #'unisim__service__loader__elf_loader',
  #'unisim__service__loader__linux_loader',
  #'unisim__service__os__linux_os__linux_os_32',
  #'unisim__service__os__linux_os',
  #'unisim__service__debug__gdb_server',
  #'unisim__service__debug__inline_debugger',
  #'unisim__service__debug__sim_debugger',
  #'unisim__service__trap_handler',
  #'unisim__util__random',
  #'unisim__util__debug',
  #'unisim__util__debug__dwarf',
  #'unisim__api__debug',
  #'unisim__util__debug__blob',
  #'unisim__util__xml',
  #'unisim__kernel',
if ( BUILD_ARMEMU_POWER_SUPPORT )
  get_filename_component ( lib_path ${CACTI_4_2_LIBRARY}
    PATH )
  set ( PYTHON__ARMEMU__LIB_DIRS
    "${PYTHON__ARMEMU__LIB_DIRS},
    '${lib_path}'")
  get_filename_component ( lib_name ${CACTI_4_2_LIBRARY}
    NAME_WE )
  string ( REGEX REPLACE "^lib" "" lib_name ${lib_name} )
  set ( PYTHON__ARMEMU__LIBS
    "${PYTHON__ARMEMU__LIBS},
    '${lib_name}'" )
    #'unisim__service__power',
endif ( BUILD_ARMEMU_POWER_SUPPORT )

set ( CMAKE_REQUIRED_FLAGS "-include stdio.h" )
check_include_file ( editline/readline.h HAVE_editline_readline_H )
if ( HAVE_editline_readline_H )
  find_library ( LIBEDIT
    edit )
endif ( HAVE_editline_readline_H )

configure_file (
  ${CMAKE_SOURCE_DIR}/config.hh.in
  ${CMAKE_BINARY_DIR}/config.hh )

# add_library ( simulator
# 	${simulator__sources}
# 	)
# target_link_libraries ( simulator
# 	${armemu__libraries}
# 	)
# set_target_properties ( simulator
# 	PROPERTIES
# 		COMPILE_DEFINITIONS SIM_EXECUTABLE
# 	)

add_library ( simulator
  ${SIMULATOR_SRCS} )
set_target_properties ( simulator
  PROPERTIES
    COMPILE_DEFINITIONS SIM_LIBRARY )
#add_library ( simulatorlib
  #${simulator__sources}
  #)
#target_link_libraries ( simulatorlib
  #${armemu__libraries}
  #)
#set_target_properties ( simulatorlib
  #PROPERTIES
  #COMPILE_DEFINITIONS SIM_LIBRARY
  #)

add_executable ( ${ARMEMU_EXEC_TARGET}
  ${CMAKE_SOURCE_DIR}/main.cc
  ${SIMULATOR_SRCS} )
set_target_properties ( ${ARMEMU_EXEC_TARGET}
  PROPERTIES
    COMPILE_DEFINITIONS SIM_EXECUTABLE )
target_link_libraries ( ${ARMEMU_EXEC_TARGET}
  ${SIMULATOR_REQUIRED_LIBRARIES} )
#add_executable ( ${ARMEMU_EXEC_TARGET}
  #${CMAKE_SOURCE_DIR}/main.cc
  #${simulator__sources}
  #)
#set_target_properties ( ${ARMEMU_EXEC_TARGET}
  #PROPERTIES
    #COMPILE_DEFINITIONS SIM_EXECUTABLE )
#target_link_libraries ( ${ARMEMU_EXEC_TARGET}
  ## simulator
  #${armemu__libraries}
  #)
# add_library ( ${ARMEMU_LIB_TARGET}
# 	SHARED
# 		${simulator__sources} )
# target_link_libraries ( ${ARMEMU_LIB_TARGET}
# 	${armemu__libraries} )

# set_target_properties ( simulator
# 	PROPERTIES
# 		VERSION ${SIM_VERSION}
# 		SOVERSION ${SIM_SOVERSION} )
set_target_properties ( ${ARMEMU_EXEC_TARGET}
  PROPERTIES
    RUNTIME_OUTPUT_NAME ${ARMEMU_EXEC_NAME} )
# set_target_properties ( ${ARMEMU_LIB_TARGET}
# 	PROPERTIES
# 		LIBRARY_OUTPUT_NAME ${ARMEMU_LIB_NAME}
# 		VERSION ${SIM_VERSION}
# 		SOVERSION ${SIM_SOVERSION} )

add_library ( libuapi SHARED
  ${SIMULATOR_SRCS}
  ${API_SRCS} )
set_target_properties ( libuapi
  PROPERTIES
    COMPILE_DEFINITIONS SIM_LIBRARY )
target_link_libraries ( libuapi
  ${SIMULATOR_REQUIRED_LIBRARIES} )
#add_library ( libuapi SHARED
  #${API_SRCS}
  #)
#target_link_libraries ( libuapi
  #simulatorlib
  #)
#set_target_properties ( libuapi
  #PROPERTIES
  #COMPILE_DEFINITIONS SIM_LIBRARY )

#set ( armemu__deps
  #unisim__kernel
  #unisim__component__tlm2__processor__arm__armemu
  ## unisim__component__tlm2__interrupt
  #unisim__component__tlm2__memory__ram
  #unisim__service__time__sc_time
  #unisim__service__time__host_time
  #unisim__service__debug__gdb_server
  #unisim__service__debug__inline_debugger
  #unisim__service__debug__sim_debugger
  #unisim__api__debug
  #unisim__service__loader__elf_loader
  #unisim__service__loader__linux_loader
  #unisim__service__os__linux_os__linux_os_32
  #unisim__service__trap_handler
  #)

#if ( BUILD_SIM_PIM_SUPPORT )
  #list ( APPEND armemu__deps
    #unisim__service__pim unisim__service__pim__network
    #)
#endif ( BUILD_SIM_PIM_SUPPORT )

#if ( BUILD_ARMEMU_POWER_SUPPORT )
  #list ( APPEND armemu__deps
    #unisim__service__power
    #)
#endif ( BUILD_ARMEMU_POWER_SUPPORT )
# add_dependencies ( simulator
# 	${armemu__deps}
# 	)
# add_dependencies ( ${ARMEMU_LIB_TARGET}
# 	${armemu__deps}
# 	)
#add_dependencies ( ${ARMEMU_EXEC_TARGET}
  #${armemu__deps}
  #)

add_custom_command (
  OUTPUT
    ${SHARE_DIRECTORY}/template-default-config.xml
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHARE_DIRECTORY}
  COMMAND
    ${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME} -g ${SHARE_DIRECTORY}/template-default-config.xml
  DEPENDS
    ${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
  COMMENT
    "Creating simulator default configuration in \"${RELATIVE_SHARE_DIRECTORY}/template-default-config.xml\""
  )

if ( UNIX )
  add_custom_command (
    TARGET ${ARMEMU_EXEC_TARGET}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink ${ARMEMU_EXEC_NAME} unisim-armemu
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT
      "Creating a symbolic link with the generic name \"bin/unisim-armemu\" to \"bin/${ARMEMU_EXEC_NAME}\""
    )
endif ( UNIX )

if ( BUILD_ARMEMU_PYTHON_MODULE )
  set ( PYTHON__SIM__MODULE_NAME "armemu" )
  set ( PYTHON__SIM__IMPORT_NAME "armemu${SIM_VERSION_MAJOR}${SIM_VERSION_MINOR}${SIM_VERSION_PATCH}" )
  set ( PYTHON__SIM__EXTRA_COMPILE_FLAGS "-DSIM_LIBRARY" )
  set ( PYTHON__SIM__SIM_EXEC_NAME ${ARMEMU_EXEC_NAME} )
  set ( PYTHON__SIM__INCLUDE_DIRS
    "'${CMAKE_SOURCE_DIR}',
    '${CMAKE_BINARY_DIR}',
    '${OSCI_SYSTEMC_INCLUDE_DIR}',
    '${OSCI_TLM2_INCLUDE_DIR}'")
    set ( PYTHON__SIM__LIB_DIRS
      "'${CMAKE_LIBRARY_OUTPUT_DIRECTORY}'")
    set ( PYTHON__SIM__LIBS
      "'libuapi'" )
    if ( BUILD_POWER_SUPPORT )
      get_filename_component ( lib_path ${CACTI_4_2_LIBRARY}
        PATH )
      set ( PYTHON__SIM__LIB_DIRS
        "${PYTHON__SIM__LIB_DIRS},
        '${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}',
        '${lib_path}'")
        get_filename_component ( lib_name ${CACTI_4_2_LIBRARY}
          NAME_WE )
        string ( REGEX REPLACE "^lib" "" lib_name ${lib_name} )
        set ( PYTHON__SIM__LIBS
          "${PYTHON__SIM__LIBS},
          '${lib_name}'" )
          #'unisim__service__power',
        endif ( BUILD_POWER_SUPPORT )
        if ( LIBEDIT )
          set ( PYTHON__SIM__LIBS
            "${PYTHON__SIM__LIBS},'edit'" )
        endif ( LIBEDIT )

        include ( python/UnisimPython.cmake )

        configure_file (
          ${CMAKE_SOURCE_DIR}/share/python/demo.py.in
          ${SHARE_DIRECTORY}/python/demo.py )
        configure_file (
          ${CMAKE_SOURCE_DIR}/share/python/demo2.py.in
          ${SHARE_DIRECTORY}/python/demo2.py )

        #	configure_file (
        #		${CMAKE_SOURCE_DIR}/python/setup.py.in
        #		${CMAKE_BINARY_DIR}/python/setup.py )
        #	configure_file (
        #		${CMAKE_SOURCE_DIR}/python/python_config.hh.in
        #		${CMAKE_BINARY_DIR}/python/python_config.hh )
        #
        #	add_custom_command (
        #		OUTPUT
        #			lib/${SIM_PYTHON_IMPORT_NAME}/simulator.so
        #			lib/${SIM_PYTHON_IMPORT_NAME}/variable.so
        #			lib/${SIM_PYTHON_IMPORT_NAME}/debugger.so
        #		COMMAND
        #			${CMAKE_COMMAND} -E remove lib/${SIM_PYTHON_IMPORT_NAME}/simulator.so
        #		COMMAND
        #			${CMAKE_COMMAND} -E remove lib/${SIM_PYTHON_IMPORT_NAME}/variable.so
        #		COMMAND
        #			${CMAKE_COMMAND} -E remove lib/${SIM_PYTHON_IMPORT_NAME}/debugger.so
        #		COMMAND
        #			${CMAKE_COMMAND} -E make_directory python
        #		COMMAND
        #			${PYTHON_EXECUTABLE} python/setup.py build -t python --build-lib lib
        #		DEPENDS
        #			${CMAKE_BINARY_DIR}/config.hh
        #			${CMAKE_BINARY_DIR}/python/setup.py
        #			${CMAKE_BINARY_DIR}/python/python_config.hh
        #			${CMAKE_SOURCE_DIR}/python/py_simulator.hh
        #			${CMAKE_SOURCE_DIR}/python/py_simulator.cc
        #			${CMAKE_SOURCE_DIR}/python/py_variable.hh
        #			${CMAKE_SOURCE_DIR}/python/py_variable.cc
        #			${CMAKE_SOURCE_DIR}/python/py_debugger.hh
        #			${CMAKE_SOURCE_DIR}/python/py_debugger.cc
        #			simulatorlib
        #			${armemu__deps}
        #		COMMENT
        #			"Building python extension."
        #		)
        #
        #	if ( ${PYTHONINTERP_FOUND} )
        #		add_custom_target ( ${ARMEMU_LIB_TARGET}-python
        #			ALL
        #			DEPENDS
        #				lib/${SIM_PYTHON_IMPORT_NAME}/simulator.so
        #				lib/${SIM_PYTHON_IMPORT_NAME}/variable.so
        #				lib/${SIM_PYTHON_IMPORT_NAME}/debugger.so
        #			COMMENT
        #				"Python target."
        #		)
        #	endif ( ${PYTHONINTERP_FOUND} )
        #
        #	if ( ${PYTHONINTERP_FOUND} )
        #		install (
        #			FILES
        #				${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
        #			DESTINATION
        #				lib
        #			)
        #	endif ( ${PYTHONINTERP_FOUND} )
        #	if ( LIBEDIT )
        #		set ( PYTHON__ARMEMU__LIBS
        #			"${PYTHON__ARMEMU__LIBS},'edit'" )
        #	endif ( LIBEDIT )
      endif ( BUILD_ARMEMU_PYTHON_MODULE )

      install ( TARGETS ${ARMEMU_EXEC_TARGET} # ${ARMEMU_LIB_TARGET}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
      # if ( APPLE )
      # 	install ( TARGETS ${ARMEMU_EXEC_TARGET}-apple
      # 		BUNDLE DESTINATION applications )
      # endif ( APPLE )
      if ( UNIX )
        install (
          FILES ${CMAKE_BINARY_DIR}/bin/unisim-armemu
          DESTINATION bin
          )
      endif ( UNIX )
      install (
        FILES
          ${SHARE_DIRECTORY}/AUTHORS.txt
          ${SHARE_DIRECTORY}/COPYING.txt
          ${SHARE_DIRECTORY}/INSTALL.txt
          ${SHARE_DIRECTORY}/NEWS.txt
          ${SHARE_DIRECTORY}/README.txt
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}
        )
      install (
        FILES
          ${SHARE_DIRECTORY}/template-default-config.xml
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}
        )
      install (
        FILES
          ${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_arm_with_fpa.xml
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/gdb_server
        )
      install (
        FILES
          ${SHARE_DIRECTORY}/test/src/main.c
          ${SHARE_DIRECTORY}/test/src/CMakeLists.txt
          ${SHARE_DIRECTORY}/test/src/toolchain-armv5l.cmake
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/test/src
        )
      # the test programs will be installed if they are compiled
      install (
        PROGRAMS
          ${SHARE_DIRECTORY}/test/install/test.armv5l
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/test/install
        OPTIONAL
        )
      install (
        FILES
          ${SHARE_DIRECTORY}/test/mibench/automotive/basicmath/config_large.xml
          ${SHARE_DIRECTORY}/test/mibench/automotive/basicmath/config_small.xml
          ${SHARE_DIRECTORY}/test/mibench/automotive/basicmath/output_large.txt
          ${SHARE_DIRECTORY}/test/mibench/automotive/basicmath/output_small.txt
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/test/mibench/automotive/basicmath
        )
      install (
        FILES
          ${SHARE_DIRECTORY}/test/mibench/run.py
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/test/mibench
        )
      install (
        FILES
          ${SHARE_DIRECTORY}/logo/logo_unisim.icns
          ${SHARE_DIRECTORY}/logo/logo_unisim.png
          ${SHARE_DIRECTORY}/logo/logo_unisim256.png
          ${SHARE_DIRECTORY}/logo/logo_unisim128.png
          ${SHARE_DIRECTORY}/logo/logo_unisim32.png
          ${SHARE_DIRECTORY}/logo/logo_unisim16.png
          ${SHARE_DIRECTORY}/logo/unisim.ico
        DESTINATION
          ${RELATIVE_SHARE_DIRECTORY}/logo
        )
      if ( APPLE )
        install (
          FILES
            ${SHARE_DIRECTORY}/macosx/Info.plist
            ${SHARE_DIRECTORY}/macosx/armemu-launcher.scpt
            ${SHARE_DIRECTORY}/macosx/armemu-launcher.sh
          DESTINATION
            ${RELATIVE_SHARE_DIRECTORY}/macosx
          )
      elseif ( UNIX )
        #	add_custom_command ( TARGET ${ARMEMU_EXEC_TARGET}
        #		POST_BUILD
        #		COMMAND
        #			${CMAKE_SOURCE_DIR}/share/linux/find_deb_dependencies.sh
        #				${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
        #				${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}
        #				${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
        #				> ${CMAKE_BINARY_DIR}/deps.deb
        #		COMMENT
        #			"Computing ARMEmu Debian dependencies."
        #		)
        add_custom_command (
          OUTPUT ${CMAKE_BINARY_DIR}/deps.deb
          COMMAND
            ${CMAKE_SOURCE_DIR}/share/linux/find_deb_dependencies.sh
              ${CMAKE_BINARY_DIR}/bin/${ARMEMU_EXEC_NAME}
              ${CMAKE_BINARY_DIR}/lib/${ARMEMU_LIB_NAME}
              ${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
              > ${CMAKE_BINARY_DIR}/deps.deb
          DEPENDS
            ${ARMEMU_EXEC_TARGET}
            ${ARMEMU_LIB_TARGET}
            ${CMAKE_BINARY_DIR}/lib/${SIM_PYTHON_IMPORT_NAME}.so
          COMMENT
            "Computing ARMEmu Debian dependencies."
          )
        add_custom_target ( debian_package
          COMMAND
            make
          COMMAND
            ${CMAKE_COMMAND} .
          COMMAND
            make package
          DEPENDS ${CMAKE_BINARY_DIR}/deps.deb
          )
        if ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
          file ( STRINGS ${CMAKE_BINARY_DIR}/deps.deb ARMEMU__DEPS_DEB
            LIMIT_COUNT 1 )
          message ( "ARMEMU__DEPS_DEB = ${ARMEMU__DEPS_DEB}" )
          set ( ARMEMU__DEPS_DEB ${ARMEMU__DEPS_DEB} CACHE STRING "Debian dependencies" FORCE )
        else ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
          set ( ARMEMU__DEPS_DEB "" CACHE STRING "Debian dependencies" FORCE )
          message ( "Could not find debian dependencies file, if you "
            "are executing make package, make sure you have done a simple "
            "make or make install before (or a previous make package)." )
        endif ( EXISTS ${CMAKE_BINARY_DIR}/deps.deb )
        install (
          FILES
            ${SHARE_DIRECTORY}/linux/${ARMEMU_EXEC_NAME}.desktop
          DESTINATION
            share/applications
          )
      endif ( APPLE )

      add_custom_target ( armemu__resources
        ALL
        DEPENDS
          ${SHARE_DIRECTORY}/template-default-config.xml
        )

      add_dependencies ( armemu__resources
        ${ARMEMU_EXEC_TARGET} )

      add_test ( working_test
        bin/${ARMEMU_EXEC_NAME}
        )

      add_custom_target ( prepare_check
        COMMENT
          "Preparing tests"
        )

      add_custom_target ( calling_ctest
        COMMAND
          ${CMAKE_CTEST_COMMAND}
        DEPENDS
          ${ARMEMU_EXEC_TARGET} )

      add_custom_target ( check
        COMMENT
          "Tests finished"
        )

      add_dependencies ( check
        prepare_check
        calling_ctest
        )

      enable_testing()

      set ( CPACK_PACKAGE_NAME "unisim-armemu" )
      set ( CPACK_PACKAGE_FILE_NAME "unisim-armemu-${SIM_VERSION}" )
      if ( DISTRIB_ID AND DISTRIB_RELEASE )
        if ( ${DISTRIB_ID} MATCHES "Ubuntu" )
          set ( CPACK_PACKAGE_FILE_NAME "unisim-armemu_${SIM_VERSION}-${DISTRIB_ID}${DISTRIB_RELEASE}_${CMAKE_SYSTEM_PROCESSOR}" )
        endif ( ${DISTRIB_ID} MATCHES "Ubuntu" )
      endif ( DISTRIB_ID AND DISTRIB_RELEASE )
      set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "UNISIM ArmEmu project." )
      set ( CPACK_PACKAGE_VENDOR "CEA" )
      set ( CPACK_PACKAGE_CONTACT "daniel.gracia-perez@cea.fr" )
      set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.txt" )
      set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.txt" )
      set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt" )
      set ( CPACK_PACKAGE_VERSION_MAJOR ${SIM_VERSION_MAJOR} )
      set ( CPACK_PACKAGE_VERSION_MINOR ${SIM_VERSION_MINOR} )
      set ( CPACK_PACKAGE_VERSION_PATCH ${SIM_VERSION_PATCH} )
      set ( CPACK_PACKAGE_INSTALL_DIRECTORY "UNISIM ArmEmu ${SIM_VERSION}" )
      if ( APPLE )
        set ( CPACK_GENERATOR "Bundle" )
        set ( CPACK_BUNDLE_NAME "UNISIM ArmEmu" )
        set ( CPACK_BUNDLE_PLIST "${SHARE_DIRECTORY}/macosx/Info.plist" )
        set ( CPACK_BUNDLE_STARTUP_COMMAND "${SHARE_DIRECTORY}/macosx/armemu-launcher.sh" )
        set ( CPACK_BUNDLE_ICON "${SHARE_DIRECTORY}/logo/logo_unisim.icns" )
      elseif ( UNIX )
        set ( CPACK_GENERATOR "DEB" )
        set ( CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386 )
        set ( CPACK_DEBIAN_PACKAGE_DEPENDS ${ARMEMU__DEPS_DEB} )
        set ( CPACK_DEBIAN_PACKAGE_SECTION "devel" )
        set ( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
        set ( CPACK_DEBIAN_PACKAGE_RECOMMENDS "" )
        set ( CPACK_DEBIAN_PACKAGE_SUGGESTS "" )
        set ( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "" )
      endif ( APPLE )
      # set ( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/logo/logo_unisim.icns" )
      IF(WIN32 AND NOT UNIX)
        # There is a bug in NSI that does not handle full unix paths properly. Make
        # sure there is at least one set of four (4) backlasshes.
        SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
        SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
        SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
        SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
        SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
        SET(CPACK_NSIS_MODIFY_PATH ON)
        # ELSE(WIN32 AND NOT UNIX)
        # 	SET(CPACK_STRIP_FILES "bin/${ARMEMU_EXEC_NAME}")
        # SET(CPACK_SOURCE_STRIP_FILES "")
      ENDIF(WIN32 AND NOT UNIX)
      SET(CPACK_PACKAGE_EXECUTABLES "${ARMEMU_EXEC_NAME}" "${ARMEMU_EXEC_NAME}")
      set ( CPACK_SOURCE_GENERATOR "TBZ2;TGZ;ZIP" )
      set ( CPACK_SOURCE_PACKAGE_FILE_NAME "unisim-armemu-${SIM_VERSION}" )
      INCLUDE(CPack)
