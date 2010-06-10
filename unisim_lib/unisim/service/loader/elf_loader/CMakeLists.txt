message ( STATUS "Checking unisim::service::loader::elf_loader" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes stdlib string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES iosfwd iostream fstream vector map sstream limits )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( elf_loader_subdirs
	unisim/kernel
	unisim/service/interfaces/memory
	unisim/service/interfaces/loader
	unisim/service/interfaces/symbol_table_lookup
	unisim/service/interfaces/stmt_lookup
	unisim/util/endian
	unisim/util/debug
	)
foreach ( dir ${elf_loader_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__loader__elf_loader
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf_common.h
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf32.h
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf64.h
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf_loader.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf32_loader.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf64_loader.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf_loader.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf32_loader.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/elf64_loader.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf_32.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf_64.cc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf.hh
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/loader/elf_loader/dwarf.cc
	)

target_link_libraries ( unisim__service__loader__elf_loader
	unisim__kernel
	unisim__util__endian
	unisim__util__debug
	)

add_dependencies ( unisim__service__loader__elf_loader
	unisim__kernel
	unisim__service__interfaces__memory
	unisim__service__interfaces__loader
	unisim__service__interfaces__symbol_table_lookup
	unisim__util__endian
	unisim__util__debug
	)

message ( STATUS "Checking unisim::service::loader::elf_loader - done" )


