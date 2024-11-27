/*
 *  Copyright (c) 2024,
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

#include <unisim/util/cfg/dominance/dominance.hh>
#include <unisim/util/ostream/ostream.hh>

namespace unisim {
namespace util {
namespace cfg {
namespace dominance {

using unisim::util::ostream::FScope;

///////////////////////////////////////////////////////////////////////////////
//                               Definitions                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DominatorTreeBase ///////////////////////////////

JSON_Value *DominatorTreeBase::Save() const
{
	JSON_Array *doms_array = new JSON_Array();
	for(Tree::const_iterator itr = doms.begin(); itr != doms.end(); ++itr)
	{
		doms_array->Push(new JSON_Integer(*itr));
	}
	return doms_array;
}

void DominatorTreeBase::Load(const JSON_Value& value)
{
	doms.clear();
	const JSON_Array& doms_array = value.AsArray();
	for(JSON_Array::size_type idx = 0, doms_array_length = doms_array.Length(); idx < doms_array_length; ++idx)
	{
		doms.push_back(unsigned(doms_array[idx].AsInteger()));
	}
}

void DominatorTreeBase::Print(std::ostream& stream) const
{
	FScope<std::ostream> fscope(stream);
	stream << "[";
	for(Tree::const_iterator itr = doms.begin(); itr != doms.end(); ++itr)
	{
		stream << ((itr != doms.begin()) ? ", " :  " ");
		stream << std::dec << *itr;
	}
	stream << " ]";
}

//////////////////////////// DominanceFrontierSet /////////////////////////////

JSON_Value *DominanceFrontierSet::Save() const
{
	JSON_Array *array = new JSON_Array();
	for(Set::const_iterator itr = set.begin(); itr != set.end(); ++itr)
	{
		array->Push(new JSON_Integer(*itr));
	}
	return array;
}

void DominanceFrontierSet::Load(const JSON_Value& value)
{
	set.clear();
	const JSON_Array& array = value.AsArray();
	for(JSON_Array::size_type idx = 0, array_length = array.Length(); idx < array_length; ++idx)
	{
		set.insert(unsigned(array[idx].AsInteger()));
	}
}

void DominanceFrontierSet::Print(std::ostream& stream) const
{
	stream << "{";
	for(Set::const_iterator itr = set.begin(); itr != set.end(); ++itr)
	{
		stream << ((itr != set.begin()) ? ", " :  " ");
		stream << std::dec << *itr;
	}
	stream << " }";
}

} // end of namespace dominance
} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim

