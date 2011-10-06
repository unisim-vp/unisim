/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_BLOB_BLOB_HH__
#define __UNISIM_UTIL_DEBUG_BLOB_BLOB_HH__

#include <unisim/util/debug/blob/section.hh>
#include <unisim/util/debug/blob/segment.hh>
#include <unisim/util/endian/endian.hh>
#include <string>
#include <vector>


namespace unisim {
namespace util {
namespace debug {
namespace blob {

using unisim::util::endian::endian_type;

template <class MEMORY_ADDR>
class Blob
{
public:
	typedef enum
	{
		CAP_DEFAULT = 0,
		CAP_FILENAME = 1,
		CAP_ENTRY_POINT = 2,
		CAP_ARCHITECTURE = 4,
		CAP_STACK_BASE = 8,
		CAP_ENDIAN = 16,
		CAP_ADDRESS_SIZE = 32
	} Capability;
	
	Blob();
	Blob(const Blob<MEMORY_ADDR>& blob);
	virtual ~Blob();
	
	void SetFilename(const char *filename);
	void SetEntryPoint(MEMORY_ADDR entry_point);
	void SetArchitecture(const char *architecture);
	void SetStackBase(MEMORY_ADDR stack_base);
	void SetEndian(endian_type endian);
	void SetAddressSize(unsigned int address_size);
	
	Capability GetCapability() const;
	const char *GetFilename() const;
	MEMORY_ADDR GetEntryPoint() const;
	const char *GetArchitecture() const;
	MEMORY_ADDR GetStackBase() const;
	endian_type GetEndian() const;
	unsigned int GetAddressSize() const;
	const std::vector<const Blob<MEMORY_ADDR> *>& GetBlobs() const;
	const std::vector<const Section<MEMORY_ADDR> *>& GetSections() const;
	const std::vector<const Segment<MEMORY_ADDR> *>& GetSegments() const;
	const Section<MEMORY_ADDR> *GetSection(unsigned int index) const;
	const Segment<MEMORY_ADDR> *GetSegment(unsigned int index) const;
	void GetAddrRange(MEMORY_ADDR& min_addr, MEMORY_ADDR& max_addr) const;
	
	void AddBlob(const Blob<MEMORY_ADDR> *blob);
	void AddSection(const Section<MEMORY_ADDR> *section);
	void AddSegment(const Segment<MEMORY_ADDR> *segment);
	
	const Section<MEMORY_ADDR> *FindSection(const char *name) const;
	
	void Catch() const;
	void Release() const;
private:
	Capability capability;
	std::string filename;
	std::string architecture;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR stack_base;
	endian_type endian;
	unsigned int address_size;
	std::vector<const Blob<MEMORY_ADDR> *> blobs;
	std::vector<const Section<MEMORY_ADDR> *> sections;
	std::vector<const Segment<MEMORY_ADDR> *> segments;
	unsigned int *refcount;
};


} // end of namespace blob
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif

