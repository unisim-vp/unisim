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

AC_LANG([C++])

USER_CXXFLAGS="${CXXFLAGS}"     # save user's CXXFLAGS

AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

PKG_PROG_PKG_CONFIG

AC_SUBST(VERSION)

############################## Check for POSIX.1-2001 compliance ######################################

AC_MSG_CHECKING([POSIX.1-2001 compliance]) 

case ${host} in
	*mingw*)
		LIBIEEE1666_POSIX=0
		;;
	*)
		AC_EGREP_CPP(conftest,
		     [#define _POSIX_C_SOURCE 200112L
		      #include <unistd.h>
		      #ifdef _POSIX_VERSION
		      #if _POSIX_VERSION >= 200112L
		      success
		      #endif
		      #endif
		     ],
		     [LIBIEEE1666_POSIX=1],
		     [LIBIEEE1666_POSIX=0])
		;;
esac

if test ${LIBIEEE1666_POSIX} -eq 1; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

LIBIEEE1666_POSIX_GUARDED_STACK=${LIBIEEE1666_POSIX}

AC_SUBST(LIBIEEE1666_POSIX_GUARDED_STACK)

############################## Check for Windows ######################################

case "${host}" in
	*mingw*) # windows host
		LIBIEEE1666_WINDOWS_GUARDED_STACK=1
		;;
	*)
		LIBIEEE1666_WINDOWS_GUARDED_STACK=0
		;;
esac

AC_SUBST(LIBIEEE1666_WINDOWS_GUARDED_STACK)

######################################### check boost ####################################

AX_BOOST_BASE([1.0.0], AC_MSG_NOTICE([boost found]), AC_MSG_ERROR([boost not found]))

##################################### check boost context ################################

AX_BOOST_BASE([1.51.0], LIBIEEE1666_BOOST_1_51=yes, LIBIEEE1666_BOOST_1_51=no)

if test "x${LIBIEEE1666_BOOST_1_51}" = "xyes"; then
	AC_MSG_NOTICE([boost >= 1.51 found])
	CPPFLAGS="${BOOST_CPPFLAGS} ${CPPFLAGS}"
	LDFLAGS="${BOOST_LDFLAGS} ${LDFLAGS}"
	AX_BOOST_CONTEXT
	LIBS="${BOOST_CONTEXT_LIB} ${LIBS}"
else
	AC_MSG_NOTICE([boost >= 1.51 not found])
fi

if test "x${ax_cv_boost_context}" = "xyes"; then
	LIBIEEE1666_FCONTEXT=1
else
	LIBIEEE1666_FCONTEXT=0
fi

AC_SUBST(LIBIEEE1666_FCONTEXT)

################################## check POSIX threads ##################################

AX_PTHREAD(LIBIEEE1666_PTHREAD=1, LIBIEEE1666_PTHREAD=0)

if test ${LIBIEEE1666_PTHREAD} -eq 1; then
	AC_MSG_NOTICE([POSIX threads found])
	LIBS="${PTHREAD_LIBS} ${LIBS}"
	case ${host} in
		*mingw*)
			LIBS="-lpthread ${LIBS}"
			;;
	esac
	CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
else
	AC_MSG_NOTICE([POSIX threads not found])
fi

AC_SUBST(LIBIEEE1666_PTHREAD)

############################# Check for backtrace support ###################################

LIBIEEE1666_BACKTRACE=1
AC_CHECK_HEADER(execinfo.h, , LIBIEEE1666_BACKTRACE=0)
AC_CHECK_HEADER(cxxabi.h, , LIBIEEE1666_BACKTRACE=0)
AC_CHECK_LIB(c, backtrace, , LIBIEEE1666_BACKTRACE=0)
AC_CHECK_LIB(c, backtrace_symbols, , LIBIEEE1666_BACKTRACE=0)

if test ${LIBIEEE1666_BACKTRACE} -eq 1; then
    case "${CXX}" in
       *g++)
           LDFLAGS="${LDFLAGS} -rdynamic"
           ;;
       *)
           ;;
    esac
fi

AC_SUBST(LIBIEEE1666_BACKTRACE)

######################## Check for valgrind "client request mechanism" ##################################

AC_CHECK_HEADER(valgrind/valgrind.h, LIBIEEE1666_VALGRIND=1, LIBIEEE1666_VALGRIND=0)

AC_SUBST(LIBIEEE1666_VALGRIND)

################################## host specific stuff #######################################

case "${host}" in
	*mingw*) # windows host
		LDFLAGS="-no-undefined ${LDFLAGS}"
		;;
esac

##################################### packaging stuff ########################################

LIBIEEE1666_PKG_CONFIG_CXXFLAGS=$(echo "${CXXFLAGS}" | sed "s/${USER_CXXFLAGS}//g")  # remove user's CXX flags

PACKAGE_VERSION_MAJOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\1/')
PACKAGE_VERSION_MINOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\2/')
PACKAGE_VERSION_PATCH=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\3/')
PACKAGE_RELEASE=1

RPM_PACKAGE_DATE=$(date +'%a %b %d %Y')
DEB_PACKAGE_DATE=$(date +'%a, %d %b %Y %T %z')

AC_SUBST(LIBIEEE1666_PKG_CONFIG_CXXFLAGS)
AC_SUBST(PACKAGE_VERSION_MAJOR)
AC_SUBST(PACKAGE_VERSION_MINOR)
AC_SUBST(PACKAGE_VERSION_PATCH)
AC_SUBST(PACKAGE_RELEASE)
AC_SUBST(RPM_PACKAGE_DATE)
AC_SUBST(DEB_PACKAGE_DATE)

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

############################## Generated files at configure time ##############################

if test ${LIBIEEE1666_FCONTEXT} -eq 0; then
	if test ${LIBIEEE1666_PTHREAD} -eq 0; then
		AC_MSG_ERROR([Boost::Context library or POSIX threads support must be available])
	fi
fi

############################## system dependent objects  ##############################

LIBIEEE1666_SYSDEPS=
if test ${LIBIEEE1666_FCONTEXT} -eq 1; then
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/fcontext_coroutine.lo"
fi
if test ${LIBIEEE1666_PTHREAD} -eq 1; then
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/pthread_coroutine.lo"
fi

if test ${LIBIEEE1666_POSIX_GUARDED_STACK} -eq 1; then
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/posix_guarded_stack.lo"
fi

if test ${LIBIEEE1666_WINDOWS_GUARDED_STACK} -eq 1; then
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/core/sysdep/windows_guarded_stack.lo"
fi

if test ${LIBIEEE1666_BACKTRACE} -eq 1; then
	LIBIEEE1666_SYSDEPS="${LIBIEEE1666_SYSDEPS} libieee1666/utilities/sysdep/backtrace.lo"
fi

AC_SUBST(LIBIEEE1666_SYSDEPS)

##################################### output  ##########################################

AC_OUTPUT

#################################### summary  ##########################################

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
