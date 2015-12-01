AC_INIT([libieee1666], [0.0.0])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([subdir-objects tar-pax])

AC_PATH_PROGS(SH, sh)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL # must be *before* AC_PROG_LIBTOOL
AC_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_PROG_LN_S

AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

###### CHECK FOR STANDARD C HEADERS ###########################################

AC_CHECK_HEADERS([iostream stdio.h math.h stdlib.h string.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

###### CHECK FOR STANDARD C++ HEADERS #########################################
AC_CHECK_HEADERS([iostream],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

CXX_COMP=`basename $CXX`
case "${host}" in
	x386-apple-* | i386-apple-*)
		IEEE1666_TARGET_ARCH="macosx386"
		;;
	powerpc-apple-macosx*)
		IEEE1666_TARGET_ARCH="macosx"
		;;
	sparc-sun-solaris*)
		case "$CXX_COMP" in
			CC)
			IEEE1666_TARGET_ARCH="sparcOS5"
			;;
			c++ | g++)
			IEEE1666_TARGET_ARCH="gccsparcOS5"
			;;
		esac
		;;
	x86_64*linux*)
		IEEE1666_TARGET_ARCH="linux64"
		;;
	*86*linux*)
		IEEE1666_TARGET_ARCH="linux"
		;;
	*arm*linux*)
		IEEE1666_TARGET_ARCH="linux-arm"
		;;
	powerpc*linux*)
		IEEE1666_TARGET_ARCH="linux-powerpc"
		;;
	amd64*freebsd* | x86_64*freebsd*)
		IEEE1666_TARGET_ARCH="freebsd64"
		;;
	*freebsd*)
		IEEE1666_TARGET_ARCH="freebsd"
		;;
	*cygwin*)
		IEEE1666_TARGET_ARCH="cygwin"
		;;
	amd64*mingw* | x86_64*mingw*)
		IEEE1666_TARGET_ARCH="mingw64"
		;;
	*mingw*)
		IEEE1666_TARGET_ARCH="mingw"
		;;
	*hpux11*)
		case "$CXX_COMP" in
		aCC)
			IEEE1666_TARGET_ARCH="hpux11"
			;;
		c++ | g++)
			IEEE1666_TARGET_ARCH="gcchpux11"
			;;
		esac
		;;
esac

AC_SUBST(IEEE1666_TARGET_ARCH)
AC_SUBST(VERSION)
LIBIEEE1666_SC_THREAD_PROCESS_BACKEND
LIBIEEE1666_CHECK_CXXABI(main)

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile systemc.pc])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
