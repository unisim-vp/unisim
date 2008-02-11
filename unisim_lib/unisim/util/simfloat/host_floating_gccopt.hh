/***************************************************************************
                Floating_gccopt.h  -  Template for various types of floating point computations
                             -------------------
    first release        : 15 Jul 2005
    copyright ©          : (C) 2004-2005 CEA
    authors              : Franck Védrine, Bruno Marre, Benjamin Blanc, Gilles Mouchard
    email                : Franck.Vedrine@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Lesser General Public License      *
 * as published by the Free Software Foundation; either version 2.1        *
 * of the License, or (at your option) any later version.                  *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Lesser General Public License for more details.                     *
 *                                                                         *
 ***************************************************************************/

#ifndef Numerics_Double_HostFloatingH
#define Numerics_Double_HostFloatingH

#include <unisim/util/simfloat/floating_gccopt.hh>

namespace unisim {
namespace util {
namespace simfloat {

namespace Numerics { namespace Double {

/***************************************/
/* Definition - template TFloatingBase */
/***************************************/

class DoubleTraits {
  public:
   typedef double TypeFloat;
   //typedef unsigned long int TypeIntegerConversion; // sizeof(TypeFloat) == sizeof(TypeIntegerConversion)
   inline static double max() GCC_INLINE;              
   inline static double normalizedMin() GCC_INLINE;    
   inline static double denormalizedMin() GCC_INLINE;  
   inline static bool isBigEndian() GCC_INLINE;
   inline static bool isLittleEndian() GCC_INLINE; 

   static const int UBitSizeMantissa = DBL_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-DBL_MANT_DIG;
   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline static double epsilon() GCC_INLINE; 
   inline static bool isValid() GCC_INLINE; 
};

class FloatTraits {
  public:
   typedef float TypeFloat;
   //typedef unsigned int TypeIntegerConversion; // sizeof(TypeFloat) == sizeof(TypeIntegerConversion)
   inline static float max() GCC_INLINE;              
   inline static float normalizedMin() GCC_INLINE;    
   inline static float denormalizedMin() GCC_INLINE;  
   inline static bool isBigEndian() GCC_INLINE;
   inline static bool isLittleEndian() GCC_INLINE; 

   static const int UBitSizeMantissa = FLT_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-FLT_MANT_DIG;
   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline static float epsilon() GCC_INLINE; 
   inline static bool isValid() GCC_INLINE; 
};

class LongDoubleTraits {
  public:
   typedef long double TypeFloat;
   //typedef unsigned int TypeIntegerConversion; // sizeof(TypeFloat) == sizeof(TypeIntegerConversion)
   inline static long double max() GCC_INLINE;              
   inline static long double normalizedMin() GCC_INLINE;    
   inline static long double denormalizedMin() GCC_INLINE;  
   inline static bool isBigEndian() GCC_INLINE;
   inline static bool isLittleEndian() GCC_INLINE; 

   static const int UBitSizeMantissa = LDBL_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-LDBL_MANT_DIG;
   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline static long double epsilon() GCC_INLINE; 
   inline static bool isValid() GCC_INLINE; 
};

template <class TypeTraits>
class TFloatingBase {
  public:
   typedef TBuiltDouble<BuiltDoubleTraits<TypeTraits::UBitSizeMantissa, TypeTraits::UBitSizeExponent> >
      BuiltDouble;

  protected:
   typedef typename TypeTraits::TypeFloat Implantation;
   typedef TFloatingBase<TypeTraits> thisType;

  private:
   Implantation dDouble;
   static void test_double(double dDouble);
   static void test_int(unsigned int uInt);
   static void test_char(char cChar);
   static void copy_mem(void* pvDestination, void* pvSource, int uBytes);

  public:
   inline TFloatingBase() GCC_INLINE;
   inline TFloatingBase(const Implantation& dDoubleSource) GCC_INLINE;
   inline TFloatingBase(const BuiltDouble& bdDouble) GCC_INLINE;
   inline TFloatingBase(const thisType& sSource) GCC_INLINE;
   inline thisType& operator=(const thisType& dsSource) GCC_INLINE;
      
   thisType& operator=(const BuiltDouble& bdDouble);

   inline operator BuiltDouble() const GCC_INLINE;
   inline Implantation& implantation() GCC_INLINE;  
   inline const Implantation& implantation() const GCC_INLINE;  
   inline thisType queryEpsilon() const GCC_INLINE;
   inline bool isZero() const GCC_INLINE; 
   
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   inline thisType& plusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& minusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& divAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;

   inline thisType& operator-=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator*=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator+=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator/=(const thisType& ttSource) GCC_INLINE;

