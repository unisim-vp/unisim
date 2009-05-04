# ## UNISIM_CHECK_WINSOCK2
# ## Checks if the winsock2 library is available
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_WINSOCK2], [
# 	AC_CHECK_HEADERS([winsock2.h],, AC_MSG_ERROR([Some standard headers are missing.]))
# ])


# ## UNISIM_WITH_BOOST
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_WITH_BOOST], [
#     # Check if boost path has been overloaded
#     AC_ARG_WITH(boost,
# 	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
#     if test "x$with_boost" != "x"; then
# 	AC_MSG_NOTICE([using boost at $with_boost])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
#     fi
# ])

# ## UNISIM_CHECK_BOOST_INTEGER
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_INTEGER], [
#     # Check for some boost integer headers
#     AC_CHECK_HEADERS([boost/integer.hpp],,\
# 	AC_MSG_ERROR([boost integer headers not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_ARRAY
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_ARRAY], [
#     # Check for some boost graph headers
#     AC_CHECK_HEADERS([boost/array.hpp],,\
# 	AC_MSG_ERROR([boost array headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_GRAPH
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_GRAPH], [
#     # Check for some boost graph headers
#     AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
# 	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_THREAD
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_THREAD], [
#     # Check for boost thread headers
#     AC_CHECK_HEADERS([boost/thread/thread.hpp boost/thread/mutex.hpp boost/thread/xtime.hpp],,\
# 	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_FUNCTION
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_FUNCTION], [
#     # Check for boost function header
#     AC_CHECK_HEADERS([boost/function.hpp],,\
# 	AC_MSG_ERROR([boost function header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_MEM_FN
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_MEM_FN], [
#     # Check for mem_fn header
#     AC_CHECK_HEADERS([boost/mem_fn.hpp],,\
# 	AC_MSG_ERROR([boost mem_fn header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_BOOST_BIND
# ## Checks if the boost library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST_BIND], [
#     # Check for boost bind
#     AC_CHECK_HEADERS([boost/bind.hpp],,\
# 	AC_MSG_ERROR([boost bind header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

## UNISIM_CHECK_UNISIM_TOOLS
## Checks if the UNISIM tools are installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_UNISIM_TOOLS], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(unisim_tools,
	AS_HELP_STRING([--with-unisim-tools=<path>], [UNISIM tools to use (will be completed with /bin)]))
	GENISSLIB=genisslib
    if test "x$with_unisim_tools" != "x"; then
		AC_MSG_NOTICE([using UNISIM tools at $with_unisim_tools])
		GENISSLIB_DIR=$with_unisim_tools/bin
		GENISSLIB_PATH=$GENISSLIB_DIR/$GENISSLIB
		AC_SUBST(GENISSLIB_PATH)
		# Check for the GenISSLib program
		AC_CHECK_PROG(genisslib_installed, $GENISSLIB, yes, no, $GENISSLIB_DIR)
    else
		AC_MSG_NOTICE([using UNISIM tools at the default program path])
		GENISSLIB_PATH=$GENISSLIB
		AC_SUBST(GENISSLIB_PATH)
		# Check for the GenISSLib program
		AC_CHECK_PROG(genisslib_installed, $GENISSLIB, yes, no)
	fi

	
	if test "x$genisslib_installed" != "xyes"; then
			AC_MSG_ERROR([GenISSLib not found. Please install UNISIM tools.])
	fi
])


