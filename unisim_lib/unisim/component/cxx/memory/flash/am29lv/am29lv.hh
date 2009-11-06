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
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_HH__

#include <inttypes.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/memory/flash/am29lv/types.hh"
#include "unisim/component/cxx/memory/flash/am29lv/config.hh"
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29lv {

using unisim::service::interfaces::Memory;
using unisim::kernel::logger::Logger;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ParameterArray;
using unisim::util::endian::endian_type;

static const uint32_t K = 1024;
static const uint32_t M = 1024 * K;
static const uint32_t G = 1024 * M;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
class AM29LV :
	public Service<Memory<typename CONFIG::ADDRESS> >
{
public:
	static const unsigned int NUM_CHIPS = BYTESIZE / CONFIG::BYTESIZE;
	static const unsigned int CHIP_IO_WIDTH = IO_WIDTH / NUM_CHIPS;
	static const uint32_t ADDR_MASK = BYTESIZE - 1; // WARNING! BYTESIZE must be a power of two

	ServiceExport<Memory<typename CONFIG::ADDRESS> > memory_export;

	AM29LV(const  char *name, Object *parent = 0);
	virtual ~AM29LV();
	
	virtual void Reset();
	virtual bool Setup();
	virtual bool WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size);
protected:
	bool FSM(COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	
	// debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	unsigned int config_addr_shift; // shift (right) amount to apply to addresses in CONFIG when matching addresses
	unsigned int addr_shift;      // shift (right) amount to apply to a byte address in order to obtain the actual address provided to the flash chips
	typename CONFIG::ADDRESS org;
	uint32_t bytesize;
	endian_type endian;
	unsigned int cycle[NUM_CHIPS];
	typename CONFIG::STATE state[NUM_CHIPS];
	bool sector_protect[CONFIG::NUM_SECTORS];
	uint8_t *storage;
	Parameter<typename CONFIG::ADDRESS> param_org;
	Parameter<typename CONFIG::ADDRESS> param_bytesize;
	Parameter<endian_type> param_endian;
	ParameterArray<bool> param_sector_protect;

	bool ReverseCompare(const uint8_t *data1, const uint8_t *data2, uint32_t size);
	bool ReverseCopy(uint8_t *dest, const uint8_t *source, uint32_t size);
	int GetSector(typename CONFIG::ADDRESS addr);
	void FSM(unsigned int chip_num, COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void Read(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void ReadAutoselect(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void Program(unsigned int chip_num, typename CONFIG::ADDRESS addr, const uint8_t *data, uint32_t size);
	void ChipErase(unsigned int chip_num);
	void SectorErase(unsigned int chip_num, typename CONFIG::ADDRESS addr);
};

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_HH__
