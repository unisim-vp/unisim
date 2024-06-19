/*
 *  Copyright (c) 2011,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_STACK_FRAME_HH__
#define __UNISIM_SERVICE_INTERFACES_STACK_FRAME_HH__

#include <unisim/service/interfaces/interface.hh>

namespace unisim {
namespace service {
namespace interfaces {

template <typename ADDRESS>
struct StackFrameInfo
{
	unsigned int frame_num;
	ADDRESS addr;
	ADDRESS pc;
	ADDRESS ret_addr;
};

template <typename ADDRESS>
struct StackFrameInfoScanner : public ServiceInterface
{
	virtual void Append(const StackFrameInfo<ADDRESS>& frame_info) = 0;
};

template <typename ADDRESS>
struct StackFrame : ServiceInterface
{
	struct FrameScope
	{
		FrameScope(StackFrame *_stack_frame_if) : stack_frame_if(_stack_frame_if), save_frame_num(stack_frame_if->GetSelectedStackFrameNumber()) {}
		~FrameScope() { stack_frame_if->SelectStackFrame(save_frame_num); }
	private:
		StackFrame *stack_frame_if;
		unsigned int save_frame_num;
	};
	
	virtual bool SelectStackFrame(unsigned int frame_num) = 0;
	virtual unsigned int GetSelectedStackFrameNumber() const = 0;
	virtual std::vector<ADDRESS> *GetBackTrace() const = 0;
	virtual bool GetReturnAddress(ADDRESS& ret_addr) const = 0;
	virtual bool UnwindStack(unsigned int frame_num = 1) = 0;
	virtual void ScanStackFrameInfos(StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames = 0) const = 0;
	virtual unsigned int GetStackFrameInfos(StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
