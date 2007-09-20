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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <unisim/component/cxx/cache/mesi/cache.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace mesi {
	
Action actions[5][4] = {
	// EvPrRd
	{
		// ST_INVALID
		AcBusRd,        // (I)-[PrRd/BusRd(S)]->(S)  or   (I)-[PrRd/BusRd(!S)]->(E)
		// ST_SHARED
		AcWB,           // (S)-[PrRd/WB]->(S)
		// ST_EXCLUSIVE
		AcNone,         // (E)-[PrRd/..]->(E)
		// ST_MODIFIED
		AcNone          // (M)-[PrRd/..]->(M)
	},
	// EvPrRdX,
	{
		// ST_INVALID
		AcBusRdX,       // (I)-[PrRdX/BusRdX]->(E)
		// ST_SHARED
		AcBusRdX,       // (I)-[PrRdX/BusRdX]->(E)
		// ST_EXCLUSIVE
		AcNone,         // (E)-[PrRdX/..]->(E)
		// ST_MODIFIED
		AcNone,         // (M)-[PrRdX/..]->(E)
	},
	// EvPrWr
	{
		// ST_INVALID
		AcBusRdX,       // (I)-[PrWr/BusRdX]->(M)
		// ST_SHARED
		AcBusRdX,       // (E)-[PrWr/BusRdX]->(M)
		// ST_EXCLUSIVE
		AcNone,         // (E)-[PrWr/..]->(M)
		// ST_MODIFIED
		AcNone          // (M)-[PrWr/..]->(M)
	},
	// EvBusRd
	{
		// ST_INVALID
		AcNone,         // (I)-[BusRd/..]->(I)
		// ST_SHARED
		AcWB,           // (S)-[BusRd/WB]->(S)
		// ST_EXCLUSIVE
		AcWB,           // (E)-[BusRd/WB]->(S)
		// ST_MODIFIED
		AcWB            // (M)-[BusRd/WB]->(S)
	},
	// EvBusRdX
	{
		// ST_INVALID
		AcNone,         // (I)-[BusRdX/..]->(I)
		// ST_SHARED
		AcWB,           // (S)-[BusRdX/WB]->(I)
		// ST_EXCLUSIVE
		AcWB,           // (E)-[BusRdX/WB]->(I)
		// ST_MODIFIED
		AcWB            // (M)-[BusRdX/WB]->(I)
	}
};

CacheBlockState transitions[2][5][4] = {
	// not shared
	{
		// EvPrRd
		{
			// ST_INVALID
			ST_EXCLUSIVE,       // (I)-[PrRd/BusRd(!S)->(E)
			// ST_SHARED
			ST_SHARED,          // (S)-[PrRd/..]->(S)
			// ST_EXCLUSIVE
			ST_EXCLUSIVE,       // (E)-[PrRd/..]->(E)
			// ST_MODIFIED
			ST_MODIFIED         // (M)-[PrRd/..]->(M)
		},
		// EvPrRdX
		{
			// ST_INVALID
			ST_EXCLUSIVE,       // (I)-[PrRdX/BusRdX->(E)
			// ST_SHARED
			ST_EXCLUSIVE,       // (S)-[PrRdX/BusRdX]->(E)
			// ST_EXCLUSIVE
			ST_EXCLUSIVE,       // (E)-[PrRdX/..]->(E)
			// ST_MODIFIED
			ST_EXCLUSIVE        // (M)-[PrRdX/..]->(M)
		},
		// EvPrWr
		{
			// ST_INVALID
			ST_MODIFIED,        // (I)-[PrWr/BusRdX]->(M)
			// ST_SHARED
			ST_MODIFIED,        // (S)-[PrWr/BusRdX]->(M)
			// ST_EXCLUSIVE
			ST_MODIFIED,        // (E)-[PrWr/..]->(M)
			// ST_MODIFIED
			ST_MODIFIED         // (M)-[PrWr/..]->(M)
		},
		// EvBusRd
		{
			// ST_INVALID
			ST_INVALID,         // (I)-[BusRd/..]->(I)
			// ST_SHARED
			ST_SHARED,          // (S)-[BusRd/WB]->(S)
			// ST_EXCLUSIVE
			ST_SHARED,          // (E)-[BusRd/WB]->(S)
			// ST_MODIFIED
			ST_SHARED           // (M)-[BusRd/WB]->(S)
		},
		// EvBusRdX
		{
			// ST_INVALID
			ST_INVALID,         // (I)-[BusRdX/..]->(I)
			// ST_SHARED
			ST_INVALID,         // (S)-[BusRdX/WB]->(I)
			// ST_EXCLUSIVE
			ST_INVALID,         // (E)-[BusRdX/WB]->(I)
			// ST_MODIFIED
			ST_INVALID          // (M)-[BusRdX/WB]->(I)
		}
	},
	// shared
	{
		// EvPrRd
		{
			// ST_INVALID
			ST_SHARED,          // (I)-[PrRd/BusRd(S)->(S)
			// ST_SHARED
			ST_SHARED,          // (S)-[PrRd/..]->(S)
			// ST_EXCLUSIVE
			ST_EXCLUSIVE,       // (E)-[PrRd/..]->(E)
			// ST_MODIFIED
			ST_MODIFIED         // (M)-[PrRd/..]->(M)
		},
		// EvPrRdX
		{
			// ST_INVALID
			ST_EXCLUSIVE,       // (I)-[PrRdX/BusRdX->(E)
			// ST_SHARED
			ST_EXCLUSIVE,       // (S)-[PrRdX/BusRdX]->(E)
			// ST_EXCLUSIVE
			ST_EXCLUSIVE,       // (E)-[PrRdX/..]->(E)
			// ST_MODIFIED
			ST_EXCLUSIVE        // (M)-[PrRdX/..]->(M)
		},
		// EvPrWr
		{
			// ST_INVALID
			ST_MODIFIED,        // (I)-[PrWr/BusRdX]->(M)
			// ST_SHARED
			ST_MODIFIED,        // (S)-[PrWr/BusRdX]->(M)
			// ST_EXCLUSIVE
			ST_MODIFIED,        // (E)-[PrWr/..]->(M)
			// ST_MODIFIED
			ST_MODIFIED         // (M)-[PrWr/..]->(M)
		},
		// EvBusRd
		{
			// ST_INVALID
			ST_INVALID,         // (I)-[BusRd/..]->(I)
			// ST_SHARED
			ST_SHARED,          // (S)-[BusRd/WB]->(S)
			// ST_EXCLUSIVE
			ST_SHARED,          // (E)-[BusRd/WB]->(S)
			// ST_MODIFIED
			ST_SHARED           // (M)-[BusRd/WB]->(S)
		},
		// EvBusRdX
		{
			// ST_INVALID
			ST_INVALID,         // (I)-[BusRdX/..]->(I)
			// ST_SHARED
			ST_INVALID,         // (S)-[BusRdX/WB]->(I)
			// ST_EXCLUSIVE
			ST_INVALID,         // (E)-[BusRdX/WB]->(I)
			// ST_MODIFIED
			ST_INVALID          // (M)-[BusRdX/WB]->(I)
		}
	}
};


} // end of namespace mesi
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

