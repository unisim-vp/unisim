AC_DEFUN([LIBIEEE1666_CHECK_CXXABI], [
    # Check for execinfo.h
    AC_CHECK_HEADER(execinfo.h, broken_cxxabi=no, broken_cxxabi=yes)
    AC_CHECK_HEADER(cxxabi.h, , broken_cxxabi=yes)
    LIBIEEE1666_CHECK_LIB(c, backtrace, $1, , broken_cxxabi=yes)
    LIBIEEE1666_CHECK_LIB(c, backtrace_symbols, $1, , broken_cxxabi=yes)

    if test "$broken_cxxabi" == "yes"; then
        AC_MSG_NOTICE([C++ ABI functions not found. No backtrace will be available while simulation kernel debugging.])
    else
        AC_DEFINE([HAVE_CXXABI], [], [Whether C++ ABI functions are available])
        case "${CXX}" in
           *g++)
               LDFLAGS="${LDFLAGS} -rdynamic"
               ;;
           *)
               ;;
        esac
    fi
])
