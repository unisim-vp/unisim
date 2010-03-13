cmake_minimum_required ( VERSION 2.8 )
project ( unisim_arm9emu )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh CACHE STRING "Extensions of the source files" )
	
set ( CXX_INCLUDES iostream list string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES getopt stdlib signal )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( arm9emu_subdirs
	unisim/kernel/service
	unisim/component/tlm2/processor/arm/arm9tdmi
	unisim/component/tlm2/interrupt
	unisim/component/tlm2/memory/ram
	unisim/service/time/sc_time
	unisim/service/time/host_time
	unisim/service/debug/gdb_server
	unisim/service/debug/inline_debugger
	unisim/service/loader/elf_loader
	unisim/service/loader/linux_loader
	unisim/service/os/linux_os/linux_os_32
	)
foreach ( dir ${arm9emu_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${OSCI_SYSTEMC_INCLUDE_DIR} ${OSCI_TLM2_INCLUDE_DIR})

add_executable ( arm9emu 
	${CMAKE_SOURCE_DIR}/main.cc )
target_link_libraries ( arm9emu
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel__service
	unisim__component__cxx__processor__arm
	unisim__component__tlm2__processor__arm__arm9tdmi
	unisim__component__tlm2__interrupt
	unisim__component__tlm2__memory__ram
	unisim__service__time__host_time
	unisim__service__time__sc_time
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__os__linux_os__linux_os_32
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	)

# set_source_files_properties (
#	${CMAKE_SOURCE_DIR}/main.cc
#	PROPERTIES
#		COMPILE_FLAGS "-I${OSCI_SYSTEMC_INCLUDE_DIR} -I${OSCI_TLM2_INCLUDE_DIR}"
#	)
include ( CPack )

