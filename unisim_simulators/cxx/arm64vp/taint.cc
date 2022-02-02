/*
 *  Copyright (c) 2019-2021,
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

#include <taint.hh>
#include <iostream>
#include <cmath>

void PrintBin( std::ostream& sink, uint64_t vbits, uint64_t ubits ) { Print(sink << "0b", 0, 1, vbits, ubits); }
void PrintHex( std::ostream& sink, unsigned ml, uint64_t vbits, uint64_t ubits ) { Print(sink << "0x", ml, 4, vbits, ubits); }

void Print( std::ostream& sink, unsigned minlength, unsigned logradix, uint64_t vbits, uint64_t ubits )
{
  struct
  {
    void recurse( std::ostream& sink, unsigned left, unsigned bsz, uint64_t vbits, uint64_t ubits )
    {
      {
        uint64_t _vbits = vbits >> bsz, _ubits = ubits >> bsz;
        left = left ? left - 1 : 0;
        if (_vbits | _ubits | left)
          recurse( sink, left, bsz, _vbits, _ubits );
      }
      uint64_t mask = (1<<bsz)-1;
      sink << (ubits & mask ? 'X' : "0123456789abcdef"[vbits & mask] );
    }
  } _;
  
  _.recurse(sink, minlength, logradix, vbits, ubits);
}

TaintedValue<float>  fabs(TaintedValue<float> const& v) { return TaintedValue<float>(fabsf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> fabs(TaintedValue<double> const& v)  { return TaintedValue<double>(fabs(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  ceil(TaintedValue<float> const& v) { return TaintedValue<float>(ceilf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> ceil(TaintedValue<double> const& v)  { return TaintedValue<double>(ceil(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  floor(TaintedValue<float> const& v) { return TaintedValue<float>(floorf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> floor(TaintedValue<double> const& v)  { return TaintedValue<double>(floor(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  trunc(TaintedValue<float> const& v) { return TaintedValue<float>(truncf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> trunc(TaintedValue<double> const& v)  { return TaintedValue<double>(trunc(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  round(TaintedValue<float> const& v) { return TaintedValue<float>(roundf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> round(TaintedValue<double> const& v)  { return TaintedValue<double>(round(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  nearbyint(TaintedValue<float> const& v) { return TaintedValue<float>(nearbyintf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> nearbyint(TaintedValue<double> const& v)  { return TaintedValue<double>(nearbyint(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  sqrt(TaintedValue<float> const& v) { return TaintedValue<float>(sqrtf(v.value), v.ubits ? -1 : 0); }
TaintedValue<double> sqrt(TaintedValue<double> const& v)  { return TaintedValue<double>(sqrt(v.value), v.ubits ? -1 : 0); }

TaintedValue<float>  fmin(TaintedValue<float> const& l, TaintedValue<float> const& r) { return TaintedValue<float>(fminf(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0); }
TaintedValue<double> fmin(TaintedValue<double> const& l, TaintedValue<double> const& r)  { return TaintedValue<double>(fmin(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0); }

TaintedValue<float>  fmax(TaintedValue<float> const& l, TaintedValue<float> const& r) { return TaintedValue<float>(fmaxf(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0); }
TaintedValue<double> fmax(TaintedValue<double> const& l, TaintedValue<double> const& r)  { return TaintedValue<double>(fmax(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0); }