# ## UNISIM_CHECK_ZLIB
# ## Checks if the zlib library is installed
# ## Does not take parameters
# ####################################################
# AC_DEFUN([UNISIM_CHECK_ZLIB], [
#     # Check if zlib path has been overloaded
#     AC_ARG_WITH(zlib,
# 	AS_HELP_STRING([--with-zlib=<path>], [zlib library to use (will be completed with /include and /lib)]))
#     if test "x$with_zlib" != "x"; then
# 	AC_MSG_NOTICE([using zlib at $with_zlib])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_zlib/include"
#     fi
# 	
#     # Check for zlib.h
#     AC_CHECK_HEADER(zlib.h,, AC_MSG_ERROR([zlib.h not found. Please install the zlib development library. Use --with-zlib=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_CXXABI
# ## Does not take parameters
# ####################################################
# AC_DEFUN([UNISIM_CHECK_CXXABI], [
#     # Check for execinfo.h
#     AC_CHECK_HEADER(execinfo.h, broken_cxxabi=no, broken_cxxabi=yes)
#     AC_CHECK_HEADER(cxxabi.h, , broken_cxxabi=yes)
#     AC_CHECK_LIB(c, backtrace, , broken_cxxabi=yes)
#     AC_CHECK_LIB(c, backtrace_symbols, , broken_cxxabi=yes)
#     #AX_CXX_CHECK_LIB(stdc++, [abi::__cxa_demangle()], , broken_cxxabi=yes)
# 
#     if test "$broken_cxxabi" == "yes"; then
#         AC_MSG_NOTICE([C++ ABI functions not found. No kernel debug will be available.])
#     else
#         AC_DEFINE([HAVE_CXXABI], [], [Whether C++ ABI functions are available])
#     fi
# ])

# ## UNISIM_CHECK_CURSES
# ## Checks if the curses library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_CURSES], [
#     # Check if curses path has been overloaded
#     AC_ARG_WITH(ncurses,
# 	AS_HELP_STRING([--with-ncurses=<path>], [ncurses library to use (will be completed with /include and /lib)]))
#     if test "x$with_ncurses" != "x"; then
# 	AC_MSG_NOTICE([using ncurses at $with_ncurses])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_ncurses/include"
#     fi
# 	
#     # Check for ncurses.h
#     AC_CHECK_HEADER(ncurses.h, broken_ncurses=no, broken_ncurses=yes)
# 
#     # Check for functions wgetch in libncurses.a
#     AC_CHECK_LIB(ncurses, wgetch, broken_ncurses=no, broken_ncurses=yes)
# 
#     if test "$broken_ncurses" == "yes"; then
# 		AC_MSG_NOTICE([ncurses not found. No line edition capabilities will be available.])
#     else
# 		AC_DEFINE([HAVE_NCURSES], [], [Whether ncurses is available])
#     fi
# ])

# ## UNISIM_CHECK_PTHREAD
# ## Checks if the pthread library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_PTHREAD], [
#     # Check for pthread.h
#     AC_CHECK_HEADER(pthread.h,broken_pthread=no, broken_pthread=yes)
# 
#     if test "$broken_pthread" == "yes"; then
# 		AC_MSG_NOTICE([POSIX thread library not found.])
#     else
# 		AC_DEFINE([HAVE_PTHREAD], [], [Whether POSIX thread library is available])
#     fi
# ])

# ## UNISIM_CHECK_LIBEDIT
# ## Checks if the libedit library (Port of NetBSD alternative 'editline' to GNU GPL 'readline') is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_LIBEDIT], [
#     # Check if libedit path has been overloaded
#     AC_ARG_WITH(libedit,
# 	AS_HELP_STRING([--with-libedit=<path>], [libedit library to use (will be completed with /include and /lib)]))
#     if test "x$with_libedit" != "x"; then
# 	AC_MSG_NOTICE([using libedit at $with_libedit])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_libedit/include"
#     fi
# 	
#     # Check for editline/readline.h
#     AC_CHECK_HEADER(editline/readline.h, broken_libedit=no, broken_libedit=yes)
# 
#     if test "$broken_libedit" == "yes"; then
# 		AC_MSG_NOTICE([libedit (NetBSD editline port) not found. No line edition capabilities will be available.])
#     else
# 		AC_DEFINE([HAVE_LIBEDIT], [], [Whether libedit is available])
# 	fi
# 	
# ])

# ## UNISIM_CHECK_SDL
# ## Checks if the SDL library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_SDL], [
#     # Check if SDL path has been overloaded
#     AC_ARG_WITH(sdl,
# 	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))
#     if test "x$with_sdl" != "x"; then
# 	AC_MSG_NOTICE([using SDL at $with_sdl])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_sdl/include"
#     fi
# 
#     # Check for the main SDL header
#     AC_CHECK_HEADER(SDL/SDL.h, broken_sdl=no, broken_sdl=yes)
# 
#     if test "$broken_sdl" == "yes"; then
# 		AC_MSG_NOTICE([SDL not found. No video frame buffer or input devices will be available.])
#     else
# 		AC_DEFINE([HAVE_SDL], [], [Whether SDL is available])
#     fi
# ])

