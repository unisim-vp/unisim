## UNISIM_CHECK_WINSOCK2
## Checks if the winsock2 library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_WINSOCK2], [
	AC_CHECK_HEADERS([winsock2.h],, AC_MSG_ERROR([Some standard headers are missing.]))
])


## UNISIM_WITH_BOOST
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_WITH_BOOST], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
    if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
    fi
])

## UNISIM_CHECK_BOOST_ARRAY
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_ARRAY], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/array.hpp],,\
	AC_MSG_ERROR([boost array headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_GRAPH
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_GRAPH], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_THREAD
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_THREAD], [
    # Check for boost thread headers
    AC_CHECK_HEADERS([boost/thread/thread.hpp boost/thread/mutex.hpp boost/thread/xtime.hpp],,\
	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))

	# Note: we can't check libboost_thread functions from libboost_thread because it's a library of C++ classes with no C functions.
	LIBS=${LIBS}" -lboost_thread"
])

## UNISIM_CHECK_BOOST_FUNCTION
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_FUNCTION], [
    # Check for boost function header
    AC_CHECK_HEADERS([boost/function.hpp],,\
	AC_MSG_ERROR([boost function header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_MEM_FN
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_MEM_FN], [
    # Check for mem_fn header
    AC_CHECK_HEADERS([boost/mem_fn.hpp],,\
	AC_MSG_ERROR([boost mem_fn header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_BIND
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_BIND], [
    # Check for boost bind
    AC_CHECK_HEADERS([boost/bind.hpp],,\
	AC_MSG_ERROR([boost bind header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

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


## UNISIM_CHECK_ZLIB
## Checks if the zlib library is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_ZLIB], [
    # Check if zlib path has been overloaded
    AC_ARG_WITH(zlib,
	AS_HELP_STRING([--with-zlib=<path>], [zlib library to use (will be completed with /include and /lib)]))
    if test "x$with_zlib" != "x"; then
	AC_MSG_NOTICE([using zlib at $with_zlib])
	CPPFLAGS=${CPPFLAGS}" -I$with_zlib/include"
    fi
	
    # Check for zlib.h
    AC_CHECK_HEADER(zlib.h,, AC_MSG_ERROR([zlib.h not found. Please install the zlib development library. Use --with-zlib=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_CURSES
## Checks if the curses library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_CURSES], [
    # Check if curses path has been overloaded
    AC_ARG_WITH(ncurses,
	AS_HELP_STRING([--with-ncurses=<path>], [ncurses library to use (will be completed with /include and /lib)]))
    if test "x$with_ncurses" != "x"; then
	AC_MSG_NOTICE([using ncurses at $with_ncurses])
	CPPFLAGS=${CPPFLAGS}" -I$with_ncurses/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(ncurses.h,, AC_MSG_ERROR([ncurses.h not found. Please install the readline development library. Use --with-ncurses=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_PTHREAD
## Checks if the curses library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_PTHREAD], [
    # Check for pthread.h
    AC_CHECK_HEADER(pthread.h,, AC_MSG_ERROR([pthread.h not found.]))

    if test "$broken_pthread" == "yes"; then
	AC_MSG_ERROR([installed pthread library is broken.])
    else
	LIBS=${LIBS}" -lpthread"
    fi
])

## UNISIM_CHECK_READLINE
## Checks if the realine library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_READLINE], [
    # Check if readline path has been overloaded
    AC_ARG_WITH(readline,
	AS_HELP_STRING([--with-readline=<path>], [readline library to use (will be completed with /include and /lib)]))
    if test "x$with_readline" != "x"; then
	AC_MSG_NOTICE([using readline at $with_readline])
	CPPFLAGS=${CPPFLAGS}" -I$with_readline/include"
    fi
	
    # Check for readline/readline.h
    AC_CHECK_HEADER(readline/readline.h,, AC_MSG_ERROR([readline/realine.h not found. Please install the readline development library. Use --with-readline=<path> to overload default includes search path.]))
	
])

## UNISIM_CHECK_SDL
## Checks if the SDL library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SDL], [
    # Check if SDL path has been overloaded
    AC_ARG_WITH(sdl,
	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))
    if test "x$with_sdl" != "x"; then
	AC_MSG_NOTICE([using SDL at $with_sdl])
	CPPFLAGS=${CPPFLAGS}" -I$with_sdl/include"
    fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL/SDL.h,, AC_MSG_ERROR([SDL/SDL.h not found. Please install the SDL development library (version >= 1.2.0).]))
	
])

## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [libxml2 library to use (will be completed with /include and /lib)]))
	
    if test "x$with_libxml2" != "x"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		CPPFLAGS=${CPPFLAGS}" -I$with_libxml2/include"
    fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" == "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (libxml/*.hh). Please use --with-libxml2=<path>])
    fi

])

## UNISIM_CHECK_SYSTEMC
## Checks if the SystemC library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [SystemC library to use (will be completed with /include)]))
    if test "x$with_systemc" != "x"; then
		AC_MSG_NOTICE([using SystemC at $with_systemc])
		CPPFLAGS=${CPPFLAGS}" -I$with_systemc/include"
    fi
	CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_TLM20
## Checks if the OSCI TLM2.0 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TLM20], [
	# Check if TLM2.0 path has been overloaded
	AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-tlm20=<path>], [TLM2.0 library to use (will be completed with /include/tlm)]))
	if test "x$with_tlm20" != "x"; then
		AC_MSG_NOTICE([using TLM2.0 at $with_tlm20])
		CPPFLAGS=${CPPFLAGS}" -I$with_tlm20/include/tlm"
	fi

	# Check for tlm.h
	AC_CHECK_HEADER(tlm.h,, AC_MSG_ERROR([tlm.h not found for TLM2.0. Please install the TLM library (version 2.0). Use --with-tlm20=<path> to overload default includes search path.]))
])

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


## UNISIM_CHECK_BOOST
## Checks if the pthread library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
    if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
    fi
	
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
    # Check for some boost thread headers
    AC_CHECK_HEADERS([boost/thread/mutex.hpp],,\
	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))
])

