/*
 *  Copyright (c) 2017,
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
 * Author: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_MASTER_ID_HH__
#define __UNISIM_KERNEL_TLM2_MASTER_ID_HH__

#include <systemc>
#include <tlm>

namespace unisim {
namespace kernel {
namespace tlm2 {

class tlm_master_id : public tlm::tlm_extension<tlm_master_id>
{
public:
	inline tlm_master_id();
	inline tlm_master_id(int master_id);
	
	inline operator int () const;
	inline tlm_master_id& operator = (int);

	virtual inline tlm::tlm_extension_base *clone() const;
	virtual inline void copy_from(const tlm::tlm_extension_base&);
private:
	int master_id;
};

inline tlm_master_id::tlm_master_id()
	: master_id(0)
{
}

inline tlm_master_id::tlm_master_id(int v)
	: master_id(v)
{
}

inline tlm_master_id::operator int () const
{
	return master_id;
}

inline tlm_master_id& tlm_master_id::operator = (int v)
{
	master_id = v;
	return *this;
}

inline tlm::tlm_extension_base *tlm_master_id::clone() const
{
	tlm_master_id *clone = new tlm_master_id(master_id);
	return clone;
}

inline void tlm_master_id::copy_from(const tlm::tlm_extension_base& ext)
{
	master_id = static_cast<const tlm_master_id *>(&ext)->master_id;
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_MASTER_ID_HH__