# ## UNISIM_CHECK_LIBXML2
# ## Checks if the libxml2 library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_LIBXML2], [
#     # Check if libxml2 path has been overloaded
#     AC_ARG_WITH(libxml2,
# 	AS_HELP_STRING([--with-libxml2=<path>], [libxml2 library to use (will be completed with /include and /lib)]))
# 	
#     if test "x$with_libxml2" != "x"; then
# 		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
# 		if test $host = $build; then
# 			# We are not crosscompiling so we can execute xml2-config on 'build' machine
# 			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no, $with_libxml2/bin)
# 			if test "x$xml2_config_installed" != "xyes"; then
# 					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
# 			fi
# 			AC_MSG_NOTICE([xml2-config found])
# 			libxml2_cflags="`$with_libxml2/bin/xml2-config --cflags`"
# 			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} to compile with libxml2])
# 			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
# 		else
# 			CPPFLAGS=${CPPFLAGS}" -I$with_libxml2/include/libxml2"
# 		fi
#     else
# 		if test $host = $build; then
# 			# We are not crosscompiling so we can execute xml2-config on 'build' machine
# 			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no)
# 			if test "x$xml2_config_installed" != "xyes"; then
# 					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
# 			fi
# 			AC_MSG_NOTICE([xml2-config found])
# 			libxml2_cflags="`xml2-config --cflags`"
# 			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} to compile with libxml2])
# 			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
# 		else
# 			libxml2_cflags="-I/usr/include/libxml2"
# 			AC_MSG_NOTICE([Trying with compiler option ${libxml2_cflags} to compile with libxml2.])
# 			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
# 		fi
# 	fi
# 	
#     # Check for some libxml2 headers
#     AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
#     AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
# 	
#     if test "$broken_incxml2" == "yes"; then
# 		AC_MSG_ERROR([libxml2 includes not found (libxml/*.hh). Please use --with-libxml2=<path>])
#     fi
# 
# ])

# ## UNISIM_CHECK_SYSTEMC
# ## Checks if the SystemC library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
#     # Check if SystemC path has been overloaded
#     AC_ARG_WITH(systemc,
# 	AS_HELP_STRING([--with-systemc=<path>], [SystemC library to use (will be completed with /include)]))
#     if test "x$with_systemc" != "x"; then
# 		AC_MSG_NOTICE([using SystemC at $with_systemc])
# 		CPPFLAGS=${CPPFLAGS}" -I$with_systemc/include"
#     fi
# 	CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"
# 
#     # Check for systemc.h
#     AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_TLM20
# ## Checks if the OSCI TLM2.0 library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_TLM20], [
# 	# Check if TLM2.0 path has been overloaded
# 	AC_ARG_WITH(tlm20,
# 	AS_HELP_STRING([--with-tlm20=<path>], [TLM2.0 library to use (will be completed with /include/tlm)]))
# 	if test "x$with_tlm20" != "x"; then
# 		AC_MSG_NOTICE([using TLM2.0 at $with_tlm20])
# 		CPPFLAGS=${CPPFLAGS}" -I$with_tlm20/include/tlm"
# 	fi
# 
# 	# Check for tlm.h
# 	AC_CHECK_HEADER(tlm.h,, AC_MSG_ERROR([tlm.h not found for TLM2.0. Please install the TLM library (version 2.0). Use --with-tlm20=<path> to overload default includes search path.]))
# ])

