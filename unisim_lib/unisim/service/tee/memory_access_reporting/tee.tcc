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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr),
 * 		Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__
#define __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace memory_access_reporting {

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent)
	: Object(name, parent)
	, Service<MemoryAccessReporting<ADDRESS> >(name, parent)
	, Client<MemoryAccessReporting<ADDRESS> >(name, parent)
	, in("in", this)
	, out_control("out_control", this)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		std::stringstream sstr;
		sstr << "out[" << i << "]";
		std::string import_name = sstr.str();
		out[i] = new ServiceImport<MemoryAccessReporting<ADDRESS> >(import_name.c_str(), this);
		std::stringstream sstr_control;
		sstr_control << "in_control[" << i << "]";
		std::string export_name = sstr_control.str();
		in_control[i] = new ServiceExport<MemoryAccessReportingControl>(export_name.c_str(), this);
		requires_memory_access_reporting[i] = true;
		requires_fetch_instruction_reporting[i] = true;
		requires_commit_instruction_reporting[i] = true;
		std::stringstream sstr_module;
		sstr_module << name << ".control_selector[" << i << "]";
		std::string module_name = sstr_module.str();
		control_selector[i] = new ControlSelector(module_name.c_str(), this, i);
		*in_control[i] >> control_selector[i]->in;
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void
Tee<ADDRESS, MAX_IMPORTS>::Setup(MemoryAccessReporting<ADDRESS>*)
{
	for(unsigned i = 0; i < MAX_IMPORTS; i++)
		out_n(i).RequireSetup();
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		delete control_selector[i];
		delete out[i];
		delete in_control[i];
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, 
		typename MemoryAccessReporting<ADDRESS>::MemoryType mt, 
		ADDRESS addr, uint32_t size)
{
	bool overlook = true;
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(requires_memory_access_reporting[i] and *out[i])
			overlook &= out_n(i)->ReportMemoryAccess(mat, mt, addr, size);
	}
	
	return overlook;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ReportCommitInstruction(ADDRESS addr, unsigned int size)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(requires_commit_instruction_reporting[i] and *out[i])
			out_n(i)->ReportCommitInstruction(addr, size);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ReportFetchInstruction(ADDRESS next_addr)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(requires_fetch_instruction_reporting[i] and *out[i])
			out_n(i)->ReportFetchInstruction(next_addr);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::RequiresMemoryAccessReporting(unsigned int index, unisim::service::interfaces::MemoryAccessReportingType type, bool report)
{
	bool *requires_reporting = 0;
	switch(type)
	{
		case unisim::service::interfaces::REPORT_MEM_ACCESS:
			requires_memory_access_reporting[index] = report;
			requires_reporting = &requires_memory_access_reporting[0];
			break;
		case unisim::service::interfaces::REPORT_FETCH_INSN:
			requires_fetch_instruction_reporting[index] = report;
			requires_reporting = &requires_fetch_instruction_reporting[0];
			break;
		case unisim::service::interfaces::REPORT_COMMIT_INSN:
			requires_commit_instruction_reporting[index] = report;
			requires_reporting = &requires_commit_instruction_reporting[0];
			break;
	}

	for(unsigned int idx = 0; not report and idx < MAX_IMPORTS; idx++)
		report = requires_reporting[idx];

	if(out_control)
		out_control->RequiresMemoryAccessReporting(type, report);
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::ControlSelector::ControlSelector(const char *name, Tee* _tee, unsigned int _index)
	: Object(name, _tee)
	, Service<MemoryAccessReportingControl>(name, _tee)
	, in("in", this)
        , tee(*_tee)
        , index(_index)
{
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void
Tee<ADDRESS, MAX_IMPORTS>::ControlSelector::Setup(MemoryAccessReportingControl*)
{
	for(unsigned i = 0; i < MAX_IMPORTS; i++)
		tee.out_control.RequireSetup();
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ControlSelector::RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report)
{
	tee.RequiresMemoryAccessReporting(index, type, report);
}

} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__
