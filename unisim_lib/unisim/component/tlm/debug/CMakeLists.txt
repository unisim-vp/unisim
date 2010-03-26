message ( STATUS "Checking unisim::component::tlm::debug" )

include ( CheckIncludeFileCXX )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
        check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
        if ( NOT HAVE_${CXX_INCLUDE} )
                message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
        endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( tlm_debug_deps
	unisim/kernel/tlm
	unisim/kernel/service
	unisim/kernel/logger
	unisim/util/garbage_collector
	)

foreach ( dir ${tlm_debug_deps} )
	unisim_include ( ${dir} )
endforeach ( dir ${tlm_debug_deps} )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__component__tlm__debug
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/debug/transaction_spy.hh
	)

target_link_libraries ( unisim__component__tlm__debug
	unisim__kernel__tlm
	unisim__kernel__service
	unisim__kernel__logger
	unisim__util__garbage_collector
	)

add_dependencies ( unisim__component__tlm__debug
	unisim__kernel__tlm
	unisim__kernel__service
	unisim__kernel__logger
	unisim__util__garbage_collector
	)

message ( STATUS "Checking unisim::component::tlm::debug - done" )

