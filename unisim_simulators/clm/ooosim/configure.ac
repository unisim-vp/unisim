#
#  Copyright (c) 2007,
#  University of Perpignan (UPVD)
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#
#   - Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#
#   - Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#   - Neither the name of UPVD nor the names of its contributors may be used to
#     endorse or promote products derived from this software without specific prior
#     written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
#  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: David Parello (david.parello@univ-perp.fr)
#          
#

AC_INIT([UNISIM Out-of-order generic user level CLM simulator], [1.0],
	[David Parello <david.parello@univ-perp.fr>],
	[ooosim])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
    
AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S

AC_LANG([C++])

AC_PROG_CC

UNISIM_STATIC

###### CHECK FOR STANDARD C HEADERS ###########################################
AC_CHECK_HEADERS([string.h stdio.h stdlib.h inttypes.h stdarg.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))


UNISIM_WITH_BOOST
UNISIM_CHECK_BOOST_ARRAY

###### CHECK FOR TARGET SPECIFIC HEADERS/LIBRARIES ######################



case $host in
	*linux*)   # linux host
		UNISIM_CHECK_CURSES
		UNISIM_CHECK_LIBEDIT
		UNISIM_CHECK_PTHREAD
		;;
	*darwin*)   # darwin host
		UNISIM_CHECK_CURSES
		UNISIM_CHECK_LIBEDIT
		UNISIM_CHECK_PTHREAD
		;;
	*mingw32*)  # win32 host
		UNISIM_CHECK_WINSOCK2
		;;
esac

UNISIM_CHECK_ZLIB
UNISIM_CHECK_LIBXML2
UNISIM_CHECK_SYSTEMC
UNISIM_CHECK_CACTI

UNISIM_CHECK_UNISIM_LIB
UNISIM_CHECK_CXXABI
UNISIM_RELEASE

UNISIM_LINK(unisim_kernel_clm_engine)
UNISIM_LINK(unisim_kernel_logger)
UNISIM_LINK(unisim_kernel_service)
UNISIM_LINK(unisim_kernel_debug)

UNISIM_LINK(unisim_util_xml)
UNISIM_LINK(unisim_util_debug)
UNISIM_LINK(unisim_util_endian)
UNISIM_LINK(unisim_util_garbage_collector)
UNISIM_LINK(unisim_util_queue)

UNISIM_LINK(unisim_service_time_host_time)
UNISIM_LINK(unisim_service_power)
UNISIM_LINK(unisim_service_os_linux_os)
UNISIM_LINK(unisim_service_loader_elf_loader)
UNISIM_LINK(unisim_service_loader_linux_loader)


UNISIM_LINK(unisim_component_cxx_processor_powerpc)
UNISIM_LINK(unisim_component_cxx_memory_ram)

UNISIM_LINK(unisim_component_clm_interfaces_memreq)


UNISIM_LIBTOOL

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
