function (clib_assert_h)
	include (CheckIncludeFile)

	check_include_file (assert.h HAVE_CLIB_ASSERT_H)
	if (NOT ${HAVE_CLIB_ASSERT_H})
		message ( SEND_ERROR "Could not find \"assert.h\"" )
	endif (NOT ${HAVE_CLIB_ASSERT_H})
endfunction (clib_assert_h)

