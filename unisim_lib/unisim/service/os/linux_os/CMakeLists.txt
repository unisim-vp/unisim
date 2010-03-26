message ( STATUS "Checking unisim::service::os::linux_os" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

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

set ( CXX_INCLUDES map fstream sstream iostream string stdexcept )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( linux_os_subdirs
	unisim/kernel/service
	unisim/kernel/logger
	unisim/service/interfaces/linux_os
	unisim/service/interfaces/cpu_linux_os
	unisim/service/interfaces/loader
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_injection
	unisim/service/interfaces/registers
	unisim/util/endian
	unisim/util/debug
	)
foreach ( dir ${linux_os_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__os__linux_os
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/service/os/linux_os/linux_os.hh
		${CMAKE_SOURCE_DIR}/unisim/service/os/linux_os/linux_os_exception.hh
		${CMAKE_SOURCE_DIR}/unisim/service/os/linux_os/linux_os.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/os/linux_os/linux_os_exception.cc
		${CMAKE_SOURCE_DIR}/unisim/service/os/linux_os/linux_os_32.cc
	)

target_link_libraries ( unisim__service__os__linux_os
	unisim__kernel__service
	unisim__kernel__logger
	unisim__util__endian
	unisim__util__debug
	)

add_dependencies ( unisim__service__os__linux_os
	unisim__service__interfaces__linux_os
	unisim__service__interfaces__cpu_linux_os
	unisim__service__interfaces__loader
	unisim__service__interfaces__memory
	unisim__service__interfaces__memory_injection
	unisim__service__interfaces__registers
	)

message ( STATUS "Checking unisim::service::os::linux_os - done" )