## UNISIM_CONFIG_SUBDIR
## 1st param: friendly name (like unisim-bla-bla)
## 2nd param: subdirectory name to enter if enabled (define COND_<subdirectory> automake macro)
#####################################################
AC_DEFUN([UNISIM_CONFIG_SUBDIR], [
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1], [enable compiling $1 (default)])
	AS_HELP_STRING([--disable-$1], [disable compiling $1]),
	[case "${enableval}" in
	yes) unisim_enabled=yes ;;
	no) unisim_enabled=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-$1) ;;
	esac], [unisim_enabled=yes])
	if test "x$3" != x; then
		supported_host1="$3"
		supported_host2="$4"
		supported_host3="$5"
		supported_host4="$6"
		supported_host5="$7"
		supported_host6="$8"
		supported_host7="$8"
		case $host in
			$supported_host1)
				;;
			*)
				case $host in
					$supported_host2)
						;;
					*)
						case $host in
							$supported_host3)
								;;
							*)
								case $host in
									$supported_host4)
										;;
									*)
										case $host in
											$supported_host5)
												;;
											*)
												case $host in
													$supported_host6)
														;;
													*)
														case $host in
															$supported_host7)
																;;
															*)
																unisim_enabled=no
																;;
														esac
														;;
												esac
												;;
										esac
										;;
								esac
								;;
						esac
						;;
				esac
				;;
		esac
	fi
	AM_CONDITIONAL(COND_$2, test $unisim_enabled == yes)
	if test $unisim_enabled == yes; then
		AC_CONFIG_SUBDIRS([$2])
	fi
])

## UNISIM_RELEASE
## Create the variable COND_release that the user can use 
##   in Makefile.am to decide what needs to be compile for release (--enable-release)
##   or just for development (--disable-release) (default)
## No parameters
#####################################################
AC_DEFUN([UNISIM_RELEASE], [
AC_ARG_ENABLE(release,
	AS_HELP_STRING([--enable-release], [build all the library versions])
	AS_HELP_STRING([--disable-release], [build the minimum number of library versions (default)]),
	[case "${enableval}" in
	yes) unisim_enabled_release=yes ;;
	no) unisim_enabled_release=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-release) ;;
	esac], [unisim_enabled_release=no])
	AM_CONDITIONAL(COND_release, test x$unisim_enabled_release = xyes)
])


# ## UNISIM_CHECK_BOOST
# ## Checks if the pthread library is installed
# ## Does not take parameters
# #####################################################
# AC_DEFUN([UNISIM_CHECK_BOOST], [
#     # Check if boost path has been overloaded
#     AC_ARG_WITH(boost,
# 	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
#     if test "x$with_boost" != "x"; then
# 	AC_MSG_NOTICE([using boost at $with_boost])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
#     fi
# 	
#     # Check for some boost graph headers
#     AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
# 	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
#     # Check for some boost thread headers
#     AC_CHECK_HEADERS([boost/thread/mutex.hpp],,\
# 	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))
# ])

# ## UNISIM_CHECK_CACTI
# ## Checks if the cacti library is installed
# ## Does not take parameters
# ####################################################
# AC_DEFUN([UNISIM_CHECK_CACTI], [
#     # Check if cacti path has been overloaded
#     AC_ARG_WITH(cacti,
# 	AS_HELP_STRING([--with-cacti=<path>], [Cacti 4.2 library to use (will be completed with /include and /lib)]))
#     if test "x$with_cacti" != "x"; then
# 	AC_MSG_NOTICE([using cacti at $with_cacti])
# 	CPPFLAGS=${CPPFLAGS}" -I$with_cacti/include"
#     fi
# 	
#     # Check for cacti4_2.hh
#     AC_CHECK_HEADER(cacti4_2.hh, broken_cacti=no, broken_cacti=yes)
# 
#     if test "$broken_cacti" == "yes"; then
# 		AC_MSG_NOTICE([CACTI 4.2 not found. Cache power estimation will be unavailable.])
#     else
# 		AC_DEFINE([HAVE_CACTI4_2], [], [Whether Cacti 4.2 is available])
# 	fi
# ])

# UNISIM_LIBTOOL
# Add some additional argument to gcc command interpreted by libtool
# This macro should be called just before generating the Makefile
# No parameters
####################################################
AC_DEFUN([UNISIM_LIBTOOL], [
	case $host in
		*mingw32*)  # win32 host
			if test "$enable_win32_dll" = "yes"; then
				LDFLAGS="-no-undefined ${LDFLAGS}"
			fi
			;;
	esac
])
