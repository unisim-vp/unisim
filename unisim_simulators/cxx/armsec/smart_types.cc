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
  
  bool BinaryNot( bool val ) { return not val; }
  double BinaryNot( double val ) { throw std::logic_error( "No ~ for double." ); }
  float BinaryNot( float val ) { throw std::logic_error( "No ~ for float." ); }
  
  double BinarySHL( double, uint8_t ) { throw std::logic_error( "No << for double." ); }
  float BinarySHL( float, uint8_t ) { throw std::logic_error( "No << for float." ); }
  
  double BinarySHR( double, uint8_t ) { throw std::logic_error( "No >> for double." ); }
  float BinarySHR( float, uint8_t ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t BSwp( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t BSwp( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }

  unsigned DumpConstant( std::ostream& sink, bool v ) { sink << int(v) << "<1>"; }


  void
  BONode::Repr( std::ostream& sink ) const
  {
    switch (binop.code)
      {
      default:                sink << binop.c_str() << "( " << left << ", " << right << " )"; break;
        
      case BinaryOp::Add:     sink << "(" << left << " + " << right << ")"; break;
        
      case BinaryOp::Sub:     sink << "(" << left << " - " << right << ")"; break;
        
      case BinaryOp::Xor:     sink << "(" << left << " xor " << right << ")"; break;
      case BinaryOp::Or:      sink << "(" << left << " or " << right << ")"; break;
      case BinaryOp::And:     sink << "(" << left << " and " << right << ")"; break;
        
      case BinaryOp::Teq:     sink << "(" << left << " = " << right << ")"; break;
      case BinaryOp::Tne:     sink << "(" << left << " <> " << right << ")"; break;
        
      case BinaryOp::Tle:     sink << "(" << left << " <=s " << right << ")"; break;
      case BinaryOp::Tleu:    sink << "(" << left << " <=u " << right << ")"; break;
        
      case BinaryOp::Tge:     sink << "(" << left << " >=s " << right << ")"; break;
      case BinaryOp::Tgeu:    sink << "(" << left << " >=u " << right << ")"; break;
        
      case BinaryOp::Tlt:     sink << "(" << left << " <s " << right << ")"; break;
      case BinaryOp::Tltu:    sink << "(" << left << " <u " << right << ")"; break;
        
      case BinaryOp::Tgt:     sink << "(" << left << " >s " << right << ")"; break;
      case BinaryOp::Tgtu:    sink << "(" << left << " >u " << right << ")"; break;
        
        // case BinaryOp::SHL: break;
        // case BinaryOp::SHR: break;
        // case BinaryOp::ROR: break;
        // case BinaryOp::Mod: break;
        // case BinaryOp::Mul: break;
        // case BinaryOp::Div: break;
      }
  }
  
  void
  UONode::Repr( std::ostream& sink ) const
  {
    switch (unop.code)
      {
      default:                sink << unop.c_str() << "( " << src << " )"; break;
        
      case UnaryOp::Not:    sink << "(not " << src << ")"; break;
      case UnaryOp::Neg:      sink << "(- " << src << ")"; break;
        
        // case UnaryOp::BSwp:  break;
        // case UnaryOp::BSR:   break;
        // case UnaryOp::BSF:   break;
      }
  }


} // end of namespace armsec
