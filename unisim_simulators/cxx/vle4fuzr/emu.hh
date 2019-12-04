/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#ifndef __VLE4FUZR_EMU_HH__
#define __VLE4FUZR_EMU_HH__

#include <iosfwd>
#include <set>
#include <vector>
#include <cassert>
#include <cstdint>

struct Processor
{
  Processor();
  
  virtual ~Processor();

  struct Abort { virtual ~Abort() {}; virtual void dump(std::ostream&); };

  struct Page
  {
    typedef uint64_t (*hook_t) (void* uc, unsigned access, uint64_t address, unsigned size, unsigned endianness, uint64_t value);
    
    uint64_t hi() const { return base + size - 1; }

    bool operator < (Page const& p) const { return hi() < p.base; }
    bool operator > (Page const& p) const { return p.hi() < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, unsigned b) const { return b < a.base; }
    };
    Page( Page&& page )
      : base(page.base), size(page.size), data(page.data), hook(page.hook), perms(page.perms)
    {
      page.data = 0;
    }
    Page( Page const& ) = delete;
    Page( uint64_t addr, uint64_t size, unsigned _perms, hook_t _hook )
      : base(addr), size(size), data(_perms ? new uint8_t[size] : 0), hook(_hook), perms(_perms)
    {}
    ~Page()
    {
      delete [] data;
    }
    uint64_t len() const { return size; }
    uint64_t write(uint64_t addr, uint8_t const* bytes, uint64_t size) const
    {
      addr -= base;
      uint64_t count = std::min(len()-addr,size);
      std::copy( &bytes[0], &bytes[count], &data[addr] );
      return count;
    }
    bool write(uint64_t addr, unsigned size, unsigned endianness, uint64_t value) const
    {
      uint8_t bytes[8];
      for (unsigned idx = 0; idx < size; ++idx)
        { bytes[idx^endianness] = value; value >>= 8; }
      return write( addr, &bytes[0], size ) == size;
    }
    bool phys_write(Processor* p, uint64_t addr, unsigned size, unsigned endianness, uint64_t value) const
    {
      if (data and access( Write ))
        return write(addr, size, endianness, value);
      if (not hook) return false;
      hook(p, 1, addr, size, endianness, value);
      return true;
    }
    uint64_t read(uint64_t addr, uint8_t* bytes, uint64_t size) const
    {
      addr -= base;
      uint64_t count = std::min(len()-addr,size);
      std::copy( &data[addr], &data[addr+count], &bytes[0] );
      return count;
    }
    bool read(uint64_t addr, unsigned size, unsigned endianness, uint64_t* value) const
    {
      uint8_t bytes[8];
      if (read(addr, &bytes[0], size) != size) return false;
      uint64_t res = 0;
      for (unsigned idx = size; idx-- != 0;)
        { res <<= 8; res |= bytes[idx^endianness]; }
      *value = res;
      return true;
    }
    bool phys_read(Processor* p, uint64_t addr, unsigned size, unsigned endianness, uint64_t* value) const
    {
      if (data and access( Read ))
        return read(addr, size, endianness, value);
      if (not hook) return false;
      *value = hook(p, 0, addr, size, endianness, 0);
      return true;
    }
    bool phys_fetch(Processor* p, uint64_t addr, unsigned size, unsigned endianness, uint64_t* value) const
    {
      if (data and access( Execute ))
        return read(addr, size, endianness, value);
      if (not hook) return false;
      *value = hook(p, 2, addr, size, endianness, 0);
      return true;
    }
    
    void chperms(unsigned new_perms) const { const_cast<unsigned&>(perms) = new_perms; }
    void chhook(hook_t new_hook) const { const_cast<hook_t&>(hook) = new_hook; }
    enum Permision { Read = 1, Write = 2, Execute = 4 };
    bool access(unsigned _perms) const { return (_perms & perms) == _perms; }
    bool has_data() const { return data; }

