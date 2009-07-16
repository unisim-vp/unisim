/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_HH
#define UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_HH

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/resetable.hh>
#include <unisim/service/interfaces/runnable.hh>
#include <unisim/service/interfaces/typed_registers.hh>
#include <unisim/component/cxx/processor/tesla/interfaces.hh>

using boost::shared_ptr;
using unisim::service::interfaces::TypedRegisters;
using unisim::service::interfaces::Resetable;
using unisim::service::interfaces::Runnable;
using unisim::service::interfaces::Memory;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::component::cxx::processor::tesla::GPRID;
using unisim::component::cxx::processor::tesla::SMAddress;
using unisim::component::cxx::processor::tesla::ConfigurationRegisterID;

namespace unisim {
namespace component {
namespace cxx {
namespace scheduler {
namespace cuda_scheduler {

template<class CPUCONFIG>
struct CoreSocket :
	Client<TypedRegisters<uint32_t, GPRID> >,
	Client<TypedRegisters<uint32_t, ConfigurationRegisterID> >,
	Client<Memory<SMAddress> >,
	Client<Resetable>,
	Client<Runnable>
{
	ServiceImport<TypedRegisters<uint32_t, GPRID> > registers_import;
	ServiceImport<TypedRegisters<uint32_t, ConfigurationRegisterID> > configuration_import;
	ServiceImport<Memory<SMAddress> > shared_memory_import;
	ServiceImport<Resetable> reset_import;
	ServiceImport<Runnable> run_import;
	
	CoreSocket(const char *name, Object *parent = 0);
	virtual bool Setup();
};

struct Schedulable
{
	virtual int BlockX() const = 0;
	virtual int BlockY() const = 0;
	virtual int BlockZ() const = 0;
	virtual int GridX() const = 0;
	virtual int GridY() const = 0;
	//virtual int ThreadsPerBlock() const = 0;
	//virtual int WarpsPerBlock() const = 0;
	virtual int GPRs() const = 0;
	virtual uint32_t SharedTotal() const = 0;
	virtual void InitShared(Memory<SMAddress> & mem,
		int index = 0, int bidx = 0, int bidy = 0, int core = 0) const = 0;
};

#if 0
struct CUDAGrid
{
	int gridx, gridy;
	int blockx, blocky, blockz;
	
	int blocks_per_core;
	
	unsigned int shared_total;
	unsigned int gprs;
	unsigned int parameters_size;
	uint8_t const * parameters;
};
#endif

template<class CONFIG>
struct CUDAScheduler : virtual Object
{
	CUDAScheduler(unsigned int cores, const char *name, Object *parent = 0);
	virtual bool Setup();
	
	void Run(Schedulable const & grid);

	// One CONFIG to rule them all?
	CoreSocket<CONFIG> & Socket(unsigned int i);
private:	
	unsigned int core_count;
	typedef std::vector<shared_ptr<CoreSocket<CONFIG> > > sockets_t;
	sockets_t sockets;
};

} // end of namespace cuda_scheduler
} // end of namespace scheduler
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
