function (clib_stdlib_h)
	include (CheckIncludeFile)

	check_include_file (stdlib.h HAVE_CLIB_STDLIB_H)
	if (NOT ${HAVE_CLIB_STDLIB_H})
		message ( SEND_ERROR "Could not find \"stdlib.h\"" )
	endif (NOT ${HAVE_CLIB_STDLIB_H})
endfunction (clib_stdlib_h)

