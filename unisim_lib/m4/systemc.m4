## UNISIM_CHECK_SYSTEMC
## Checks if the systemc library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
    # Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
    CXX_COMP=`basename $CXX`
    case "$host" in
	*powerpc*macosx* | *powerpc*darwin*)
	    SYSTEMC_TARGET_ARCH="macosx"
	;;
	*86*macosx* | *86*darwin*)
	    SYSTEMC_TARGET_ARCH="macosx-x86"
	;;
	sparc-sun-solaris*)
	    case "$CXX_COMP" in
	    CC)
		SYSTEMC_TARGET_ARCH="sparcOS5"
        	;;
	    c++ | g++)
		SYSTEMC_TARGET_ARCH="gccsparcOS5"
		;;
	    esac
	    ;;
	x86_64*linux*)
	    SYSTEMC_TARGET_ARCH="linux64"
    	    ;;
	*linux*)
    	    SYSTEMC_TARGET_ARCH="linux"
    	    ;;
        *cygwin*)
    	    SYSTEMC_TARGET_ARCH="cygwin"
    	    ;;
	*hpux11*)
    	    case "$CXX_COMP" in
        	aCC)
            	    SYSTEMC_TARGET_ARCH="hpux11"
            	    ;;
        	c++ | g++)
            	    SYSTEMC_TARGET_ARCH="gcchpux11"
            	    ;;
    	    esac
    	    ;;
	*mingw32*)
    	    SYSTEMC_TARGET_ARCH="mingw32"
    	    ;;
    esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [systemc library to use (will be completed with /include and /lib-${SYSTEMC_TARGET_ARCH})]))
    if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LDFLAGS=${LDFLAGS}" -L$with_systemc/lib-${SYSTEMC_TARGET_ARCH}"
	CPPFLAGS=${CPPFLAGS}" -I$with_systemc/include"
    fi
	CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))

    # Check for function 'main' in libsystemc.a
    AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_system=yes)

    if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default includes search path.])
    else
	LIBS="-lsystemc ${LIBS}"
    fi
])