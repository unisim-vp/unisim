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

USER_CXXFLAGS="${CXXFLAGS}"

AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

PKG_PROG_PKG_CONFIG

AC_SUBST(VERSION)

LIBIEEE1666_SC_THREAD_PROCESS_BACKEND
LIBIEEE1666_CHECK_CXXABI(main)

LIBIEEE1666_PKG_CONFIG_CXXFLAGS=$(echo "${CXXFLAGS}" | sed "s/${USER_CXXFLAGS}//g")  # remove user's CXX flags

PACKAGE_VERSION_MAJOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\1/')
PACKAGE_VERSION_MINOR=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\2/')
PACKAGE_VERSION_PATCH=$(echo "${PACKAGE_VERSION}" | sed 's/^.*\([[0-9]]\+\)\.\([[0-9]]\+\)\.\([[0-9]]\+\).*$/\3/')
PACKAGE_RELEASE=1

RPM_PACKAGE_DATE=$(date +'%a %b %d %Y')
DEB_PACKAGE_DATE=$(date +'%a, %d %b %Y %T %z')

case "${host}" in
	*mingw*) # windows host
		LDFLAGS="-no-undefined ${LDFLAGS}"
		;;
esac

AC_SUBST(SC_THREAD_PROCESSES_USE_PTHREADS)
AC_SUBST(LIBIEEE1666_PKG_CONFIG_CXXFLAGS)
AC_SUBST(PACKAGE_VERSION_MAJOR)
AC_SUBST(PACKAGE_VERSION_MINOR)
AC_SUBST(PACKAGE_VERSION_PATCH)
AC_SUBST(PACKAGE_RELEASE)
AC_SUBST(RPM_PACKAGE_DATE)
AC_SUBST(DEB_PACKAGE_DATE)

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

AC_OUTPUT
