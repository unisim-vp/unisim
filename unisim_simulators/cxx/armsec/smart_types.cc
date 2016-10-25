#include <smart_types.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace armsec
{
  std::ostream&
  operator << (std::ostream& sink, Expr const& expr)
  {
    expr->Repr( sink );
    return sink;
  }
  
  std::ostream& ConstNodeBase::warn() { return std::cerr; }
  
  double BinaryXor( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float BinaryXor( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  double BinaryAnd( double l, double r ) { throw std::logic_error( "No & for double." ); }
  float BinaryAnd( float l, float r ) { throw std::logic_error( "No & for float." ); }
  
  double BinaryOr( double l, double r ) { throw std::logic_error( "No | for double." ); }
  float BinaryOr( float l, float r ) { throw std::logic_error( "No | for float." ); }
  
  double BinarySHL( double, uint8_t ) { throw std::logic_error( "No << for double." ); }
  float BinarySHL( float, uint8_t ) { throw std::logic_error( "No << for float." ); }
  
  double BinarySHR( double, uint8_t ) { throw std::logic_error( "No >> for double." ); }
  float BinarySHR( float, uint8_t ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t BSwp( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t BSwp( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }


  void
  BONode::Repr( std::ostream& sink ) const
  {
    switch (binop.code)
      {
      default:                sink << binop.c_str() << "( " << left << ", " << right << " )"; break;
        
      case BinaryOp::Add:     sink << "(" << left << " + " << right << ")"; break;
        
      case BinaryOp::Sub:     sink << "(" << left << " - " << right << ")"; break;
      }
  }

} // end of namespace armsec
