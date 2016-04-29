AC_INIT([libieee1666], [0.0.0], [Gilles Mouchard <gilles.mouchard@cea.fr>], [], [http://unisim-vp.org])
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
PKG_PROG_PKG_CONFIG

USER_CXXFLAGS="${CXXFLAGS}"     # save user's CXXFLAGS

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

########################################### configure options  ###############################################

AC_ARG_ENABLE([posix-guarded-stack],
	AC_HELP_STRING([--enable-posix-guarded-stack], [guarded stack support for POSIX [default=yes]]), , enable_posix_guarded_stack=yes)

if test "x${enable_posix_guarded_stack}" = "xyes"; then
	LIBIEEE1666_ENABLE_POSIX_GUARDED_STACK=1
else
	LIBIEEE1666_ENABLE_POSIX_GUARDED_STACK=0
fi

AC_ARG_ENABLE([fcontext],
	AC_HELP_STRING([--enable-fcontext], [Boost::Context (fcontext) coroutine system support [default=yes]]), , enable_fcontext=yes)

if test "x${enable_fcontext}" = "xyes"; then
	LIBIEEE1666_ENABLE_FCONTEXT=1
else
	LIBIEEE1666_ENABLE_FCONTEXT=0
fi

AC_ARG_ENABLE([ucontext],
	AC_HELP_STRING([--enable-ucontext], [ucontext coroutine system support [default=no]]), , enable_ucontext=no)

if test "x${enable_ucontext}" = "xyes"; then
	LIBIEEE1666_ENABLE_UCONTEXT=1
else
	LIBIEEE1666_ENABLE_UCONTEXT=0
fi

AC_ARG_ENABLE([pthread],
	AC_HELP_STRING([--enable-pthread], [POSIX thread coroutine system support [default=yes]]), , enable_pthread=yes)

if test "x${enable_pthread}" = "xyes"; then
	LIBIEEE1666_ENABLE_PTHREAD=1
else
	LIBIEEE1666_ENABLE_PTHREAD=0
fi

AC_ARG_ENABLE([windows-guarded-stack],
	AC_HELP_STRING([--enable-windows-guarded-stack], [guarded stack support for Windows [default=yes]]), , enable_windows_guarded_stack=yes)

if test "x${enable_windows_guarded_stack}" = "xyes"; then
	LIBIEEE1666_ENABLE_WINDOWS_GUARDED_STACK=1
else
	LIBIEEE1666_ENABLE_WINDOWS_GUARDED_STACK=0
fi

AC_ARG_ENABLE([backtrace],
	AC_HELP_STRING([--enable-backtrace], [backtrace support (for hacking) [default=no]]), , enable_backtrace=no)

if test "x${enable_backtrace}" = "xyes"; then
	LIBIEEE1666_ENABLE_BACKTRACE=1
else
	LIBIEEE1666_ENABLE_BACKTRACE=0
fi

AC_ARG_ENABLE([valgrind],
	AC_HELP_STRING([--enable-valgrind], [valgrind support [default=yes]]), , enable_valgrind=yes)

if test "x${enable_valgrind}" = "xyes"; then
	LIBIEEE1666_ENABLE_VALGRIND=1
else
	LIBIEEE1666_ENABLE_VALGRIND=0
fi

####################################### helper macro for testing a function ###################################

AC_DEFUN([LIBIEEE1666_CHECK_FUN], [
	AC_MSG_CHECKING([for $1])
	AC_LINK_IFELSE([AC_LANG_SOURCE([[
$2
int main(int argc, char **argv) { $3; return 0; }
		]])],
		$4; AC_MSG_RESULT([yes]),
		$5; AC_MSG_RESULT([no])
	)
])

############################## Check for POSIX.1-2001 compliance ######################################

AC_CHECK_HEADER(unistd.h, LIBIEEE1666_UNISTD_H=1 , LIBIEEE1666_UNISTD_H=0)

if test ${LIBIEEE1666_UNISTD_H} -eq 1; then
	AC_MSG_CHECKING([for POSIX.1-2001 compliance]) 
	AC_EGREP_CPP(
		yes,
		[AC_LANG_SOURCE(
			[[
#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#ifdef _POSIX_VERSION
#if _POSIX_VERSION >= 200112L
yes
#endif
#endif
			]]
		)],
		[LIBIEEE1666_POSIX=1],
		[LIBIEEE1666_POSIX=0])
else
	LIBIEEE1666_POSIX=0
fi

if test ${LIBIEEE1666_POSIX} -eq 1; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

############################## Check for Windows ######################################

AC_MSG_CHECKING([for Windows]) 
AC_EGREP_CPP(yes,
	[AC_LANG_SOURCE(
		[[
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
yes
#endif
		]]
	)],
	LIBIEEE1666_WINDOWS=1,
	LIBIEEE1666_WINDOWS=0)

if test ${LIBIEEE1666_WINDOWS} -eq 1; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

######################################### Checking for windows.h #####################################################

if test ${LIBIEEE1666_WINDOWS} -eq 1; then
	AC_CHECK_HEADER(windows.h, LIBIEEE1666_WINDOWS_H=1, LIBIEEE1666_WINDOWS_H=0)
else
	LIBIEEE1666_WINDOWS_H=0
fi
 
############################## Check for sys/mman.h, mmap, munmap and mprotect  ######################################

if test ${LIBIEEE1666_POSIX} -eq 1; then
	AC_CHECK_HEADER(sys/mman.h, LIBIEEE1666_SYS_MMAN_H=1, LIBIEEE1666_SYS_MMAN_H=0)

	if test ${LIBIEEE1666_UNISTD_H} -eq 1; then
		LIBIEEE1666_CHECK_FUN([sysconf], [#include <unistd.h>], [sysconf(0)], [LIBIEEE1666_SYSCONF=1], [LIBIEEE1666_SYSCONF=0])
	else
		LIBIEEE1666_SYSCONF=0
	fi
	if test ${LIBIEEE1666_SYS_MMAN_H} -eq 1; then
		LIBIEEE1666_CHECK_FUN([mmap], [#include <sys/mman.h>], [mmap(0,0,0,0,0,0)], [LIBIEEE1666_MMAP=1], [LIBIEEE1666_MMAP=0])
		LIBIEEE1666_CHECK_FUN([munmap], [#include <sys/mman.h>], [munmap(0,0)], [LIBIEEE1666_MUNMAP=1], [LIBIEEE1666_MUNMAP=0])
		LIBIEEE1666_CHECK_FUN([mprotect], [#include <sys/mman.h>], [mprotect(0,0,0)], [LIBIEEE1666_MPROTECT=1], [LIBIEEE1666_MPROTECT=0])
	else
		# sys/mman.h is missing
		LIBIEEE1666_MMAP=0
		LIBIEEE1666_MUNMAP=0
		LIBIEEE1666_MPROTECT=0
	fi
else
	LIBIEEE1666_MMAP=0
	LIBIEEE1666_MUNMAP=0
	LIBIEEE1666_MPROTECT=0
fi

#################### Check for GetSystemInfo, VirtualAlloc, VirtualProtect and VirtualFree #################

if test ${LIBIEEE1666_WINDOWS} -eq 1 && test ${LIBIEEE1666_WINDOWS_H} -eq 1; then
	LIBIEEE1666_CHECK_FUN([GetSystemInfo], [#include <windows.h>], [GetSystemInfo(0)], [LIBIEEE1666_GET_SYSTEM_INFO=1], [LIBIEEE1666_GET_SYSTEM_INFO=0])
	LIBIEEE1666_CHECK_FUN([VirtualAlloc], [#include <windows.h>], [VirtualAlloc(0,0,0,0)], [LIBIEEE1666_VIRTUAL_ALLOC=1], [LIBIEEE1666_VIRTUAL_ALLOC=0])
	LIBIEEE1666_CHECK_FUN([VirtualProtect], [#include <windows.h>], [VirtualProtect(0,0,0,0)], [LIBIEEE1666_VIRTUAL_PROTECT=1], [LIBIEEE1666_VIRTUAL_PROTECT=0])
	LIBIEEE1666_CHECK_FUN([VirtualFree], [#include <windows.h>], [VirtualFree(0,0,0)], [LIBIEEE1666_VIRTUAL_FREE=1], [LIBIEEE1666_VIRTUAL_FREE=0])
else
	LIBIEEE1666_GET_SYSTEM_INFO=0
	LIBIEEE1666_VIRTUAL_ALLOC=0
	LIBIEEE1666_VIRTUAL_PROTECT=0
	LIBIEEE1666_VIRTUAL_FREE=0
fi

######################################### check boost ####################################

AX_BOOST_BASE([1.0.0], AC_MSG_NOTICE([boost found]), AC_MSG_ERROR([boost not found]))

##################################### check boost context ################################

AX_BOOST_BASE([1.51.0], LIBIEEE1666_BOOST_1_51=1, LIBIEEE1666_BOOST_1_51=0)

if test ${LIBIEEE1666_BOOST_1_51} -eq 1; then
	AC_MSG_NOTICE([boost >= 1.51 found])
	CPPFLAGS="${BOOST_CPPFLAGS} ${CPPFLAGS}"
	LDFLAGS="${BOOST_LDFLAGS} ${LDFLAGS}"
	AX_BOOST_CONTEXT
	LIBS="${BOOST_CONTEXT_LIB} ${LIBS}"
else
	AC_MSG_NOTICE([boost >= 1.51 not found])
fi

if test "x${ax_cv_boost_context}" = "xyes"; then
	LIBIEEE1666_BOOST_CONTEXT=1
else
	LIBIEEE1666_BOOST_CONTEXT=0
fi

################################## check POSIX threads ##################################

AX_PTHREAD(LIBIEEE1666_PTHREAD=1, LIBIEEE1666_PTHREAD=0)

if test ${LIBIEEE1666_PTHREAD} -eq 1; then
	AC_MSG_NOTICE([POSIX threads found])
else
	AC_MSG_NOTICE([POSIX threads not found])
fi

################################# check for ucontext #######################################

AC_CHECK_HEADER(ucontext.h, LIBIEEE1666_UCONTEXT_H=1, LIBIEEE1666_UCONTEXT_H=0)

if test ${LIBIEEE1666_UCONTEXT_H} -eq 1; then
	LIBIEEE1666_CHECK_FUN([getcontext], [#include <ucontext.h>], [getcontext(0)], [LIBIEEE1666_GETCONTEXT=1], [LIBIEEE1666_GETCONTEXT=0])
	LIBIEEE1666_CHECK_FUN([makecontext], [#include <ucontext.h>], [makecontext(0, 0, 0)], [LIBIEEE1666_MAKECONTEXT=1], [LIBIEEE1666_MAKECONTEXT=0])
	LIBIEEE1666_CHECK_FUN([swapcontext], [#include <ucontext.h>], [swapcontext(0, 0)], [LIBIEEE1666_SWAPCONTEXT=1], [LIBIEEE1666_SWAPCONTEXT=0])
else
	LIBIEEE1666_GETCONTEXT=0
	LIBIEEE1666_MAKECONTEXT=0
	LIBIEEE1666_SWAPCONTEXT=0
fi

############################# Check for backtrace support ###################################

AC_CHECK_HEADER(execinfo.h, LIBIEEE1666_EXECINFO_H=1, LIBIEEE1666_EXECINFO_H=0)
AC_CHECK_HEADER(cxxabi.h, , LIBIEEE1666_CXXABI_H=1, LIBIEEE1666_CXXABI_H=0)
AC_CHECK_LIB(c, backtrace, LIBIEEE1666_BACKTRACE_FUNCTION=1, LIBIEEE1666_BACKTRACE_FUNCTION=0)
AC_CHECK_LIB(c, backtrace_symbols, LIBIEEE1666_BACKTRACE_SYMBOLS_FUNCTION=1, LIBIEEE1666_BACKTRACE_SYMBOLS_FUNCTION=0)

######################## Check for valgrind "client request mechanism" ##################################

AC_CHECK_HEADER(valgrind/valgrind.h, LIBIEEE1666_VALGRIND_H=1, LIBIEEE1666_VALGRIND_H=0)

################################## host specific stuff #######################################

if test ${LIBIEEE1666_WINDOWS} -eq 1; then
	LDFLAGS="-no-undefined ${LDFLAGS}"
fi

######################## activate system dependent features if enabled and available ##########################

LIBIEEE1666_SYSDEPS=

if test ${LIBIEEE1666_ENABLE_POSIX_GUARDED_STACK} -eq 1 && test ${LIBIEEE1666_MMAP} -eq 1 && test ${LIBIEEE1666_MUNMAP} -eq 1 && test ${LIBIEEE1666_MPROTECT} -eq 1; then
	LIBIEEE1666_POSIX_GUARDED_STACK=1
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/posix_guarded_stack.lo"
else
	LIBIEEE1666_POSIX_GUARDED_STACK=0
fi

if test ${LIBIEEE1666_ENABLE_FCONTEXT} -eq 1 && test ${LIBIEEE1666_BOOST_CONTEXT} -eq 1; then
	LIBIEEE1666_FCONTEXT=1
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/fcontext_coroutine.lo"
else
	LIBIEEE1666_FCONTEXT=0
fi

if test ${LIBIEEE1666_ENABLE_UCONTEXT} -eq 1 && test ${LIBIEEE1666_GETCONTEXT} -eq 1 && test ${LIBIEEE1666_MAKECONTEXT} -eq 1 && test ${LIBIEEE1666_SWAPCONTEXT} -eq 1; then
	LIBIEEE1666_UCONTEXT=1
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/ucontext_coroutine.lo"
else
	LIBIEEE1666_UCONTEXT=0
fi

if test ${LIBIEEE1666_ENABLE_PTHREAD} -eq 1 && test ${LIBIEEE1666_PTHREAD} -eq 1; then
	LIBS="${PTHREAD_LIBS} ${LIBS}"
	if test ${LIBIEEE1666_WINDOWS} -eq 1; then
		LIBS="-lpthread ${LIBS}"
	fi
	CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/pthread_coroutine.lo"
fi

if test ${LIBIEEE1666_ENABLE_WINDOWS_GUARDED_STACK} -eq 1 && test ${LIBIEEE1666_GET_SYSTEM_INFO} -eq 1 && test ${LIBIEEE1666_VIRTUAL_ALLOC} -eq 1 && test ${LIBIEEE1666_VIRTUAL_PROTECT} -eq 1 && test ${LIBIEEE1666_VIRTUAL_FREE} -eq 1; then
	LIBIEEE1666_WINDOWS_GUARDED_STACK=1
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/windows_guarded_stack.lo"
else
	LIBIEEE1666_WINDOWS_GUARDED_STACK=0
fi

if test ${LIBIEEE1666_ENABLE_BACKTRACE} -eq 1 && test ${LIBIEEE1666_EXECINFO_H} -eq 1 && test ${LIBIEEE1666_CXXABI_H} -eq 1 && test ${LIBIEEE1666_BACKTRACE_FUNCTION} && test ${LIBIEEE1666_BACKTRACE_SYMBOLS_FUNCTION} -eq 1; then

	LIBIEEE1666_BACKTRACE=1

    case "${CXX}" in
       *g++)
           LDFLAGS="${LDFLAGS} -rdynamic"
           ;;
       *)
           ;;
    esac

	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/utilities/sysdep/backtrace.lo"
else
	LIBIEEE1666_BACKTRACE=0
fi

if test ${LIBIEEE1666_ENABLE_VALGRIND} -eq 1 && test ${LIBIEEE1666_VALGRIND_H} -eq 1; then
	LIBIEEE1666_VALGRIND=1
else
	LIBIEEE1666_VALGRIND=0
fi

AC_SUBST(LIBIEEE1666_POSIX_GUARDED_STACK)
AC_SUBST(LIBIEEE1666_FCONTEXT)
AC_SUBST(LIBIEEE1666_UCONTEXT)
AC_SUBST(LIBIEEE1666_PTHREAD)
AC_SUBST(LIBIEEE1666_WINDOWS_GUARDED_STACK)
AC_SUBST(LIBIEEE1666_BACKTRACE)
AC_SUBST(LIBIEEE1666_VALGRIND)
AC_SUBST(LIBIEEE1666_SYSDEPS)

############################## Check configuration coherency ##############################

if test ${LIBIEEE1666_FCONTEXT} -eq 0 && test ${LIBIEEE1666_PTHREAD} -eq 0 && test ${LIBIEEE1666_UCONTEXT} -eq 0; then
	AC_MSG_ERROR([Boost::Context library, POSIX threads, or ucontext support must be available])
fi

##################################### packaging stuff ########################################

LIBIEEE1666_PKG_CONFIG_CXXFLAGS=$(echo "${CXXFLAGS}" | sed "s/${USER_CXXFLAGS}//g")  # remove user's CXX flags

PACKAGE_VERSION_MAJOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\1/')
PACKAGE_VERSION_MINOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\2/')
PACKAGE_VERSION_PATCH=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\3/')
PACKAGE_RELEASE=1

RPM_PACKAGE_DATE=$(date +'%a %b %d %Y')
DEB_PACKAGE_DATE=$(date +'%a, %d %b %Y %T %z')

if test ${LIBIEEE1666_FCONTEXT} -eq 1; then
	PACKAGE_DEPENDENCY=libboost-context
	PACKAGE_DEV_DEPENDENCY=libboost-context-dev
fi

AC_SUBST(LIBIEEE1666_PKG_CONFIG_CXXFLAGS)
AC_SUBST(PACKAGE_VERSION_MAJOR)
AC_SUBST(PACKAGE_VERSION_MINOR)
AC_SUBST(PACKAGE_VERSION_PATCH)
AC_SUBST(PACKAGE_RELEASE)
AC_SUBST(RPM_PACKAGE_DATE)
AC_SUBST(DEB_PACKAGE_DATE)
AC_SUBST(VERSION)
AC_SUBST(PACKAGE_DEPENDENCY)
AC_SUBST(PACKAGE_DEV_DEPENDENCY)

############################## Generated files at configure time ##############################

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libieee1666.pc:pkg/pkgconfig/libieee1666.pc.in])
AC_CONFIG_FILES([systemc.pc:pkg/pkgconfig/systemc.pc.in])
AC_CONFIG_FILES([tlm.pc:pkg/pkgconfig/tlm.pc.in])
AC_CONFIG_FILES([libieee1666.spec:pkg/rpm/libieee1666.spec.in])
AC_CONFIG_FILES([debian/changelog:pkg/deb/changelog.in])
AC_CONFIG_FILES([debian/compat:pkg/deb/compat.in])
AC_CONFIG_FILES([debian/control:pkg/deb/control.in])
AC_CONFIG_FILES([debian/copyright:COPYING])
AC_CONFIG_FILES([debian/libieee1666-dev.install:pkg/deb/libieee1666-dev.install.in])
AC_CONFIG_FILES([debian/libieee1666-${PACKAGE_VERSION}.install:pkg/deb/libieee1666.install.in])
AC_CONFIG_FILES([debian/libieee1666-static-dev.install:pkg/deb/libieee1666-static-dev.install.in])
AC_CONFIG_FILES([debian/rules:pkg/deb/rules.in],[chmod +x debian/rules])
AC_CONFIG_FILES([debian/source/format:pkg/deb/source/format.in])
AC_CONFIG_FILES([libieee1666/core/features.h])

##################################### output  ##########################################

AC_OUTPUT

#################################### summary ###########################################

AC_MSG_NOTICE([-------------------------------------------------------------------])
AC_MSG_NOTICE([                Configuration summary of ${PACKAGE}                ])
AC_MSG_NOTICE([-------------------------------------------------------------------])
AC_MSG_NOTICE([Building system: ${build}])
AC_MSG_NOTICE([Target system: ${host}])
!
if test ${LIBIEEE1666_PTHREAD} -eq 1; then
	AC_MSG_NOTICE([POSIX thread coroutine system support: yes])
else
	AC_MSG_NOTICE([POSIX thread coroutine system support: no])
fi

if test ${LIBIEEE1666_FCONTEXT} -eq 1; then
	AC_MSG_NOTICE([Boost::Context (fcontext) coroutine system support: yes])
else
	AC_MSG_NOTICE([Boost::Context (fcontext) coroutine system support: no])
fi

if test ${LIBIEEE1666_UCONTEXT} -eq 1; then
	AC_MSG_NOTICE([ucontext coroutine system support: yes])
else
	AC_MSG_NOTICE([ucontext coroutine system support: no])
fi

if test ${LIBIEEE1666_POSIX_GUARDED_STACK} -eq 1; then
	AC_MSG_NOTICE([guarded stack support for POSIX: yes])
else
	AC_MSG_NOTICE([guarded stack support for POSIX: no])
fi

if test ${LIBIEEE1666_WINDOWS_GUARDED_STACK} -eq 1; then
	AC_MSG_NOTICE([guarded stack support for Windows: yes])
else
	AC_MSG_NOTICE([guarded stack support for Windows: no])
fi

if test ${LIBIEEE1666_BACKTRACE} -eq 1; then
	AC_MSG_NOTICE([backtrace support (for hacking): yes])
else
	AC_MSG_NOTICE([backtrace support (for hacking): no])
fi

if test ${LIBIEEE1666_VALGRIND} -eq 1; then
	AC_MSG_NOTICE([valgrind support: yes])
else
	AC_MSG_NOTICE([valgrind support: no])
fi

##########################################################################
