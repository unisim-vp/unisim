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
    std::vector<uint8_t> storage;
    uint32_t base;
    unsigned perms;

    uint32_t hi() const { return base + storage.size(); }

    bool operator < (Page const& p) const { return hi() <= p.base; }
    bool operator > (Page const& p) const { return p.hi() <= base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, unsigned b) const { return b < a.base; }
    };

    Page( uint32_t addr, uint32_t size, unsigned _perms )
      : storage( size ), base(addr), perms(_perms)
    {}
    uint32_t len() const { return storage.size(); }
    uint32_t write(uint32_t addr, uint8_t const* bytes, uint32_t size) const
    {
      if (addr < base) throw 0;
      addr -= base;
      if (addr > len()) throw 0;
      uint32_t count = std::min(len()-addr,size);
      std::copy( &bytes[0], &bytes[count], at(addr) );
      return count;
    }
    uint32_t read(uint32_t addr, uint8_t* bytes, uint32_t size) const
    {
      if (addr < base) throw 0;
      addr -= base;
      if (addr > len()) throw 0;
      uint32_t count = std::min(len()-addr,size);
      std::copy( at(addr), at(addr+count), &bytes[0] );
      return count;
    }
    uint8_t* at(uint32_t x) const { return const_cast<uint8_t*>(&storage[x]); }
    void chperms(unsigned new_perms) const { const_cast<unsigned&>(perms) = new_perms; }
    enum Permision { Read = 1, Write = 2, Execute = 4 };
    void access(unsigned _perms) const { if ((_perms & perms) != _perms) throw Abort(); }

    void dump(std::ostream&) const;
    friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }
  };
  
  typedef std::set<Page, Page::Above> Pages;
  Pages pages;

  Pages::iterator mem_page(uint32_t addr, uint32_t size)
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_pagemiss(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_pagemiss(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    return pi;
  }

  Pages::iterator mem_pagemiss(uint32_t lo, uint32_t addr, uint32_t hi)
  {
    uint32_t const page_size = 4096;
    uint32_t page_lo = std::max(addr & (0u-page_size), lo);
    uint32_t page_hi = std::min(addr | (page_size-1u), hi);
    auto pi = mem_map(page_lo,page_hi-page_lo+1,7);
    assert( pi != pages.end() );
    return pi;
  }

  void mem_overlap_error( Page const& a, Page const& b );
  
  Pages::iterator mem_map(uint32_t addr, uint32_t size, unsigned perms)
  {
    Page page(addr, size, perms);
    auto below = pages.lower_bound(page);
    if (below != pages.end() and not (*below < page))
      {
        mem_overlap_error(page, *below);
        return pages.end();
      }
    if (pages.size() and below != pages.begin() and not (page < *std::prev(below)))
      {
        mem_overlap_error(page, *std::prev(below));
        return pages.end();
      }
    
    return pages.insert(below,std::move(page));
  }

  int
  mem_prot(uint32_t addr, unsigned perms)
  {
    auto page = pages.lower_bound(addr);
    if (page == pages.end() or page->hi() <= addr)
      return -1;
    page->chperms( perms );
    return 0;
  }

  int
  mem_write(uint32_t addr, uint8_t const* bytes, uint32_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
        uintptr_t count = pi->write(addr, bytes, size);
        if (count >= size)
          return 0;
        addr += count;
        bytes += count;
        size -= count;
      }
    
    throw 0;
    return 0;
  }

  int
  mem_read(uint32_t addr, uint8_t* bytes, uint32_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
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

  bool PhysicalWriteMemory( uint32_t addr, uint8_t const* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Write);
    uint32_t pos = addr - pi->base;
    std::copy(&buffer[0], &buffer[size], pi->at(pos));
    return true;
  }

  bool PhysicalReadMemory( uint32_t addr, uint8_t* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Read);
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  bool PhysicalFetchMemory( uint32_t addr, uint8_t* buffer, uint32_t size )
  {
    auto pi = mem_page(addr, size);
    if (pi == pages.end()) return false;
    pi->access(pi->Execute);
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  struct RegView
  {
    virtual ~RegView() {}
    virtual void write( Processor& proc, int id, uint8_t const* bytes ) const = 0;
    virtual void read( Processor& proc, int id, uint8_t* bytes ) const = 0;
  };

  virtual RegView const* get_reg(char const* id, uintptr_t size) = 0;
  
  int
  reg_write(char const* id, uintptr_t size, int regid, uint8_t const* bytes)
  {
    RegView const* rv = get_reg(id, size);
    if (not rv) return -1;
    rv->write(*this, regid, bytes);
    return 0;
  }
  
  int
  reg_read(char const* id, uintptr_t size, int regid, uint8_t* bytes)
  {
    RegView const* rv = get_reg(id, size);
    if (not rv) return -1;
    rv->read(*this, regid, bytes);
    return 0;
  }
  
  struct Hook
  {
    typedef void (*cb_code)(void* uc, uint64_t address, uint32_t size);
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
       MEM_READ_UNMAPPED = 4, // Hook for memory read on unmapped memory
       MEM_WRITE_UNMAPPED = 5, // Hook for invalid memory write events
       MEM_FETCH_UNMAPPED = 6, // Hook for invalid memory fetch for execution events
       MEM_READ_PROT = 7, // Hook for memory read on read-protected memory
       MEM_WRITE_PROT = 8, // Hook for memory write on write-protected memory
       MEM_FETCH_PROT = 9, // Hook for memory fetch on non-executable memory
       MEM_READ = 10, // Hook memory read events.
       MEM_WRITE = 11, // Hook memory write events.
       MEM_FETCH = 12, // Hook memory fetch for execution events
       MEM_READ_AFTER = 13, // Hook memory after successful read events.
       INSN_INVALID = 14, // Hook invalid instructions exceptions.
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

    bool bound_check(uint32_t addr) { return (addr >= begin and addr <= end) or begin > end; }
    
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
  void insn_hooks(uint64_t addr, uint64_t len);
  
  std::vector<Hook*> hooks[Hook::TYPE_COUNT];

  bool disasm;
  bool bblock;
  
  void set_disasm(bool _disasm) { disasm = _disasm; }
  virtual int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count ) = 0;
};

struct Branch
{
  Branch() : address(), target(BNone), pass(false) {}
  enum { BNone = 0, Direct, Indirect };
  uint32_t address;
  unsigned target : 2;
  unsigned pass : 1;
};

template <class ARCH> struct InsnBranch : public Branch {};

template <class OP, uint32_t SZ>
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
