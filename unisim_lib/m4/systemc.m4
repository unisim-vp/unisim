## UNISIM_CHECK_SYSTEMC
## Checks if the systemc library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
	# Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
	CXX_COMP=`basename $CXX`
	case "$host" in
		x386-apple-* | i386-apple-*)
			SYSTEMC_TARGET_ARCH="macosx386"
			;;
		powerpc-apple-macosx*)
			SYSTEMC_TARGET_ARCH="macosx"
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
		amd64*freebsd* | x86_64*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd64"
			;;
		*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd"
			;;
		*cygwin*)
			SYSTEMC_TARGET_ARCH="cygwin"
			;;
		amd64*mingw* | x86_64*mingw*)
			SYSTEMC_TARGET_ARCH="mingw64"
			;;
		*mingw*)
			SYSTEMC_TARGET_ARCH="mingw"
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
	esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [SystemC library to use (will be completed with /include and /lib-${SYSTEMC_TARGET_ARCH})]))
    if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LDFLAGS=${LDFLAGS}" -L$with_systemc/lib-${SYSTEMC_TARGET_ARCH}"
	CPPFLAGS=${CPPFLAGS}" -I$with_systemc/include"
    fi
	CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default search path.]))

    # Check for function 'sc_start' in libsystemc.a
	unisim_check_systemc_save_LIBS="${LIBS}"
	LIBS="-lsystemc ${LIBS}"
	AC_MSG_CHECKING([for sc_start in -lsystemc])
	AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <systemc.h>
int sc_main(int argc, char **argv)
{
	sc_start();
	return 0;
}
extern "C"
int main(int argc, char *argv[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}]])],
		LIBS="${unisim_check_systemc_save_LIBS}"; AC_MSG_RESULT([yes]); [broken_systemc=no],
		LIBS="${unisim_check_systemc_save_LIBS}"; AC_MSG_RESULT([no]); [broken_systemc=yes])

    #AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_systemc=yes)

    if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default search path.])
    else
	LIBS="-lsystemc ${LIBS}"
    fi
])
