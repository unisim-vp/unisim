#include <smart_types.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace armsec
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
  
  void DumpConstant( std::ostream& sink, bool v ) { sink << int(v) << "<1>"; }


  int
  BONode::GenCode( Label& label, std::ostream& sink ) const
  {
    int retsz;
    sink << '(' << left.GetCode(label,retsz);
    
    switch (binop.code)
      {
      default:                sink << " [" << binop.c_str() << "] "; break;
        
      case BinaryOp::Add:     sink << " + "; break;
        
      case BinaryOp::Sub:     sink << " - "; break;
        
      case BinaryOp::Xor:     sink << " xor "; break;
      case BinaryOp::Or:      sink << " or "; break;
      case BinaryOp::And:     sink << " and "; break;
        
      case BinaryOp::Teq:     sink << " = "; break;
      case BinaryOp::Tne:     sink << " <> "; break;
        
      case BinaryOp::Tle:     sink << " <=s "; break;
      case BinaryOp::Tleu:    sink << " <=u "; break;
        
      case BinaryOp::Tge:     sink << " >=s "; break;
      case BinaryOp::Tgeu:    sink << " >=u "; break;
        
      case BinaryOp::Tlt:     sink << " <s "; break;
      case BinaryOp::Tltu:    sink << " <u "; break;
        
      case BinaryOp::Tgt:     sink << " >s "; break;
      case BinaryOp::Tgtu:    sink << " >u "; break;
      
      case BinaryOp::Lsl:     sink << " lshift "; break;
      case BinaryOp::Asr:     sink << " rshifts "; break;
      case BinaryOp::Lsr:     sink << " rshiftu "; break;
      case BinaryOp::Ror:     sink << " rrotate "; break;
      case BinaryOp::Mul:     sink << " * "; break;
        // case BinaryOp::Mod: break;
        // case BinaryOp::Div: break;
      }
    
    sink << right.GetCode(label) << ')';
    return retsz;
  }
  
  int
  UONode::GenCode( Label& label, std::ostream& sink ) const
  {
    sink << '(';
    switch (unop.code)
      {
      default:              sink << "[" << unop.c_str() << "] "; break;
        
      case UnaryOp::Not:    sink << "not "; break;
      case UnaryOp::Neg:    sink << "- "; break;
        
        // case UnaryOp::BSwp:  break;
        // case UnaryOp::BSR:   break;
        // case UnaryOp::BSF:   break;
      }
    
    int retsz = 0;
    sink << src.GetCode(label,retsz) << ')';
    return retsz;
  }


} // end of namespace armsec
