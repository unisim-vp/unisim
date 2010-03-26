message ( STATUS "Checking unisim::component::tlm::bridge::snooping_fsb_to_mem" )

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

set ( CXX_INCLUDES cmath )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
        check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
        if ( NOT HAVE_${CXX_INCLUDE} )
                message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
        endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( tlm_bridge_deps
	unisim/kernel/tlm
	unisim/kernel/logger
	unisim/component/tlm/message/snooping_fsb
	unisim/component/tlm/message/memory
	unisim/service/interfaces/memory
	unisim/util/garbage_collector
	)

foreach ( dir ${tlm_bridge_deps} )
	unisim_include ( ${dir} )
endforeach ( dir ${tlm_bridge_deps} )

include_directories ( 
	${CMAKE_SOURCE_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR}
	)

add_library ( unisim__component__tlm__bridge__snooping_fsb_to_mem
	STATIC
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/bridge/snooping_fsb_to_mem/bridge.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/bridge/snooping_fsb_to_mem/bridge.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/bridge/snooping_fsb_to_mem/config.hh
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/bridge/snooping_fsb_to_mem/addr32_burstsize32.cc
		${CMAKE_SOURCE_DIR}/unisim/component/tlm/bridge/snooping_fsb_to_mem/addr32_burstsize32_debug.cc
	)

add_dependencies ( unisim__component__tlm__bridge__snooping_fsb_to_mem
	unisim__kernel__tlm
	unisim__kernel__logger
	unisim__component__tlm__message__snooping_fsb
	unisim__component__tlm__message__memory
	unisim__service__interfaces__memory
	unisim__util__garbage_collector
	)

target_link_libraries ( unisim__component__tlm__bridge__snooping_fsb_to_mem
	${OSCI_SYSTEMC_LIB}
	unisim__kernel__tlm
	unisim__kernel__logger
	unisim__util__garbage_collector
	)

message ( STATUS "Checking unisim::component::tlm::bridge::snooping_fsb_to_mem - done" )




