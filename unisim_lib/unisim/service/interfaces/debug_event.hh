/*
 *  Copyright (c) 2012,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_DEBUG_EVENT_HH__
#define __UNISIM_SERVICE_INTERFACES_DEBUG_EVENT_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/debug/memory_access_type.hh>
#include <unisim/util/debug/source_code_location.hh>
#include <unisim/util/debug/subprogram.hh>
#include <inttypes.h>
#include <string>

namespace unisim {
namespace util {
namespace debug {

class Event;
template <typename ADDRESS> class Breakpoint;
template <typename ADDRESS> class Watchpoint;
template <typename ADDRESS> class FetchInsnEvent;
template <typename ADDRESS> class FetchStmtEvent;
template <typename ADDRESS> class CommitInsnEvent;
class TrapEvent;
class NextInsnEvent;
class NextStmtEvent;
class FinishEvent;
class SourceCodeBreakpoint;
template <typename ADDRESS> class SubProgramBreakpoint;
class RegisterValueChangedEvent;

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

namespace unisim {
namespace service {
namespace interfaces {

class DebugEventScanner : public ServiceInterface
{
public:
	virtual void Append(unisim::util::debug::Event *event) = 0;
};

namespace detail
{
template <class CONTAINER> class DebugEventScannerInserter;
template <class CONTAINER> class DebugEventScannerFrontInserter;
template <class CONTAINER> class DebugEventScannerBackInserter;
} // end of namespace detail

template <class CONTAINER> detail::DebugEventScannerInserter<CONTAINER> DebugEventScannerInserter(CONTAINER& container, typename CONTAINER::iterator iter);
template <class CONTAINER> detail::DebugEventScannerFrontInserter<CONTAINER> DebugEventScannerFrontInserter(CONTAINER& container, typename CONTAINER::iterator iter);
template <class CONTAINER> detail::DebugEventScannerBackInserter<CONTAINER> DebugEventScannerBackInserter(CONTAINER& container, typename CONTAINER::iterator iter);

template <class ADDRESS>
class DebugEventFactory : public ServiceInterface
{
public:
	virtual unisim::util::debug::Breakpoint<ADDRESS> *CreateBreakpoint(ADDRESS addr) = 0;
	virtual unisim::util::debug::Watchpoint<ADDRESS> *CreateWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook) = 0;
	virtual unisim::util::debug::FetchInsnEvent<ADDRESS> *CreateFetchInsnEvent() = 0;
	virtual unisim::util::debug::FetchStmtEvent<ADDRESS> *CreateFetchStmtEvent() = 0;
	virtual unisim::util::debug::CommitInsnEvent<ADDRESS> *CreateCommitInsnEvent() = 0;
	virtual unisim::util::debug::NextInsnEvent *CreateNextInsnEvent() = 0;
	virtual unisim::util::debug::NextStmtEvent *CreateNextStmtEvent() = 0;
	virtual unisim::util::debug::FinishEvent *CreateFinishEvent() = 0;
	virtual unisim::util::debug::TrapEvent *CreateTrapEvent() = 0;
	virtual unisim::util::debug::SourceCodeBreakpoint *CreateSourceCodeBreakpoint(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename = std::string()) = 0;
	virtual unisim::util::debug::SubProgramBreakpoint<ADDRESS> *CreateSubProgramBreakpoint(const unisim::util::debug::SubProgram<ADDRESS> *subprogram) = 0;
	virtual unisim::util::debug::RegisterValueChangedEvent *CreateRegisterValueChangedEvent(const char *reg_name) = 0;
};

template <class ADDRESS>
class DebugEventTrigger : public DebugEventFactory<ADDRESS>
{
public:
	// "named" events
	virtual bool Listen(unisim::util::debug::Event *event) = 0;
	virtual bool Unlisten(unisim::util::debug::Event *event) = 0;
	virtual bool IsEventListened(unisim::util::debug::Event *event) const = 0;
	virtual void ScanListenedEvents(DebugEventScanner& scanner) const = 0;
	virtual void ClearEvents() = 0;
	
	// idem potent interface: anonymous events
	virtual bool SetBreakpoint(ADDRESS addr) = 0;
	virtual bool RemoveBreakpoint(ADDRESS addr) = 0;
	virtual bool HasBreakpoints(ADDRESS addr) = 0;
	virtual bool SetWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook) = 0;
	virtual bool RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) = 0;
	virtual bool HasWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) = 0;
};

class DebugEventListener : public ServiceInterface
{
public:
	virtual void OnDebugEvent(const unisim::util::debug::Event *event) = 0;
};

namespace detail
{

template <class CONTAINER>
class DebugEventScannerInserter : DebugEventScanner
{
public:
	DebugEventScannerInserter(CONTAINER& _container, typename CONTAINER::iterator _iter) : container(_container), iter(_iter) {}
	virtual void Append(unisim::util::debug::Event *event) { container.insert(event); }
private:
	CONTAINER& container;
	typename CONTAINER::iterator iter;
};

template <class CONTAINER>
class DebugEventScannerFrontInserter : DebugEventScanner
{
public:
	DebugEventScannerFrontInserter(CONTAINER& _container, typename CONTAINER::iterator _iter) : container(_container), iter(_iter) {}
	virtual void Append(unisim::util::debug::Event *event) { container.push_front(event); }
private:
	CONTAINER& container;
	typename CONTAINER::iterator iter;
};

template <class CONTAINER>
class DebugEventScannerBackInserter : DebugEventScanner
{
public:
	DebugEventScannerBackInserter(CONTAINER& _container, typename CONTAINER::iterator _iter) : container(_container), iter(_iter) {}
	virtual void Append(unisim::util::debug::Event *event) { container.push_back(event); }
private:
	CONTAINER& container;
	typename CONTAINER::iterator iter;
};

} // end of namespace detail

template <class CONTAINER>
detail::DebugEventScannerInserter<CONTAINER> DebugEventScannerInserter(CONTAINER& container, typename CONTAINER::iterator iter)
{
	return detail::DebugEventScannerInserter<CONTAINER>(container, iter);
}

template <class CONTAINER>
detail::DebugEventScannerFrontInserter<CONTAINER> DebugEventScannerFrontInserter(CONTAINER& container, typename CONTAINER::iterator iter)
{
	return detail::DebugEventScannerFrontInserter<CONTAINER>(container, iter);
}

template <class CONTAINER>
detail::DebugEventScannerBackInserter<CONTAINER> DebugEventScannerBackInserter(CONTAINER& container, typename CONTAINER::iterator iter)
{
	return detail::DebugEventScannerBackInserter<CONTAINER>(container, iter);
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
