#include <intel/arch.hh>
#include <intel/isa.tcc>
#include <intel/math.hh>
#include <dtlib/misc.hh>
#include <cmath>

namespace intel
{
  Arch::Arch()
    : m_running( true ), m_instcount( 0 ), m_disasm( false ), m_latest_insn( 0 )
    , m_target( *this ), m_EIP( 0 ), m_ftop( 0 ), m_fcw( 0x23f ),
      m_dirtfregs( 0 )
  {
    std::memset( static_cast<void*>( &m_flags[0] ), 0, sizeof (m_flags) );
    std::memset( static_cast<void*>( &m_regs[0] ), 0, sizeof (m_regs) );
    std::memset( static_cast<void*>( &m_fregs[0] ), 0, sizeof (m_fregs) );
  }
  
  template <typename DECODER>
  struct ICache : public DECODER
  {
    struct Page
    {
      static unsigned const NUM_OPERATIONS_PER_PAGE = 0x1000;
      Page* next;
      uint32_t key;
      Operation* operations[NUM_OPERATIONS_PER_PAGE];
      uint8_t bytes[NUM_OPERATIONS_PER_PAGE+15];
      
      Page( Page* _next, uint32_t _key ) : next( _next ), key( _key ) { memset( operations, 0, sizeof (operations) ); }
      ~Page() { for( unsigned idx = 0; idx < NUM_OPERATIONS_PER_PAGE; ++idx ) delete operations[idx]; delete next; }
    };

    static unsigned const NUM_HASH_TABLE_ENTRIES = 0x1000;
    Page* hash_table[NUM_HASH_TABLE_ENTRIES];
    Page* mru_page;
    ICache() : mru_page( 0 ) { memset( hash_table, 0, sizeof (hash_table) ); }
    ~ICache() { for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx) delete hash_table[idx]; }
    
    Operation* Get( Mode mode, uint32_t address, uint8_t* bytes )
    {
      uint32_t offset;
      Page* page = GetPage( address, offset );
      Operation* operation = page->operations[offset];
      
      if (operation) {
        if ((operation->mode == mode) and (memcmp( &bytes[0], &page->bytes[offset], operation->length ) == 0))
          return operation;
        delete operation;
      }
      
      page->operations[offset] = operation = this->Decode( mode, address, bytes );
      memcpy( &page->bytes[offset], &bytes[0], operation->length );
      
      return operation;
    }
    
  private:
    Page*
    GetPage( uint32_t address, uint32_t& offset )
    {
      uint32_t page_key = address / Page::NUM_OPERATIONS_PER_PAGE;
      offset = address & (Page::NUM_OPERATIONS_PER_PAGE-1);
      
      if (mru_page and (mru_page->key == page_key)) return mru_page;
      
      unsigned index = page_key % NUM_HASH_TABLE_ENTRIES; // hash the key
      
      Page* cur = hash_table[index];
      Page** ref = &hash_table[index];
      
      while (cur) {
        if (cur->key == page_key) {

          *ref = cur->next;
          cur->next = hash_table[index];
          return (mru_page = hash_table[index] = cur);
        }
        ref = &cur->next;
        cur = cur->next;
      }
      
      return (mru_page = hash_table[index] = new Page( hash_table[index], page_key ));
    }
  };

  struct Decoder
  {
    Operation* Decode( Mode mode, uint32_t address, uint8_t* bytes )
    {
      if (Operation* op = getoperation( CodeBase( mode, address, bytes ) ))
        return op;
      
      std::cerr << "No decoding for " << DisasmBytes( bytes, 16 ) << " @" << std::hex << address << std::endl;
      throw 0;
      return 0;
    }
  };
  

  Operation*
  Arch::fetch()
  {
    static ICache<Decoder> icache;
    uint32_t eip = m_EIP;
    uint8_t decbuf[15];
    lla_memcpy( decbuf, eip, sizeof (decbuf) );
    Mode mode( 0, 1, 1 );
    
    Operation* operation = icache.Get( mode, eip, &decbuf[0] );
    
    m_EIP = eip + operation->length;
    
    if (m_disasm) {
      dtlib::osprintf( m_events(), "%#010x: ", eip );
      operation->disasm( m_events() );
      m_events() << " (" << DisasmBytes( &decbuf[0], operation->length ) << ")" << std::endl;
    }
    
    ++m_instcount;
    return operation;
  }

  f64_t sine( f64_t angle ) { return sin( angle ); }

  f64_t cosine( f64_t angle ) { return cos( angle ); }

  f64_t tangent( f64_t angle ) { return tan( angle ); }

  f64_t arctan( f64_t angle ) { return atan( angle ); }

  f64_t fmodulo( f64_t dividend, f64_t modulus ) { return fmod( dividend, modulus ); }

  f64_t firound( f64_t value, int x87frnd_mode )
  {
    // double floor_res = floor( value );                                                                                                                                                     
    double result = intel::x87frnd( value, intel::x87frnd_mode_t( x87frnd_mode ) );
    // if (floor_res != result) {                                                                                                                                                             
    //   std::cerr << "Rounding result fixed; old: " << floor_res << ", new: " << result << std::endl;                                                                                        
    // }                                                                                                                                                                                      

    return result;
  }

  f64_t power( f64_t exponent, f64_t value ) { return pow( exponent, value ); }

  f64_t logarithm( f64_t value ) { return log2( value ); }

  f64_t square_root( f64_t value ) { return sqrt( value ); }
}
