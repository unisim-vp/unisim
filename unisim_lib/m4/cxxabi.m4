## UNISIM_CHECK_CXXABI
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_CXXABI], [
    # Check for execinfo.h
    AC_CHECK_HEADER(execinfo.h, broken_cxxabi=no, broken_cxxabi=yes)
    AC_CHECK_HEADER(cxxabi.h, , broken_cxxabi=yes)
    AC_CHECK_LIB(c, backtrace, , broken_cxxabi=yes)
    AC_CHECK_LIB(c, backtrace_symbols, , broken_cxxabi=yes)
    #AX_CXX_CHECK_LIB(stdc++, [abi::__cxa_demangle()], , broken_cxxabi=yes)

    if test "$broken_cxxabi" == "yes"; then
        AC_MSG_NOTICE([C++ ABI functions not found. No kernel debug will be available.])
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
