/*
 *  Copyright (c) 2015-2016,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__

#include <unisim/util/identifier/identifier.hh>
#include <inttypes.h>
#include <stdexcept>
#include <iosfwd>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  /** struct Exception
   *
   * Base class used to abort normal execution of an instruction and
   * take processor to related handler (using a throw).
   */
  struct Exception : public std::exception { Exception() {} virtual const char* what() const throw() { return "Exception"; } };

  struct UndefInstrException : Exception { UndefInstrException() {} virtual const char* what() const throw() { return "UndefInstrException"; } };
  struct HypTrapException : Exception { HypTrapException() {} virtual const char* what() const throw() { return "HypTrapException"; } };
  struct SVCException : Exception  { SVCException() {} virtual const char* what() const throw() { return "SVCException"; } };
  struct SMCException : Exception  { SMCException() {} virtual const char* what() const throw() { return "SMCException"; } };
  struct HVCException : Exception  { HVCException() {} virtual const char* what() const throw() { return "HVCException"; } };
  struct PrefetchAbortException : Exception { PrefetchAbortException() {} virtual const char* what() const throw() { return "PrefetchAbortException"; } };
  struct DataAbortException : Exception { DataAbortException() {} virtual const char* what() const throw() { return "DataAbortException"; } };
  struct VirtualAbortException : Exception { VirtualAbortException() {} virtual const char* what() const throw() { return "VirtualAbortException"; } };

  // Data Abort Types
  struct DAbort : public unisim::util::identifier::Identifier<DAbort>
  {
    enum Code {
      AccessFlag,
      Alignment,
      Background,
      Domain,
      Permission,
      Translation,
      AddressSize,
      SyncExternal,
      SyncExternalOnWalk,
      SyncParity,
      SyncParityOnWalk,
      AsyncParity,
      AsyncExternal,
      Debug,
      TLBConflict,
      Lockdown,
      Coproc,
      ICacheMaint,
      end
    } code;

    char const* c_str() const
    {
      switch (code)
        {
        default: break;
        case AccessFlag:         return "AccessFlag";
        case Alignment:          return "Alignment";
        case Background:         return "Background";
        case Domain:             return "Domain";
        case Permission:         return "Permission";
        case Translation:        return "Translation";
        case AddressSize:        return "AddressSize";
        case SyncExternal:       return "SyncExternal";
        case SyncExternalOnWalk: return "SyncExternalOnWalk";
        case SyncParity:         return "SyncParity";
        case SyncParityOnWalk:   return "SyncParityOnWalk";
        case AsyncParity:        return "AsyncParity";
        case AsyncExternal:      return "AsyncExternal";
        case Debug:              return "Debug";
        case TLBConflict:        return "TLBConflict";
        case Lockdown:           return "Lockdown";
        case Coproc:             return "Coproc";
        case ICacheMaint:        return "ICacheMaint";
        }
      return "<bad:DAbort>";
    }

    DAbort() : code(end) {}
    DAbort( Code _code ) : code(_code) {}
    DAbort( char const* _code ) : code(end) { init(_code); }

    // EncodeLDFSC()
    // =============
    // Function that gives the Long-descriptor FSC code for types of Fault
    unsigned EncodeLDFSC(unsigned level) const
    {
      unsigned result;
      switch (code)
        {
        case AddressSize:        return 0b000000 | level;
        case Translation:        return 0b000100 | level;
        case AccessFlag:         return 0b001000 | level;
        case Permission:         return 0b001100 | level;
        case SyncExternal:       return 0b010000;
        case SyncExternalOnWalk: return 0b010100 | level;
        case SyncParity:         return 0b011000;
        case SyncParityOnWalk:   return 0b011100 | level;
        case AsyncParity:        return 0b011001;
        case AsyncExternal:      return 0b010001;
        case Alignment:          return 0b100001;
        case Debug:              return 0b100010;
        case TLBConflict:        return 0b110000;
        case Lockdown:           return 0b110100;
        case Coproc:             return 0b111010;
        default: { struct Bad{}; throw Bad(); }
        }
      return result;
    }
  };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
