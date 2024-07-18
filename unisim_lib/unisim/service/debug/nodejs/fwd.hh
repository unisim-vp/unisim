/*
 *  Copyright (c) 2023,
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

#ifndef __UNISIM_SERVICE_DEBUG_NODEJS_FWD_HH__
#define __UNISIM_SERVICE_DEBUG_NODEJS_FWD_HH__


namespace unisim {
namespace service {
namespace debug {
namespace nodejs {

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

template <typename CONFIG> struct NodeJS;
template <typename CONFIG> struct ObjectWrapper;
template <typename CONFIG> struct ProcessorWrapper;
template <typename CONFIG> struct RegisterWrapper;
template <typename CONFIG> struct EventBridge;
template <typename CONFIG> struct SourceCodeLocationWrapper;
template <typename CONFIG> struct DebugEventWrapper;
template <typename CONFIG> struct BreakpointWrapper;
template <typename CONFIG> struct SourceCodeBreakpointWrapper;
template <typename CONFIG> struct SubProgramBreakpointWrapper;
template <typename CONFIG> struct WatchpointWrapper;
template <typename CONFIG> struct DataObjectWrapper;
template <typename CONFIG> struct HookWrapper;
template <typename CONFIG> struct AddressHookWrapper;
template <typename CONFIG> struct SourceCodeHookWrapper;
template <typename CONFIG> struct SubProgramHookWrapper;
template <typename CONFIG> struct AddressHook;
template <typename CONFIG> struct SourceCodeHook;
template <typename CONFIG> struct SubProgramHook;
template <typename CONFIG> struct StubWrapper;
template <typename CONFIG> struct Stub;
template <typename CONFIG> struct ExecutableBinaryFileWrapper;
template <typename CONFIG> struct StackFrameInfoWrapper;
template <typename CONFIG> struct DebugSymbolWrapper;
template <typename CONFIG> struct StatementWrapper;
template <typename CONFIG> struct SubProgramWrapper;
template <typename CONFIG> struct TypeWrapper;
template <typename CONFIG> struct NamedTypeWrapper;
template <typename CONFIG> struct BaseTypeWrapper;
template <typename CONFIG> struct IntegerTypeWrapper;
template <typename CONFIG> struct CharTypeWrapper;
template <typename CONFIG> struct FloatingPointTypeWrapper;
template <typename CONFIG> struct BooleanTypeWrapper;
template <typename CONFIG> struct MemberWrapper;
template <typename CONFIG> struct CompositeTypeWrapper;
template <typename CONFIG> struct StructureTypeWrapper;
template <typename CONFIG> struct UnionTypeWrapper;
template <typename CONFIG> struct ClassTypeWrapper;
template <typename CONFIG> struct InterfaceTypeWrapper;
template <typename CONFIG> struct ArrayTypeWrapper;
template <typename CONFIG> struct PointerTypeWrapper;
template <typename CONFIG> struct TypedefWrapper;
template <typename CONFIG> struct FormalParameterWrapper;
template <typename CONFIG> struct FunctionTypeWrapper;
template <typename CONFIG> struct ConstTypeWrapper;
template <typename CONFIG> struct EnumeratorWrapper;
template <typename CONFIG> struct EnumTypeWrapper;
template <typename CONFIG> struct UnspecifiedTypeWrapper;
template <typename CONFIG> struct VolatileTypeWrapper;

} // end of namespace nodejs
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_NODEJS_FWD_HH__
