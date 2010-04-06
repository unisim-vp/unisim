IF ( APPLE )
	SET ( TARGET_ARCH macosx-x86 )
ELSEIF ( MINGW32 )
	SET ( TARGET_ARCH mingw32 )
ELSE ( MINGW32 )
	set ( TARGET_ARCH linux )
ENDIF ( APPLE )

# IF ( build_osci_systemc )
# 	IF ( NOT build_osci_systemc_src )
# 		MESSAGE ( FATAL_ERROR
# 			"When building the osci systemc library, we require the original sources of SystemC 2.2.0 (use -Dbuild_osci_systemc_src=<path to source>)" )
# 	ENDIF ( NOT build_osci_systemc_src )
# ENDIF ( build_osci_systemc )

set ( with_osci_systemc "" CACHE FILEPATH 
	"Path to the SystemC 2 installation." ) 

IF ( with_osci_systemc )
	SET ( OSCI_SYSTEMC_INCLUDE_HINT ${with_osci_systemc}/include )
	SET ( OSCI_SYSTEMC_LIB_HINT ${with_osci_systemc}/lib-${TARGET_ARCH} )
ELSE ( with_osci_systemc )
# 	IF ( build_osci_systemc )
# 		SET ( OSCI_SYSTEMC_INCLUDE_HINT ${CMAKE_BINARY_DIR}/osci/systemc/include )
# 		SET ( OSCI_SYSTEMC_LIB_HINT ${CMAKE_BINARY_DIR}/osci/systemc/lib )
# 	ELSE ( build_osci_systemc )
		SET ( OSCI_SYSTEMC_INCLUDE_HINT /usr/include )
		SET ( OSCI_SYSTEMC_LIB_HINT /usr/lib-${TARGET_ARCH} )
# 	ENDIF ( build_osci_systemc )
ENDIF ( with_osci_systemc )

FIND_PATH ( OSCI_SYSTEMC_INCLUDE_DIR systemc.h
	HINTS
		${OSCI_SYSTEMC_INCLUDE_HINT}
	)

FIND_LIBRARY ( OSCI_SYSTEMC_LIBRARIES 
	NAMES 
		systemc
	HINTS
		${OSCI_SYSTEMC_LIB_HINT}
	)

INCLUDE ( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS ( 
	OSCISystemC 
	DEFAULT_MSG 
	OSCI_SYSTEMC_LIBRARIES 
	OSCI_SYSTEMC_INCLUDE_DIR )
MARK_AS_ADVANCED ( OSCI_SYSTEMC_LIBRARIES OSCI_SYSTEMC_INCLUDE_DIR )
IF ( NOT OSCISYSTEMC_FOUND )
	MESSAGE ( FATAL_ERROR
		"Could not find SystemC library, please use -Dwith_osci_systemc=<path> to indicate its location." )
ENDIF ( NOT OSCISYSTEMC_FOUND )
