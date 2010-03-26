message ( STATUS "Checking unisim::component::tlm::fsb::snooping_bus" )

include ( CheckIncludeFile )
include ( CheckIncludeFileCXX )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )

set ( C_INCLUDES inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
        check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
        if ( NOT HAVE_${C_INCLUDE}_H )
                message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
        endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string list sstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
        check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
        if ( NOT HAVE_${CXX_INCLUDE} )
                message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
        endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( tlm_fsb_snooping_bus_deps
	unisim/kernel/tlm
	unisim/kernel/service
	unisim/kernel/logger
	unisim/component/tlm/message/snooping_fsb
	unisim/service/interfaces/memory
	unisim/util/garbage_collector
	)

foreach ( dir ${tlm_fsb_snooping_bus_deps} )
	unisim_include ( ${dir} )
endforeach ( dir ${tlm_debug_deps} )

include_directories ( 
	${CMAKE_SOURCE_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_library ( unisim__component__tlm__fsb__snooping_bus
	STATIC
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/fsb/snooping_bus/bus.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/fsb/snooping_bus/bus.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/fsb/snooping_bus/bus_addr32_size32_procs1.cc
	)

add_dependencies ( unisim__component__tlm__fsb__snooping_bus
	unisim__kernel__tlm
	unisim__kernel__service
	unisim__kernel__logger
	unisim__component__tlm__message__snooping_fsb
	unisim__service__interfaces__memory
	unisim__util__garbage_collector
	)

target_link_libraries ( unisim__component__tlm__fsb__snooping_bus
	${OSCI_SYSTEMC_LIB}
	unisim__kernel__tlm
	unisim__kernel__service
	unisim__kernel__logger
	unisim__util__garbage_collector
	)

message ( STATUS "Checking unisim::component::tlm::fsb::snooping_bus- done" )


