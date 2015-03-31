AC_DEFUN([LIBIEEE1666_CHECK_BOOST_COROUTINE], [
    AC_CHECK_HEADERS([boost/coroutine/coroutine.hpp],,\
	AC_MSG_ERROR([boost coroutine headers not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
    LIBS="-lboost_coroutine -lboost_context ${LIBS}"
])
