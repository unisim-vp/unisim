/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#include <unisim/component/cxx/processor/tesla/flags.hh>
#include <cassert>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

string CondCodeString(Cond c)
{
	static char const * cc_desc[] = {
		"fl",	// 0
		"lt",
		"eq",
		"le",
		"gt",
		"ne",
		"ge",
		"leg",
		"nan",
		"ltu",
		"equ",
		"leu",
		"gtu",
		"neu",
		"geu",
		"tr",
		"of",
		"cf",
		"ab",
		"sf",
		"20",
		"21",
		"22",
		"23",
		"24",
		"25",
		"26",
		"27",
		"nsf",
		"ble",
		"ncf",
		"nof"	// 31
	};
	assert(c >= 0 && c < 32);
	return string(cc_desc[c]);
}

string SetCondString(SetCond c)
{
	static char const * sc_desc[] = {
		"fl",	// 0
		"lt",
		"eq",
		"le",
		"gt",
		"ne",
		"ge",
		"tr",	// 7
	};
	assert(c >= 0 && c < 8);
	return string(sc_desc[c]);
}

bool IsPredSet(uint32_t cond, uint_t<4>::fast flags)
{
	// TODO: use a BIG truth table...
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

