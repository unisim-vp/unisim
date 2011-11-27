cmake_minimum_required ( VERSION 2.8 )
project ( unisim_lib_unisim_util_os_linux )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
# include ( ${CMAKE_SOURCE_DIR}/../../../../cmake/UnisimInclude.cmake ) 

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh 
  CACHE
  STRING "Extensions of the source files" )

set ( CXX_INCLUDES map string vector iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
  check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
  if ( NOT HAVE_${CXX_INCLUDE} )
    message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
  endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES string stdlib inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
  check_include_file( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
  if ( NOT HAVE_${C_INCLUDE}_H )
    message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}\"." )
  endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( UNISIM_LIB_PATH
  ${CMAKE_SOURCE_DIR}/../../../.. )
include_directories ( ${UNISIM_LIB_PATH} ${CMAKE_BINARY_DIR} )

set ( ENDIAN_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/endian )
set ( ENDIAN_SRCS
  ${ENDIAN_SRCS_PATH}/endian.hh )

set ( DEBUG_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/debug )
set ( DEBUG_SRCS
  ${DEBUG_SRCS_PATH}/breakpoint.hh
  ${DEBUG_SRCS_PATH}/breakpoint_registry.hh
  ${DEBUG_SRCS_PATH}/breakpoint_registry.tcc
  ${DEBUG_SRCS_PATH}/breakpoint_registry_32.cc
  ${DEBUG_SRCS_PATH}/breakpoint_registry_64.cc
  ${DEBUG_SRCS_PATH}/profile.hh
  ${DEBUG_SRCS_PATH}/profile.tcc
  ${DEBUG_SRCS_PATH}/profile_32.cc
  ${DEBUG_SRCS_PATH}/profile_64.cc
  ${DEBUG_SRCS_PATH}/simple_register.hh
  ${DEBUG_SRCS_PATH}/stmt.hh
  ${DEBUG_SRCS_PATH}/stmt.tcc
  ${DEBUG_SRCS_PATH}/stmt_32.cc
  ${DEBUG_SRCS_PATH}/stmt_64.cc
  ${DEBUG_SRCS_PATH}/symbol.hh
  ${DEBUG_SRCS_PATH}/symbol.tcc
  ${DEBUG_SRCS_PATH}/symbol_32.cc
  ${DEBUG_SRCS_PATH}/symbol_64.cc
  ${DEBUG_SRCS_PATH}/symbol_table.hh
  ${DEBUG_SRCS_PATH}/symbol_table.tcc
  ${DEBUG_SRCS_PATH}/symbol_table_32.cc
  ${DEBUG_SRCS_PATH}/symbol_table_64.cc
  ${DEBUG_SRCS_PATH}/memory_access_type.hh
  ${DEBUG_SRCS_PATH}/watchpoint.hh
  ${DEBUG_SRCS_PATH}/watchpoint_registry.hh
  ${DEBUG_SRCS_PATH}/watchpoint_registry.tcc
  ${DEBUG_SRCS_PATH}/watchpoint_registry_32.cc
  ${DEBUG_SRCS_PATH}/watchpoint_registry_64.cc
  ${DEBUG_SRCS_PATH}/register.hh
)

set ( BLOB_SRCS_PATH
  ${DEBUG_SRCS_PATH}/blob )
set ( BLOB_SRCS
  ${BLOB_SRCS_PATH}/blob.hh
  ${BLOB_SRCS_PATH}/section.hh
  ${BLOB_SRCS_PATH}/segment.hh
  ${BLOB_SRCS_PATH}/blob.tcc
  ${BLOB_SRCS_PATH}/section.tcc
  ${BLOB_SRCS_PATH}/segment.tcc
  ${BLOB_SRCS_PATH}/blob32.cc
  ${BLOB_SRCS_PATH}/blob64.cc
  ${BLOB_SRCS_PATH}/section32.cc
  ${BLOB_SRCS_PATH}/section64.cc
  ${BLOB_SRCS_PATH}/segment32.cc
  ${BLOB_SRCS_PATH}/segment64.cc )

set ( DWARF_SRCS_PATH
  ${DEBUG_SRCS_PATH}/dwarf )
