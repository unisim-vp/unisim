message ( STATUS "Checking unisim::service::debug::inline_debugger" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes stdio stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string iostream list )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( inline_debugger_subdirs
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/debug_control
	unisim/service/interfaces/registers
	unisim/service/interfaces/trap_reporting
	unisim/service/interfaces/disassembly
	unisim/service/interfaces/symbol_table_lookup
	unisim/util/debug
	unisim/util/endian
	unisim/kernel/service
	)
foreach ( dir ${inline_debugger_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__debug__inline_debugger
		${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger.hh
		${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger_32.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger_64.cc
	)

target_link_libraries ( unisim__service__debug__inline_debugger 
	unisim__util__debug )

if ( UNIX )
	include ( FindCurses )
	if ( CURSES_FOUND )
		target_link_libraries ( unisim__service__debug__inline_debugger
			${CURSES_LIBRARIES} )
	endif ( CURSES_FOUND )
	set ( CMAKE_REQUIRED_FLAGS "-include stdio.h" )
	check_include_file ( editline/readline.h HAVE_editline_readline_H )
	if ( HAVE_editline_readline_H )
		find_library ( LIBEDIT
			edit )
		if ( LIBEDIT )
			set_target_properties (
				unisim__service__debug__inline_debugger
				PROPERTIES
					COMPILE_DEFINITIONS HAVE_LIBEDIT
				)
			target_link_libraries ( unisim__service__debug__inline_debugger
				${LIBEDIT} )	
		endif ( LIBEDIT )
	endif ( HAVE_editline_readline_H )
endif ( UNIX )

set_source_files_properties (
	${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger.hh
	${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger/inline_debugger.tcc
	PROPERTY
		HEADER_FILE_ONLY TRUE
	)
	
message ( STATUS "Checking unisim::service::debug::inline_debugger - done" )



