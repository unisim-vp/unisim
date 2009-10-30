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
		if test $host = $build; then
			# We are not crosscompiling so we can execute sdl-config on 'build' machine
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no, $with_sdl/bin)
			if test "x$sdl_config_installed" != "xyes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`$with_sdl/bin/sdl-config --cflags`"
			sdl_libs="`$with_sdl/bin/sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		else
			AC_MSG_NOTICE([using SDL at $with_sdl])
			CPPFLAGS=${CPPFLAGS}" -I$with_sdl/include"
			LDFLAGS=${LDFLAGS}" -L$with_sdl/lib"
		fi
    else
		if test $host = $build; then
			# We are not crosscompiling so we can execute sdl-config on 'build' machine
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no)
			if test "x$sdl_config_installed" != "xyes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`sdl-config --cflags`"
			sdl_libs="`sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		else
			sdl_cflags="-I/usr/include"
			sdl_libs="-L/usr/lib"
			AC_MSG_NOTICE([Trying with compiler option ${sdl_cflags} ${sdl_libs} to compile and link with SDL.])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		fi
	fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL.h, broken_sdl=no, broken_sdl=yes)
	
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
		AC_MSG_NOTICE([SDL not found. No video frame buffer or input devices will be available.])
    else
		LIBS="-lSDL ${LIBS}"
		AC_DEFINE([HAVE_SDL], [], [Whether SDL is available])
    fi
])
