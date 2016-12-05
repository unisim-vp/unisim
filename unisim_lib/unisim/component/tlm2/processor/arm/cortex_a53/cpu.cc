/*
 *  Copyright (c) 2016,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#include <unisim/component/tlm2/processor/arm/cortex_a53/cpu.hh>
#include <unisim/component/cxx/processor/arm/vmsav8/cpu.tcc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/likely/likely.hh>

#include <systemc.h>
#include <tlm.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_a53 {

using namespace unisim::kernel::logger;

CPU::CPU( sc_module_name const& name, Object* parent )
  : unisim::kernel::service::Object(name, parent)
  , sc_module(name)
  , unisim::component::cxx::processor::arm::vmsav8::CPU<ConfigCA53>(name, parent)
  , master_socket("master_socket")
{
  master_socket.bind(*this);
  
  SC_THREAD(Run);
}

CPU::~CPU()
{
}

/** Main thread of the CPU simulator.
 * It is a loop that simply executes one instruction and increases the cpu time.
 * Also the quantum time is updated, and if it is bigger than the nice time, the
 * global SystemC time is updated.
 */
void
CPU::Run()
{
}

/**
 * Virtual method implementation to handle backward path of transactions sent 
 * through the master_port
 *
 * @param trans the transcation to handle
 * @param phase the state of the transaction to handle (should only be END_REQ
 *        or BEGIN_RESP)
 * @param time  the relative time at which the call is being done
 *
 * @return      the synchronization status
 */
tlm::tlm_sync_enum 
CPU::nb_transport_bw (transaction_type& trans, phase_type& phase, sc_core::sc_time& time)
{
  sync_enum_type ret = tlm::TLM_ACCEPTED;

  if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) 
  {
    PCPU::logger << DebugWarning << "Received nb_transport_bw on master socket" 
                 << ", with an ignore, which the cpu doesn't know how to handle" 
                 // << std::endl << TIME(time)
                 // << std::endl << PHASE(phase)
                 << std::endl;
    //TRANS(PCPU::logger, trans);
    PCPU::logger << EndDebug;
    return ret;
  }

  switch(phase)
  {
    case tlm::BEGIN_REQ:
    case tlm::END_RESP:
      /* The cpu should not receive the BEGIN_REQ (as it is the cpu which 
       * generates cpu requests), neither END_RESP (as it is the cpu which
       * ends responses) */
      PCPU::logger << DebugError << "Received nb_transport_bw on master_socket" 
                   << ", with unexpected phase"
                   // << std::endl << LOCATION
                   // << std::endl << TIME(time)
                   // << std::endl << PHASE(phase)
                   << std::endl;
      //TRANS(PCPU::logger, trans);
      PCPU::logger << EndDebug;
      Stop(-1);
      break;
    case tlm::END_REQ:
      /* The request phase is finished.
       * If the request was a write, then the request can be released, because we do not 
       *   expect any answer. And we can send a TLM_COMPLETED for the same reason.
       * If the request was a read, the request can not be released and TLM_ACCEPTED has to
       *   be sent, and we should wait for the BEGIN_RESP phase. */
      if (trans.is_write()) 
      {
        trans.release();
        ret = tlm::TLM_COMPLETED;
      }
      else
        ret = tlm::TLM_ACCEPTED;
      return ret;
      break;
    case tlm::BEGIN_RESP:
      /* Starting the response phase.
       * If the request is a write report an error and stop, we should not have received it.
       * The target has initiated the response to a read request, compute when the request can
       *   be completely accepted and send a TLM_COMPLETED back. Send an event to the PrRead
       *   method to unlock the thread that originated the read transaction (using the end_read_event).
       */
      trans.acquire();
      if (trans.is_write())
      {
        PCPU::logger << DebugError << "Received nb_transport_bw on BEGIN_RESP phase, with unexpected write transaction"
                     // << std::endl << LOCATION
                     // << std::endl << TIME(time)
                     // << std::endl << PHASE(phase)
                     << std::endl;
        //TRANS(PCPU::logger, trans);
        PCPU::logger << EndDebug;
        Stop(-1);
        break;
      }
      sc_time tmp_time = sc_time_stamp();
      tmp_time += time;
      /* TODO: increase tmp_time depending on the size of the transaction. */
      end_read_rsp_event.notify(time);
      ret = tlm::TLM_COMPLETED;
      trans.release();
      return ret;
      break;
  }

  /* this code should never be executed, if you are here something is wrong 
   *   above :( */
  PCPU::logger << DebugError 
               << "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN"
               // << std::endl << LOCATION
               // << std::endl << TIME(time)
               // << std::endl << PHASE(phase)
               << std::endl;
  //TRANS(PCPU::logger, trans);
  PCPU::logger << EndDebug;
  Stop(-1);
  // useless return to avoid compiler warnings/errors
  return ret;
}

/**
 * Virtual method implementation to handle backward path of dmi requests sent through the
 * master port.
 * We do not use the dmi option in our simulator, so this method is unnecessary. However,
 * we have to declare it in order to be able to compile the simulator.
 *
 * @param start_range the start address of the memory range to remove
 * @param end_range   the end address of the memory range to remove
 */
void 
CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
{
  dmi_region_cache.Invalidate(start_range, end_range);
}

} // end of namespace cortex_a53
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION
#undef TIME
#undef PHASE
#undef TRANS

