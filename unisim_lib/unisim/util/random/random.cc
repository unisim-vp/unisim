/*
 *  Copyright (c) 2009-2010,
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

#include <unisim/util/random/random.hh>

Random::Random(int32_t seed_x, int32_t seed_y, int32_t seed_z, int32_t seed_w)
	: x(seed_x)
	, y(seed_y)
	, z(seed_z)
	, w(seed_w)
	, carry(0)
{
}

int32_t Random::Generate()
{
	int32_t b;
	int32_t k;
	int32_t m;

	x = (69069 * x) + 1;
	y = y ^ (y << 13);
	y = y ^ (y >> 17);
	y = y ^ (y << 5);
	k = (z >> 2) + (w >> 3) + (carry >> 2);
	m = w + w + z + carry;
	z = w;
	w = m;
	carry = k >> 30;
	b = x + y + w;

	return b;
}

int32_t Random::Generate(uint32_t radius)
{
	// generate a random value between Min and Max
	int64_t r = generate();

	// compute current radius
	uint64_t cur_radius = (uint64_t) Max + 1;

	// scale
	r = r * radius / cur_radius;

	return (int32_t) r;
}
