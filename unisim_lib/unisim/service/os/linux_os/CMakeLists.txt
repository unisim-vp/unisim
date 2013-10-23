cmake_minimum_required ( VERSION 2.8 )
project ( unisim_lib_unisim_service_os_os_linux )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )

set ( UNISIM_LIB_PATH
  ${CMAKE_SOURCE_DIR}/../../../.. )
include ( ${UNISIM_LIB_PATH}/cmake/FindOSCISystemC.cmake )

# include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
set_property (
  GLOBAL
  PROPERTY
  BIN_TO_SHARED_DATA_PATH
  "../share/unisim_service_os_os_linux" )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/bin
  )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/lib/unisim_service_os_os_linux
  )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/lib
  )

set ( C_INCLUDES stdlib unistd fcntl errno inttypes sys/types sys/stat )
if ( WIN32 )
	list ( APPEND C_INCLUDES windows process )
else ( WIN32 )
	list ( APPEND C_INCLUDES sys/times )
endif ( WIN32 )

foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES map string vector iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

include_directories ( ${UNISIM_LIB_PATH} ${CMAKE_BINARY_DIR} )

set ( UTIL_ENDIAN_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/endian )
set ( UTIL_ENDIAN_SRCS
  ${UTIL_ENDIAN_SRCS_PATH}/endian.hh )

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
  ${LIBXML2_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIRS} 
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
  ${KERNEL_SRCS_PATH}/tlm/tlm.hh
  ${KERNEL_SRCS_PATH}/tlm/tlm.cc
  ${KERNEL_SRCS_PATH}/tlm2/tlm.hh
  )
set_source_files_properties ( ${KERNEL_SRCS_PATH}/service/service.cc
  PROPERTIES
  COMPILE_DEFINITIONS HAVE_UNISIM__KERNEL__SERVICE__CONFIG_HH
  )

# Unisim kernel configuration and sources                                  END #
################################################################################
################################################################################

set ( SERVICE_OS_LINUX_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/service/os/os_linux )
set ( SERVICE_OS_LINUX_SRCS
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.cc
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.hh
  ${SERVICE_OS_LINUX_SRCS_PATH}/linux_os.tcc )

add_library ( unisim_service_os_linux
  ${SERVICE_OS_LINUX_SRCS}
  ${KERNEL_SRCS}
  ${UTIL_OS_LINUX_SRCS}
  ${UTIL_ENDIAN_SRCS}
  ${UTIL_DEBUG_SRCS}
  ${UTIL_DEBUG_BLOB_SRCS}
  ${UTIL_DEBUG_DWARF_SRCS}
  ${UTIL_DEBUG_ELF_SYMTAB_SRCS}
  ${UTIL_LOADER_ELF_SRCS} )

