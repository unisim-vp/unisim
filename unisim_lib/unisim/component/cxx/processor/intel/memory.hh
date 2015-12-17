#ifndef INTEL_MEMORY_HH
#define INTEL_MEMORY_HH

#include <algorithm>
#include <cstring>
#include <inttypes.h>

namespace intel {
  struct NoMemSet { void operator() ( uint8_t* base, uintptr_t size ) const {} };
  
  template <uint32_t LoTagSizeT, uint32_t HiTagSizeT, typename _MemSet = NoMemSet>
  struct Memory
  {
    _MemSet _memset;
    
    Memory()
      : _memset()
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        m_pages[idx] = 0;
      }
    }
    
    Memory( _MemSet const& __memset )
      : _memset( __memset )
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        m_pages[idx] = 0;
      }
    }
    
    ~Memory()
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        delete m_pages[idx];
      }
    }
    
    // Paginated content
    struct Page {
      static uint32_t const     s_bits = LoTagSizeT;

      Page*                     m_next;
      uint32_t                  m_address;
      uint8_t                   m_storage[1<<s_bits];

      Page( Page* _next, uint32_t _address, _MemSet const& _memset )
        : m_next( _next ), m_address( _address & uint32_t(-1 << s_bits) )
      { _memset( &m_storage[0], 1<<s_bits ); }
      ~Page() { delete m_next; }
    };

    static uint32_t const       s_bits = HiTagSizeT;
    Page*                       m_pages[1<<s_bits];
    
    // Memory access functions
    Page*
    getpage( uint32_t _addr )
    {
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);

      Page* match = m_pages[pageidx];
      while (match and match->m_address != pageaddr) match = match->m_next;
      return match ? match : (m_pages[pageidx] = new Page( m_pages[pageidx], pageaddr, this->_memset ));
    }

    Page*
    getpage_cache( uint32_t _addr )
    {
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      
      Page* match = m_pages[pageidx];
      if (not match or match->m_address == pageaddr) return match;
      Page* node = match;
      while (node->m_next and node->m_next->m_address != pageaddr)  node = node->m_next;
      match = node->m_next;
      if (not match) return 0;
      /* MRU replacement */
      node->m_next = match->m_next;
      match->m_next = m_pages[pageidx];
      m_pages[pageidx] = match;
      return match;
    }

    void
    read( uint8_t* _buffer, uint32_t _addr, uint32_t _size )
    {
      {
        uint32_t offset = _addr % (1 << Page::s_bits);
        if (offset) {
          uint32_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memcpy( (void*)_buffer, (void const*)(getpage( _addr )->m_storage + offset), size );
          _size -= size;
          _buffer += size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        uint32_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memcpy( (void*)_buffer, (void const*)(cur->m_storage), size );
        _size -= size;
        _buffer += size;
        _addr += size;
      }
    }
  
    void
    write( uint32_t _addr, uint8_t const* _buffer, uint32_t _size )
    {
      {
        uint32_t offset = _addr % (1 << Page::s_bits);
        if (offset) {
          uint32_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memcpy( (void*)(getpage( _addr )->m_storage + offset), (void const*)_buffer, size );
          _size -= size;
          _buffer += size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        uint32_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memcpy( (void*)(cur->m_storage), (void const*)_buffer, size );
        _size -= size;
        _buffer += size;
        _addr += size;
      }
    }

    void
    clear( uint32_t _addr, uint32_t _size )
    {
      {
        uint32_t offset = _addr % (1 << Page::s_bits);
        if( offset ) {
          uint32_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memset( (void*)(getpage( _addr )->m_storage + offset), 0, size );
          _size -= size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        uint32_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memset( (void*)(cur->m_storage), 0, size );
        _size -= size;
        _addr += size;
      }
    }
  };


};

#endif /* INTEL_MEMORY_HH */
