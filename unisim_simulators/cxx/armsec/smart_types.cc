#include <smart_types.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace unisim
{
  std::ostream& ConstNodeBase::warn() { return std::cerr; }
  
  double BinaryXor( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float BinaryXor( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  double BinaryAnd( double l, double r ) { throw std::logic_error( "No & for double." ); }
  float BinaryAnd( float l, float r ) { throw std::logic_error( "No & for float." ); }
  
  double BinaryOr( double l, double r ) { throw std::logic_error( "No | for double." ); }
  float BinaryOr( float l, float r ) { throw std::logic_error( "No | for float." ); }
  
  bool BinaryNot( bool val ) { return not val; }
  double BinaryNot( double val ) { throw std::logic_error( "No ~ for double." ); }
  float BinaryNot( float val ) { throw std::logic_error( "No ~ for float." ); }
  
  double BinarySHL( double, uint8_t ) { throw std::logic_error( "No << for double." ); }
  float BinarySHL( float, uint8_t ) { throw std::logic_error( "No << for float." ); }
  
  double BinarySHR( double, uint8_t ) { throw std::logic_error( "No >> for double." ); }
  float BinarySHR( float, uint8_t ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t BSwp( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t BSwp( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }
  

} // end of namespace armsec