   static const int UByteSizeImplantation = TypeTraits::UByteSizeImplantation;
   static const int UBitSizeMantissa = TypeTraits::UBitSizeMantissa;
   static const int UBitSizeExponent = TypeTraits::UBitSizeExponent;
   typedef typename BuiltDouble::Mantissa Mantissa;
   void fillMantissa(Mantissa& mMantissa) const;
   void setMantissa(const Mantissa& mMantissa);

   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline thisType& opposite() GCC_INLINE; 
 
   unsigned int queryBasicExponent() const;
   inline int queryExponent() const GCC_INLINE;
   void setBasicExponent(unsigned int uExponent);
   inline void setExponent(int uExponent) GCC_INLINE;
      
   inline bool isPositive() const GCC_INLINE;
   inline bool isNegative() const GCC_INLINE; 
   inline void setSign(bool fPositive) GCC_INLINE;
   inline void setPositive() GCC_INLINE; 
   inline void setNegative() GCC_INLINE; 
   inline bool isNormalised() const GCC_INLINE;
   inline bool isRanged() const GCC_INLINE;
   inline void clear() GCC_INLINE; 
   inline void swap(thisType& dSource) GCC_INLINE;
};

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_TFloatingBase
#include <unisim/util/simfloat/host_floating_gccopt.inl>
#undef DefineGCC_Inline_Double_TFloatingBase
#endif

template <class FloatingBaseTraits>
class TDoubleElement : public FloatingBaseTraits, public Details::DTDoubleElement::Access {
  private:
   typedef FloatingBaseTraits inherited;
   typedef TDoubleElement<FloatingBaseTraits> thisType;

   typedef typename FloatingBaseTraits::Mantissa Mantissa;

  public:
   typedef typename inherited::BuiltDouble BuiltDouble;
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   inline TDoubleElement() GCC_INLINE;
   inline TDoubleElement(int iValue, StatusAndControlFlags& scfFlags) GCC_INLINE;
      
   inline TDoubleElement(typename FloatingBaseTraits::Implantation fbtSource) GCC_INLINE;
   inline TDoubleElement(const BuiltDouble& bdDouble) GCC_INLINE;
   inline TDoubleElement(const thisType& dSource) GCC_INLINE;
   inline int queryInteger(StatusAndControlFlags& scfFlags) const GCC_INLINE;
   inline unsigned int queryUnsignedInteger(StatusAndControlFlags& scfFlags) const GCC_INLINE;

   bool isInternZero() const;
   inline bool isZero() const GCC_INLINE; 
   bool isNaN() const;
   bool isQNaN() const;
   bool isSNaN() const;
   bool isInfty() const;
   inline bool hasInftyExponent() const GCC_INLINE;

   enum ComparisonResult { CRNaN, CRLess, CREqual, CRGreater };
   inline ComparisonResult compare(const thisType& bdSource) const GCC_INLINE;
   inline bool operator==(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator<(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator>(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator>=(const thisType& bdSource) const GCC_INLINE;

   static const int UByteSizeImplantation = inherited::UByteSizeImplantation;
   static const int UBitSizeMantissa = inherited::UBitSizeMantissa;
   static const int UBitSizeExponent = inherited::UBitSizeExponent;

   typedef typename BuiltDouble::DiffDouble DiffDouble;
   inline DiffDouble queryNumberOfFloatsBetween(const thisType& deElement) const GCC_INLINE;
   inline thisType queryNthSuccessor(const DiffDouble& biIntSource) const GCC_INLINE;
   inline thisType queryNthPredecessor(const DiffDouble& biIntSource) const GCC_INLINE;
   bool isPuiss2() const;
   Errors queryErrors() const;

   inline thisType& opposite() GCC_INLINE; 
   inline thisType& plusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& minusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& divAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;

   inline thisType& operator-=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator*=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator+=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator/=(const thisType& ttSource) GCC_INLINE;

   inline thisType operator-(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator+(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator*(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator/(const thisType& ttSource) const GCC_INLINE;

   inline thisType& setOpposite() GCC_INLINE;
   inline thisType& setInftyExponent() GCC_INLINE;
   void write(std::ostream& osOut, const WriteParameters& wpParams) const;
   void read(std::istream& isIn, StatusAndControlFlags& scfFlags);
};

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_TDoubleElement
#include <unisim/util/simfloat/host_floating_gccopt.inl>
#undef DefineGCC_Inline_Double_TDoubleElement
#endif

}} // end of namespace Numerics::Double

} // end of namespace simfloat
} // end of namespace util
} // end of namespace unisim

#endif // Numerics_Double_FloatingH