    void dump(std::ostream&) const;
    friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }

  public:
    uint64_t  base;
  private:
    uint64_t  size;
    uint8_t*  data;
    hook_t    hook;
    unsigned  perms;
  };
  
  typedef std::set<Page, Page::Above> Pages;
  Pages pages;

  void error_mem_overlap( Page const& a, Page const& b );
  
  Pages::iterator mem_map(uint64_t addr, uint64_t size, unsigned perms, Page::hook_t hook)
  {
    Page page(addr, size, perms, hook);
    auto below = pages.lower_bound(page);
    if (below != pages.end() and not (*below < page))
      {
        error_mem_overlap(page, *below);
        return pages.end();
      }
    if (pages.size() and below != pages.begin() and not (page < *std::prev(below)))
      {
        error_mem_overlap(page, *std::prev(below));
        return pages.end();
      }
    
    return pages.insert(below,std::move(page));
  }

  
  void error_page_at( char const* issue, uint64_t addr );
  
  int
  mem_chprot(uint64_t addr, unsigned perms)
  {
    auto page = pages.lower_bound(addr);
    if (page == pages.end() or page->hi() < addr)
      return error_page_at("no", addr), -1;
    page->chperms( perms );
    return 0;
  }

  int
  mem_chhook(uint64_t addr, Page::hook_t hook)
  {
    auto page = pages.lower_bound(addr);
    if (page == pages.end() or page->hi() < addr)
      return error_page_at("no", addr), -1;
    page->chhook( hook );
    return 0;
  }

  int
  mem_write(uint64_t addr, uint8_t const* bytes, uint64_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
        if (not pi->has_data())
          return error_page_at("cannot write hooked", addr), -1;
        uintptr_t count = pi->write(addr, bytes, size);
        if (count >= size) break;
        addr += count;
        bytes += count;
        size -= count;
      }
    
    return 0;
  }

  int
  mem_read(uint64_t addr, uint8_t* bytes, uint64_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
        if (not pi->has_data())
          return error_page_at("cannot read hooked", addr), -1;
        uintptr_t count = pi->read(addr, bytes, size);
        if (count >= size)
          return 0;
        addr += count;
        bytes += count;
        size -= count;
      }
       
    throw 0;
    return 0;
  }

  bool PhysicalWriteMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t value )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->hi() < addr)
      return error_page_at("write to nonexistent", addr), false;
    return pi->phys_write(this, addr, size, endianness, value);
  }

  bool PhysicalReadMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t* value )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->hi() < addr)
      return error_page_at("read to nonexistent", addr), false;
    return pi->phys_read(this, addr, size, endianness, value);
  }
  bool PhysicalFetchMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t* value )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->hi() < addr)
      return error_page_at("fetch to nonexistent", addr), false;
    return pi->phys_fetch(this, addr, size, endianness, value);
  }
  struct RegView
  {
    virtual ~RegView() {}
    virtual void write( Processor& proc, int id, uint64_t bytes ) const = 0;
    virtual void read( Processor& proc, int id, uint64_t* bytes ) const = 0;
  };

  virtual RegView const* get_reg(char const* id, uintptr_t size) = 0;
  
  int
  reg_write(char const* id, uintptr_t size, int regid, uint64_t value)
  {
    RegView const* rv = get_reg(id, size);
    if (not rv) return -1;
    rv->write(*this, regid, value);
    return 0;
  }
  
  int
  reg_read(char const* id, uintptr_t size, int regid, uint64_t* value)
  {
    RegView const* rv = get_reg(id, size);
    if (not rv) return -1;
    rv->read(*this, regid, value);
    return 0;
  }
  
  struct Hook
  {
    typedef void (*cb_code)(void* uc, uint64_t address, unsigned size);
    typedef void (*cb_intr)(void* uc, uint32_t intno);
    
    Hook(unsigned _types, void* cb, uint64_t _begin, uint64_t _end)
      : types(_types), begin(_begin), end(_end), callback(cb), insn()
    {}

    enum type_t
      {
       INTR = 0, // Hook all interrupt/syscall events
       // INSN = 1 ILLEGAL, // Hook a particular instruction - only a very small subset of instructions supported here
       CODE = 2, // Hook a range of code
       BLOCK = 3, // Hook basic blocks
       MEM = 4, // Hook for memory access on unmapped memory or protected memory
       TYPE_COUNT
      };
    

    template <unsigned BITS> struct Mask
    {
      enum { bits = BITS };
      template <class T> Mask<T::bits | bits> operator | (T const&) { return Mask<T::bits | bits>(); }
      bool check( unsigned types ) { return bool(types&unsigned(bits)) ^ bool(types&~unsigned(bits));}
    };
    template <type_t TYPE> Mask<1<<TYPE> Is() { return Mask<1<<TYPE>(); }
      
    bool check_types();
    
    template <typename T> T cb() const { return (T)callback; }
    bool has_type( type_t tp ) { return (types >> int(tp)) & 1; }
    void release( type_t tp )
    {
      types &= ~(1u << tp);
      if (not types)
        delete this;
    }
    void release( int tp ) { release(type_t(tp)); }

    bool bound_check(uint64_t addr) { return (addr >= begin and addr <= end) or begin > end; }
    
  private:
    uintptr_t refs;
    
  public:
    unsigned types;
    uint64_t begin, end;
    void* callback;
    void* user_data;
    int insn;
  };

  bool add( Hook* hook );
  void insn_hooks(uint64_t addr, unsigned len);
  
  std::vector<Hook*> hooks[Hook::TYPE_COUNT];

  bool disasm;
  bool bblock;
  bool terminated;
  
  void set_disasm(bool _disasm) { disasm = _disasm; }
  virtual int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count ) = 0;
};

struct BranchInfo
{
  enum { BNone = 0, Direct, Indirect };
  
  BranchInfo() : address(), target(BNone), pass(false) {}
  template <class X> void update( bool branch, X const& x ) { update( branch, x.determined, x.value ); }
  void update( bool branch, bool known, uint64_t target );
  
  uint64_t address;
  unsigned target : 2;
  unsigned pass : 1;
};

template <class OP, uint64_t SZ>
struct OpPage
{
  enum { size = SZ };
  OP* ops[SZ];
  OpPage() : ops() {}
};

template <unsigned N>
bool regname( char const (&ref)[N], char const* id, uintptr_t size )
{
  return size == (N-1) and std::equal( &ref[0], &ref[N-1], id );
}

#endif /* __VLE4FUZR_EMU_HH__ */

