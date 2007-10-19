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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace tcm {

template<uint32_t SIZE, bool DEBUG_ENABLE>
TCM<SIZE, DEBUG_ENABLE> ::
TCM(const char *name,
		Object *parent) :
	Object(name, parent),
	Client<Logger>(name, parent),
	logger_import("logger_import", this){
	SetupDependsOn(logger_import);
}

template<uint32_t SIZE, bool DEBUG_ENABLE>
TCM<SIZE, DEBUG_ENABLE> ::
~TCM() {
}

template<uint32_t SIZE, bool DEBUG_ENABLE>
bool
TCM<SIZE, DEBUG_ENABLE> ::
Setup() {
	return true;
}

template<class CONFIG>
DTCM<CONFIG> ::
DTCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG::DTCM_SIZE, CONFIG::DEBUG_ENABLE>(name, parent) {
}

template<class CONFIG>
DTCM<CONFIG> ::
~DTCM() {	
}

template<class CONFIG>
ITCM<CONFIG> ::
ITCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG::ITCM_SIZE, CONFIG::DEBUG_ENABLE>(name, parent) {
}

template<class CONFIG>
ITCM<CONFIG> ::
~ITCM() {	
}

} // end of namespace tcm
} // end of namespace arm
} // end of namespace processor
} // end of namespace component
} // end of namespace cxx
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__
