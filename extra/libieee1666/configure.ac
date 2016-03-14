AC_INIT([libieee1666], [0.0.0], [support@unisim-vp.org], [], [http://unisim-vp.org])
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
PKG_INSTALLDIR

AC_SUBST(VERSION)

LIBIEEE1666_SC_THREAD_PROCESS_BACKEND
LIBIEEE1666_CHECK_CXXABI(main)

LIBIEEE1666_PKG_CONFIG_CXXFLAGS=$(echo "${CXXFLAGS}" | sed "s/${USER_CXXFLAGS}//g")  # remove user's CXX flags

AC_SUBST(SC_THREAD_PROCESSES_USE_PTHREADS)
AC_SUBST(LIBIEEE1666_PKG_CONFIG_CXXFLAGS)

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile libieee1666.pc systemc.pc tlm.pc])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
