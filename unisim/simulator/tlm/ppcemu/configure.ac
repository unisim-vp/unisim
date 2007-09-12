#
#  Copyright (c) 2007,
#  Commissariat a l'Energie Atomique (CEA)
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
#   - Neither the name of CEA nor the names of its contributors may be used to
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
# Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
#          Gilles Mouchard (gilles.mouchard@cea.fr)
#

AC_INIT([UNISIM PowerPC user level TLM simulator], [1.0],
	[Gilles Mouchard <gilles.mouchard@cea.fr>],
	[ppcemu])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
    
AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_LANG([C++])

###### CHECK FOR STANDARD C HEADERS ###########################################
AC_CHECK_HEADERS([string.h stdio.h stdlib.h inttypes.h stdarg.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))


UNISIM_CHECK_BOOST
UNISIM_CHECK_CURSES
UNISIM_CHECK_READLINE
UNISIM_CHECK_LIBXML2
UNISIM_CHECK_SDL
UNISIM_CHECK_SYSTEMC

UNISIM_LINK(../../../../unisim/kernel/service, services)

UNISIM_LINK(../../../../unisim/util/xml, xml)
UNISIM_LINK(../../../../unisim/util/debug, debug)
UNISIM_LINK(../../../../unisim/util/endian, endian)
UNISIM_LINK(../../../../unisim/util/garbage_collector, garbage_collector)

UNISIM_LINK(../../../../unisim/service/time/sc_time, time)
UNISIM_LINK(../../../../unisim/service/power, power)
UNISIM_LINK(../../../../unisim/service/debug/gdb_server, gdb_server)
UNISIM_LINK(../../../../unisim/service/debug/inline_debugger, inline_debugger)
UNISIM_LINK(../../../../unisim/service/os/linux_os, linux_os)
UNISIM_LINK(../../../../unisim/service/loader/elf_loader, elf_loader)
UNISIM_LINK(../../../../unisim/service/debug/symbol_table, symbol_table)
UNISIM_LINK(../../../../unisim/service/loader/pmac_bootx, pmac_bootx)
UNISIM_LINK(../../../../unisim/service/loader/pmac_linux_kernel_loader, pmac_linux_kernel_loader)
UNISIM_LINK(../../../../unisim/service/loader/linux_loader, linux_loader)
UNISIM_LINK(../../../../unisim/service/logger, logger)
UNISIM_LINK(../../../../unisim/service/sdl, sdl)

UNISIM_LINK(../../../../unisim/component/cxx/cache/insn, cache)
UNISIM_LINK(../../../../unisim/component/cxx/cache/mesi, cache)
UNISIM_LINK(../../../../unisim/component/cxx/processor/arm, arm)
UNISIM_LINK(../../../../unisim/component/cxx/processor/powerpc, powerpc)
UNISIM_LINK(../../../../unisim/component/cxx/pci/debug, debug)
UNISIM_LINK(../../../../unisim/component/cxx/pci/macio, macio)
UNISIM_LINK(../../../../unisim/component/cxx/pci, pci)
UNISIM_LINK(../../../../unisim/component/cxx/pci/ide, ide)
UNISIM_LINK(../../../../unisim/component/cxx/pci/video, video)
UNISIM_LINK(../../../../unisim/component/cxx/chipset/mpc107/epic, epic)
UNISIM_LINK(../../../../unisim/component/cxx/chipset/mpc107/dma, dma)
UNISIM_LINK(../../../../unisim/component/cxx/chipset/mpc107, mpc107)
UNISIM_LINK(../../../../unisim/component/cxx/isa/i8042, i8042)
UNISIM_LINK(../../../../unisim/component/cxx/bridge/pci_isa, pci_isa)
UNISIM_LINK(../../../../unisim/component/cxx/memory/ram, ram)
UNISIM_LINK(../../../../unisim/component/cxx/memory/flash/am29lv, am29lv)

UNISIM_LINK(../../../../unisim/component/tlm/processor/powerpc, powerpc)
UNISIM_LINK(../../../../unisim/component/tlm/processor/arm, arm)
UNISIM_LINK(../../../../unisim/component/tlm/memory/flash/am29lv, am29lv)
UNISIM_LINK(../../../../unisim/component/tlm/memory/ram, ram)
UNISIM_LINK(../../../../unisim/component/tlm/bridge/pci_isa, pci_isa)
UNISIM_LINK(../../../../unisim/component/tlm/bridge/snooping_fsb_to_mem, bridge)
UNISIM_LINK(../../../../unisim/component/tlm/fsb/snooping_bus, snoopingbus)
UNISIM_LINK(../../../../unisim/component/tlm/fsb/simple_bus, simplebus)
UNISIM_LINK(../../../../unisim/component/tlm/message, message)
UNISIM_LINK(../../../../unisim/component/tlm/isa/i8042, i8042)
UNISIM_LINK(../../../../unisim/component/tlm/chipset/mpc107/epic, epic)
UNISIM_LINK(../../../../unisim/component/tlm/chipset/mpc107, mpc107)
UNISIM_LINK(../../../../unisim/component/tlm/pci/video, video)
UNISIM_LINK(../../../../unisim/component/tlm/pci/ide, ide)
UNISIM_LINK(../../../../unisim/component/tlm/pci/macio, macio)
UNISIM_LINK(../../../../unisim/component/tlm/pci/debug, debug)

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
