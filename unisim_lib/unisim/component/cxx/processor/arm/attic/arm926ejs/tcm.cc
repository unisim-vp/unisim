/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/component/cxx/processor/arm/arm926ejs/tcm.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {
						
						
TCM::TCM() :
m_size(0),
m_base_addr(0),
m_is_ok(true)
{
}

TCM::~TCM()
{
}

bool
TCM::SetSize(uint32_t size)
{
	m_size = size;
	m_is_ok = CheckSize() && CheckBaseAddress();
	return m_is_ok;
}

uint32_t
TCM::
GetSize()
{
	return m_size;
}

bool
TCM::
SetBaseAddress(uint32_t addr)
{
	m_base_addr = addr;
	m_is_ok = CheckSize() && CheckBaseAddress();
	return m_is_ok;
}

uint32_t
TCM::
GetBaseAddress()
{
	return m_base_addr;
}

bool 
TCM::
IsOK()
{
	return m_is_ok;
}

bool
TCM::
CheckSize()
{	
	bool is_ok = false;
	
	if (m_size % 1024 == 0) 
	{
		switch(m_size / 1024)
		{
			case 0:
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
			case 128:
			case 256:
			case 512:
			case 1024:
				is_ok = true;
			default:
				is_ok = false;
		}
	}
	
	return is_ok;
}
	
bool
TCM::
CheckBaseAddress()
{
	bool is_ok;
	is_ok = (m_base_addr % m_size == 0);
	return is_ok;
}
	
} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

