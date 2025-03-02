/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_LOADER_ELF_LOADER_ELF32_LOADER_HH__
#define __UNISIM_UTIL_LOADER_ELF_LOADER_ELF32_LOADER_HH__

#include <unisim/util/loader/elf_loader/elf_loader.hh>

namespace unisim {
namespace util {
namespace loader {
namespace elf_loader {

//typedef ElfLoaderImpl<uint32_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

template <class MEMORY_ADDR = uint32_t>
class Elf32Loader : public ElfLoaderImpl<MEMORY_ADDR, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym>
{
public:
	Elf32Loader(std::ostream& debug_info_stream,
	            std::ostream& debug_warning_stream,
	            std::ostream& debug_error_stream,
	            const unisim::util::blob::Blob<MEMORY_ADDR> *blob = 0)
	: ElfLoaderImpl<MEMORY_ADDR, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym>(debug_info_stream, debug_warning_stream, debug_error_stream, blob)
	{
	}
};

template <class ADDRESS_TYPE>
struct StdElf<ADDRESS_TYPE,uint32_t>
{
  typedef ElfLoaderImpl<ADDRESS_TYPE, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Loader;
};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
