## UNISIM_CHECK_LIBSTDCXX
## Checks if the stdc++ library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBSTDCXX], [
	# Note: we can't check libstdc++ functions from libstdc++ because it's a library of C++ classes with no C functions.
	LIBS+=" -lstdc++"
])

## UNISIM_CHECK_BOOST_THREAD
## Checks if the boost_thread library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_THREAD], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
    if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	CPPFLAGS+=" -I$with_boost/include"
	LDFLAGS+=" -L$with_boost/lib"
    fi
	
    # Check for some boost thread headers
    AC_CHECK_HEADERS([boost/thread/mutex.hpp],,\
	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))

	# Note: we can't check libboost_thread functions from libboost_thread because it's a library of C++ classes with no C functions.
	LIBS+=" -lboost_thread"
])

## UNISIM_CHECK_WINSOCK2
## Checks if the winsock2 library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_WINSOCK2], [
	# Note: we can't check socket functions from libwsock32 because of the PASCAL calling convention. cdecl is mandatory for autoconf.
	LIBS+=" -lwsock32"
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
	LDFLAGS+=" -L$with_ncurses/lib"
	CPPFLAGS+=" -I$with_ncurses/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(ncurses.h,, AC_MSG_ERROR([ncurses.h not found. Please install the readline development library. Use --with-ncurses=<path> to overload default includes search path.]))

    # Check for functions wgetch in libncurses.a
    AC_CHECK_LIB(ncurses, wgetch, broken_ncurses=no, broken_ncurses=yes)

    if test "$broken_ncurses" == "yes"; then
	AC_MSG_ERROR([installed ncurses library is broken.])
    else
	LIBS+=" -lncurses"
    fi
])

## UNISIM_CHECK_UNISIM_LIB
## Checks if the unisim library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_UNISIM_LIB], [
    # Check if curses path has been overloaded
    AC_ARG_WITH(unisim_lib,
	AS_HELP_STRING([--with-unisim-lib=<path>], [unisim library to use (will be completed with /include and /lib)]))
    if test "x$with_unisim_lib" != "x"; then
	AC_MSG_NOTICE([using unisim library at $with_unisim_lib])
	LDFLAGS+=" -L$with_unisim_lib/lib"
	CPPFLAGS+=" -I$with_unisim_lib/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(unisim/kernel/service/service.hh,, AC_MSG_ERROR([service.hh not found. Please install the UNISIM library.]))
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
	LDFLAGS+=" -L$with_readline/lib"
	CPPFLAGS+=" -I$with_readline/include"
    fi
	
    # Check for readline/readline.h
    AC_CHECK_HEADER(readline/readline.h,, AC_MSG_ERROR([readline/realine.h not found. Please install the readline development library. Use --with-readline=<path> to overload default includes search path.]))
	
    # Check for functions readline and add_history in libreadline.a
    AC_CHECK_LIB(readline, readline,
    AC_CHECK_LIB(readline, add_history, broken_readline=no,
    broken_readline=yes),
    broken_readline=yes)
	
    if test "$broken_readline" == "yes"; then
	AC_MSG_ERROR([installed readline library is broken.])
    else
	LIBS+=" -lreadline"
    fi
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
	LDFLAGS+=" -L$with_sdl/lib"
	CPPFLAGS+=" -I$with_sdl/include"
    fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL/SDL.h,, AC_MSG_ERROR([SDL/SDL.h not found. Please install the SDL development library (version >= 1.2.0).]))
	
    # Check for SDL functions
    AC_CHECK_LIB(SDL, SDL_WaitThread,
    AC_CHECK_LIB(SDL, SDL_FreeSurface,
    AC_CHECK_LIB(SDL, SDL_DestroyMutex,
    AC_CHECK_LIB(SDL, SDL_Quit,
    AC_CHECK_LIB(SDL, SDL_Init,
    AC_CHECK_LIB(SDL, SDL_CreateMutex,
    AC_CHECK_LIB(SDL, SDL_CreateThread,
    AC_CHECK_LIB(SDL, SDL_mutexP,
    AC_CHECK_LIB(SDL, SDL_mutexV,
    AC_CHECK_LIB(SDL, SDL_GetKeyName,
    AC_CHECK_LIB(SDL, SDL_Delay,
    AC_CHECK_LIB(SDL, SDL_PollEvent,
    AC_CHECK_LIB(SDL, SDL_SetVideoMode,
    AC_CHECK_LIB(SDL, SDL_WM_SetCaption,
    AC_CHECK_LIB(SDL, SDL_CreateRGBSurface,
    AC_CHECK_LIB(SDL, SDL_LockSurface,
    AC_CHECK_LIB(SDL, SDL_UnlockSurface,
    AC_CHECK_LIB(SDL, SDL_UpperBlit,
    AC_CHECK_LIB(SDL, SDL_UpdateRect,
    AC_CHECK_LIB(SDL, SDL_SaveBMP_RW, broken_sdl=no,
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes)

    if test "$broken_sdl" == "yes"; then
	AC_MSG_ERROR([installed SDL Library is broken.])
    else
	LIBS+=" -lSDL"
    fi
])

## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [path to the libxml2 library]))
	
    if test "x$with_libxml2" != "x"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		LDFLAGS+=" -L$with_libxml2/lib"
		CPPFLAGS+=" -I$with_libxml2/include/libxml2"
    fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" == "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (/libxml2/libxml/*.hh). Please use --with-libxml2=<path>])
    fi

    # Check for libxml2 functions
    AC_CHECK_LIB(xml2, xmlNewTextWriterFilename,
    AC_CHECK_LIB(xml2, xmlTextWriterSetIndent,
    AC_CHECK_LIB(xml2, xmlTextWriterStartDocument,
    AC_CHECK_LIB(xml2, xmlTextWriterStartElement, 
    AC_CHECK_LIB(xml2, xmlTextWriterEndElement, 
    AC_CHECK_LIB(xml2, xmlFreeTextWriter, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteAttribute, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteFormatString, broken_libxml2=no,
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes)
	
    if test "$broken_libxml2" == "yes"; then
		AC_MSG_ERROR([installed xml2 Library is broken.])
    else
		LIBS+=" -lxml2"
    fi
])

## UNISIM_CHECK_SYSTEMC
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
    # Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
    CXX_COMP=`basename $CXX`
    case "$host" in
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
	AS_HELP_STRING([--with-systemc=<path>], [systemc library to use (will be completed with /include and /lib-linux)]))
    if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LDFLAGS+=" -L$with_systemc/lib-$SYSTEMC_TARGET_ARCH"
	CPPFLAGS+=" -I$with_systemc/include"
    fi
	CPPFLAGS+=" -DSC_INCLUDE_DYNAMIC_PROCESSES"

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))

    # Check for function 'main' in libsystemc.a
    AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_system=yes)

    if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default includes search path.])
    else
	LIBS+=" -lsystemc"
    fi
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
	yes) unisim_enabled=true ;;
	no) unisim_enabled=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-$1) ;;
	esac], [unisim_enabled=true])
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
																unisim_enabled=false
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
	AM_CONDITIONAL(COND_$2, test $unisim_enabled == true)
	if test $unisim_enabled == true; then
		AC_CONFIG_SUBDIRS([$2])
	fi
])

## UNISIM_LINK
AC_DEFUN([UNISIM_LINK], [
	LIBS="-l$1 $LIBS"
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
	yes) unisim_enabled_release=true ;;
	no) unisim_enabled_release=false ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-release) ;;
	esac], [unisim_enabled_release=false])
	AM_CONDITIONAL(COND_release, test x$unisim_enabled_release = xtrue)
])
