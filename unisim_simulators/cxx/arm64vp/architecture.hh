/*
 *  Copyright (c) 2019-2020,
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

#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <iosfwd>
#include <set>
#include <algorithm>
#include <inttypes.h>

struct AArch64
{
  AArch64();

  struct Page
  {
    uint64_t hi() const { return last; }

    bool operator < (Page const& p) const { return last < p.base; }
    bool operator > (Page const& p) const { return p.last < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, uint64_t b) const { return a.base > b; }
    };
    struct Free
    {
      virtual ~Free() {};
      virtual void free (Page&) const {};
      static Free nop;
    };
    Page( uint64_t _base, uint64_t _last, uint8_t* _data, Free* _free )
      : base(_base), last(_last), data(_data), free(_free)
    {}
    Page( Page&& page )
      : base(page.base), last(page.last), data(page.data), free(page.free)
    {
      page.data = 0;
      page.free = &Free::nop;
    }
    Page( Page const& ) = delete;
    ~Page();
    
    uint64_t write(uint64_t addr, uint8_t const* bytes, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &bytes[0], &bytes[cnt], &data[start] );
      return cnt;
    }
    bool write(uint64_t addr, unsigned count, unsigned endianness, uint64_t value) const
    {
      uint8_t bytes[8];
      for (unsigned idx = 0; idx < count; ++idx)
        { bytes[idx^endianness] = value; value >>= 8; }
      return write( addr, &bytes[0], count ) == count;
    }
    uint64_t read(uint64_t addr, uint8_t* bytes, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &data[start], &data[start+cnt], &bytes[0] );
      return cnt;
    }
    bool read(uint64_t addr, unsigned count, unsigned endianness, uint64_t* value) const
    {
      uint8_t bytes[8];
      if (read(addr, &bytes[0], count) != count) return false;
      uint64_t res = 0;
      for (unsigned idx = count; idx-- != 0;)
        { res <<= 8; res |= bytes[idx^endianness]; }
      *value = res;
      return true;
    }
    bool has_data() const { return data; }
    uint8_t const* get_data() const { return data; }
    uint64_t size() const { return last - base + 1; }

    // void dump(std::ostream&) const;
    // friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }


  public:
    uint64_t    base;
    uint64_t    last;
  private:
    uint8_t*    data;
    Page::Free* free;

    void resize(uint64_t last);
  };

  typedef std::set<Page, Page::Above> Pages;

  Page const& access_page( uint64_t addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      { struct Bad {}; throw Bad(); }
    return *pi;
  }
  
  void free_read_memory(uint64_t address, uint8_t* buffer, unsigned size)
  {
    Page const& page = access_page(address);
    page.read(address, buffer, size);
  }

  struct IPB
  {
    static unsigned const LINE_SIZE = 32; //< IPB size
    uint8_t bytes[LINE_SIZE];             //< The IPB content
    uint32_t base_address;                //< base address of IPB content (cache line size aligned if valid)
    IPB() : bytes(), base_address( -1 ) {}
    uint8_t* get(AArch64& core, uint64_t address);
  };
  
  void error_mem_overlap( Page const& a, Page const& b );
  
  bool mem_map(uint64_t addr, uint64_t size);
  bool mem_map(Page&& page);

  void step_instruction();

  Pages    pages;
  IPB      ipb;
  unisim::component::cxx::processor::arm::isa::arm64::Decoder<AArch64> decoder;
  uint64_t current_insn_addr, next_insn_addr;
};