set ( DWARF_SRCS
  ${DWARF_SRCS_PATH}/abbrev.cc
  ${DWARF_SRCS_PATH}/abbrev.hh
  ${DWARF_SRCS_PATH}/addr_range.hh
  ${DWARF_SRCS_PATH}/addr_range.tcc
  ${DWARF_SRCS_PATH}/attr.cc
  ${DWARF_SRCS_PATH}/attr.hh
  ${DWARF_SRCS_PATH}/attr.tcc
  ${DWARF_SRCS_PATH}/call_frame_prog.hh
  ${DWARF_SRCS_PATH}/call_frame_prog.tcc
  ${DWARF_SRCS_PATH}/call_frame_vm.hh
  ${DWARF_SRCS_PATH}/call_frame_vm.tcc
  ${DWARF_SRCS_PATH}/cie.hh
  ${DWARF_SRCS_PATH}/cie.tcc
  ${DWARF_SRCS_PATH}/class.cc
  ${DWARF_SRCS_PATH}/class.hh
  ${DWARF_SRCS_PATH}/cu.hh
  ${DWARF_SRCS_PATH}/cu.tcc
  ${DWARF_SRCS_PATH}/die.hh
  ${DWARF_SRCS_PATH}/die.tcc
  ${DWARF_SRCS_PATH}/dwarf.hh
  ${DWARF_SRCS_PATH}/dwarf.tcc
  ${DWARF_SRCS_PATH}/dwarf32.cc
  ${DWARF_SRCS_PATH}/dwarf64.cc
  ${DWARF_SRCS_PATH}/encoding.cc
  ${DWARF_SRCS_PATH}/encoding.hh
  ${DWARF_SRCS_PATH}/expr_vm.hh
  ${DWARF_SRCS_PATH}/expr_vm.tcc
  ${DWARF_SRCS_PATH}/fde.hh
  ${DWARF_SRCS_PATH}/fde.tcc
  ${DWARF_SRCS_PATH}/filename.cc
  ${DWARF_SRCS_PATH}/filename.hh
  ${DWARF_SRCS_PATH}/fmt.hh
  ${DWARF_SRCS_PATH}/fwd.hh
  ${DWARF_SRCS_PATH}/leb128.cc
  ${DWARF_SRCS_PATH}/leb128.hh
  ${DWARF_SRCS_PATH}/loc.hh
  ${DWARF_SRCS_PATH}/loc.tcc
  ${DWARF_SRCS_PATH}/macinfo.hh
  ${DWARF_SRCS_PATH}/macinfo.tcc
  ${DWARF_SRCS_PATH}/ml.cc
  ${DWARF_SRCS_PATH}/ml.hh
  ${DWARF_SRCS_PATH}/pub.hh
  ${DWARF_SRCS_PATH}/pub.tcc
  ${DWARF_SRCS_PATH}/range.hh
  ${DWARF_SRCS_PATH}/range.tcc
  ${DWARF_SRCS_PATH}/stmt_prog.hh
  ${DWARF_SRCS_PATH}/stmt_prog.tcc
  ${DWARF_SRCS_PATH}/stmt_vm.hh
  ${DWARF_SRCS_PATH}/stmt_vm.tcc )

set ( ELF_SYMTAB_SRCS_PATH
  ${DEBUG_SRCS_PATH}/elf_symtab )
set ( ELF_SYMTAB_SRCS
  ${ELF_SYMTAB_SRCS_PATH}/elf_symtab.hh
  ${ELF_SYMTAB_SRCS_PATH}/elf_symtab.tcc
  ${ELF_SYMTAB_SRCS_PATH}/elf_symtab32.cc
  ${ELF_SYMTAB_SRCS_PATH}/elf_symtab64.cc )

set ( ELF_LOADER_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/loader/elf_loader )
set ( ELF_LOADER_SRCS
  ${ELF_LOADER_SRCS_PATH}/elf32.h
  ${ELF_LOADER_SRCS_PATH}/elf32_loader.cc
  ${ELF_LOADER_SRCS_PATH}/elf32_loader.hh
  ${ELF_LOADER_SRCS_PATH}/elf64.h
  ${ELF_LOADER_SRCS_PATH}/elf64_loader.cc
  ${ELF_LOADER_SRCS_PATH}/elf64_loader.hh
  ${ELF_LOADER_SRCS_PATH}/elf_common.h
  ${ELF_LOADER_SRCS_PATH}/elf_loader.hh
  ${ELF_LOADER_SRCS_PATH}/elf_loader.tcc )

set ( LINUX_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/os/linux )
set ( LINUX_SRCS
  ${LINUX_SRCS_PATH}/linux.cc )
add_library ( unisim_util_os_linux
  ${LINUX_SRCS}
  ${ENDIAN_SRCS}
  ${DEBUG_SRCS}
  ${BLOB_SRCS}
  ${DWARF_SRCS}
  ${ELF_SYMTAB_SRCS}
  ${ELF_LOADER_SRCS} )

set ( TEST_SRCS_PATH
  ${UNISIM_LIB_PATH}/unisim/util/os/linux )
set ( TEST_SRCS
  ${TEST_SRCS_PATH}/test.cc )
add_executable ( unisim_util_os_linux_test
  ${TEST_SRCS} )
target_link_libraries ( unisim_util_os_linux_test
  unisim_util_os_linux )
