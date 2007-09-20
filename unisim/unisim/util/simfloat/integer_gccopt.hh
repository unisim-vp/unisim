/***************************************************************************
                Integer_gccopt.h  -  Template for various types of integer
                             -------------------
    first release        : 15 Jul 2005
    copyright            : (C) 2004-2005 CEA
    authors              : Franck V�drine, Bruno Marre, Benjamin Blanc, Gilles Mouchard
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

#ifndef Numerics_Integer_IntegerH
#define Numerics_Integer_IntegerH

#ifdef WIN32
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#define ENDIAN LITTLE_ENDIAN
#else
#include <endian.h>
#endif

#include <iostream>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#if !defined(__GNUC__) || !defined(__SUN__)
#include <string.h>
#include <ctype.h>
#endif

#ifdef GCC_VERSION
#undef GCC_VERSION
#endif

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (GCC_VERSION >= 30000) && defined(EXTENSIVE_INLINING)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

namespace unisim {
namespace util {
namespace simfloat {

namespace Numerics { namespace Integer {

namespace Details {

class Access {
  public:
   enum ComparisonResult { CRLess=0, CREqual=1, CRGreater=2 };
   static int log_base_2(unsigned int uValue)
      {  register int uResult = 1;
         while ((uValue >>= 1) != 0)
            ++uResult;
         return uResult;
      }
};

class CellIntegerTraitsContract {
  public:
   inline CellIntegerTraitsContract() GCC_INLINE;
   CellIntegerTraitsContract(const CellIntegerTraitsContract& biSource) { assert(false); }
   CellIntegerTraitsContract& operator=(const CellIntegerTraitsContract& itSource) { assert(false); return *this; }
   typedef unsigned int& ArrayProperty;
   ArrayProperty array(int uIndex) { assert(false); return *((unsigned int*) NULL); }
   unsigned int array(int uIndex) const { assert(false); return 0; }
   unsigned int carray(int uIndex) const { assert(false); return 0; }
   ArrayProperty operator[](int uIndex) { assert(false); return *((unsigned int*) NULL); }
   unsigned int operator[](int uIndex) const { assert(false); return 0; }
   typedef CellIntegerTraitsContract MultResult;

   typedef CellIntegerTraitsContract QuotientResult;
   typedef CellIntegerTraitsContract RemainderResult;
   typedef CellIntegerTraitsContract NormalizedRemainderResult;
   void copyLow(const MultResult& mrResult) { assert(false); }
   int querySize() const { assert(false); return 0; }
   void adjustSize(int uNewSize) { assert(false); }
   void setSize(int uExactSize) { assert(false); }
   void normalize() { assert(false); }
   void assertSize(int uNewSize) { assert(false); }
   void clear() { assert(false); }
   typedef CellIntegerTraitsContract ExtendedInteger;
};

inline CellIntegerTraitsContract::CellIntegerTraitsContract() {}

template <int uSize>
class TBasicCellIntegerTraits : public CellIntegerTraitsContract {
  private:
   unsigned int auArray[uSize];
   typedef TBasicCellIntegerTraits<uSize> thisType;

  protected:
   inline unsigned int* _array() GCC_INLINE; 
   inline const unsigned int* _array() const GCC_INLINE;

  public:
   inline TBasicCellIntegerTraits() GCC_INLINE; 
   inline TBasicCellIntegerTraits(const thisType& biSource) GCC_INLINE;
      
   inline thisType& operator=(const thisType& itSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uSource) GCC_INLINE;
      
   typedef unsigned int& ArrayProperty;
   inline ArrayProperty array(int uIndex) GCC_INLINE;
      
   inline unsigned int array(int uIndex) const GCC_INLINE;
      
   inline unsigned int carray(int uIndex) const GCC_INLINE; 
   inline ArrayProperty operator[](int uIndex) GCC_INLINE;
      
   inline unsigned int operator[](int uIndex) const GCC_INLINE;
      

   inline static int querySize() GCC_INLINE; 
   inline void normalize() GCC_INLINE; 
   inline void adjustSize(int uNewSize) GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   inline void setSize(int uExactSize) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE;
};

template <int uSize>
inline unsigned int*
TBasicCellIntegerTraits<uSize>::_array() { return auArray; }

template <int uSize>
inline const unsigned int*
TBasicCellIntegerTraits<uSize>::_array() const { return auArray; }

template <int uSize>
inline
TBasicCellIntegerTraits<uSize>::TBasicCellIntegerTraits()
{ memset(auArray, 0, uSize*sizeof(unsigned int)); }

template <int uSize>
inline
TBasicCellIntegerTraits<uSize>::TBasicCellIntegerTraits(const thisType& biSource)
{  memcpy(auArray, biSource.auArray, uSize*sizeof(unsigned int)); }

template <int uSize>
inline TBasicCellIntegerTraits<uSize>&
TBasicCellIntegerTraits<uSize>::operator=(const thisType& itSource)
{  memcpy(auArray, itSource.auArray, uSize*sizeof(unsigned int));
   return *this;
}

template <int uSize>
inline TBasicCellIntegerTraits<uSize>&
TBasicCellIntegerTraits<uSize>::operator=(unsigned int uSource)
{  if (uSize > 1)
      memset(auArray, 0, uSize*sizeof(unsigned int));
   auArray[0] = uSource;
   return *this;
}

template <int uSize>
inline typename TBasicCellIntegerTraits<uSize>::ArrayProperty
TBasicCellIntegerTraits<uSize>::array(int uIndex)
{  assert(uIndex >= 0 && (uIndex < uSize));
   return auArray[uIndex];
}

template <int uSize>
inline unsigned int
TBasicCellIntegerTraits<uSize>::array(int uIndex) const
{  assert(uIndex >= 0);
   return (uIndex < uSize) ? auArray[uIndex] : 0U;
}

template <int uSize>
inline unsigned int
TBasicCellIntegerTraits<uSize>::carray(int uIndex) const
{ return array(uIndex); }

template <int uSize>
inline typename TBasicCellIntegerTraits<uSize>::ArrayProperty
TBasicCellIntegerTraits<uSize>::operator[](int uIndex)
{  assert(uIndex >= 0 && (uIndex < uSize));
   return auArray[uIndex];
}

template <int uSize>
inline unsigned int
TBasicCellIntegerTraits<uSize>::operator[](int uIndex) const
{  assert(uIndex >= 0);
   return (uIndex < uSize) ? auArray[uIndex] : 0U;
}

template <int uSize>
inline int
TBasicCellIntegerTraits<uSize>::querySize()
{ return uSize; }

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::normalize()
{}

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::adjustSize(int uNewSize)
{ assert(false); }

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::assertSize(int uNewSize)
{ assert(uNewSize <= uSize); }

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::setSize(int uExactSize)
{ assert(uExactSize == uSize); }

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::clear()
{ memset(auArray, 0, uSize*sizeof(unsigned int)); }

template <int uSize>
inline void
TBasicCellIntegerTraits<uSize>::swap(thisType& biSource)
{  unsigned int auTemp[uSize];
   memcpy(auTemp, auArray, uSize*sizeof(unsigned int));
   memcpy(auArray, biSource.auArray, uSize*sizeof(unsigned int));
   memcpy(biSource.auArray, auTemp, uSize*sizeof(unsigned int));
}

template <int uSize>
class TCellIntegerTraits : public TBasicCellIntegerTraits<uSize> {
  private:
   typedef TBasicCellIntegerTraits<uSize> inherited;
   typedef TCellIntegerTraits<uSize> thisType;

  public:
   inline TCellIntegerTraits() GCC_INLINE;
   inline TCellIntegerTraits(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& itSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uSource) GCC_INLINE;

   class MultResult : public TBasicCellIntegerTraits<2*uSize> {
     private:
      typedef TBasicCellIntegerTraits<2*uSize> inherited;
     public:
      inline MultResult() GCC_INLINE; 
      inline MultResult(const MultResult& mrSource) GCC_INLINE;
      inline MultResult(const TBasicCellIntegerTraits<uSize>& itSource) GCC_INLINE;
         
      friend class TCellIntegerTraits<uSize>;
   };
   friend class MultResult;
   inline thisType& operator=(const MultResult& mrSource) GCC_INLINE;
      
   typedef TCellIntegerTraits<uSize> QuotientResult;
   typedef TCellIntegerTraits<uSize> RemainderResult;
   typedef TBasicCellIntegerTraits<uSize+1> NormalizedRemainderResult;
   inline void copyLow(const MultResult& mrResult) GCC_INLINE;
      
   typedef TBasicCellIntegerTraits<uSize+1> ExtendedInteger;
};

template <int uSize>
inline
TCellIntegerTraits<uSize>::TCellIntegerTraits()
   : inherited() {}

template <int uSize>
inline
TCellIntegerTraits<uSize>::TCellIntegerTraits(const thisType& biSource)
 : inherited(biSource) {}

template <int uSize>
inline TCellIntegerTraits<uSize>&
TCellIntegerTraits<uSize>::operator=(const thisType& itSource)
{  return (thisType&) inherited::operator=(itSource); }

template <int uSize>
inline TCellIntegerTraits<uSize>&
TCellIntegerTraits<uSize>::operator=(unsigned int uSource)
{  return (thisType&) inherited::operator=(uSource); }

template <int uSize>
inline
TCellIntegerTraits<uSize>::MultResult::MultResult()
{}

template <int uSize>
inline
TCellIntegerTraits<uSize>::MultResult::MultResult(const MultResult& mrSource)
 : inherited(mrSource) {}
 
template <int uSize>
inline
TCellIntegerTraits<uSize>::MultResult::MultResult(const TBasicCellIntegerTraits<uSize>& itSource)
{  memcpy(inherited::_array(), itSource._array(), uSize*sizeof(unsigned int)); }

template <int uSize>
inline TCellIntegerTraits<uSize>&
TCellIntegerTraits<uSize>::operator=(const MultResult& mrSource)
{  memcpy(inherited::_array(), mrSource._array(), uSize*sizeof(unsigned int)); return *this; }

template <int uSize>
inline void
TCellIntegerTraits<uSize>::copyLow(const MultResult& mrResult)
{  memcpy(inherited::_array(), mrResult._array(), uSize*sizeof(unsigned int)); }

/*************************************/
/* Definition - template TBigCellInt */
/*************************************/

template <class IntegerTraits>
class TBigCellInt : public Details::Access, protected IntegerTraits {
  public:
   class FormatParameters {
     private:
      enum Type { TIntern, TBinary, TFullBinary, TFullHexaDecimal, TIntegerCell, TDecimal };
      Type tType;
      int uLength;
      
     public:
      inline FormatParameters() GCC_INLINE;
      inline FormatParameters(const FormatParameters& pSource) GCC_INLINE;

      inline bool isBinary() const GCC_INLINE; 
      inline bool isFullBinary() const GCC_INLINE; 
      inline bool isLightBinary() const GCC_INLINE; 
      inline bool isFullHexaDecimal() const GCC_INLINE; 
      inline bool isIntern() const GCC_INLINE; 
      inline bool isIntegerCell() const GCC_INLINE; 
      inline bool isDecimal() const GCC_INLINE; 

      inline FormatParameters& setIntern() GCC_INLINE; 
      inline FormatParameters& setFullBinary(int uLengthSource) GCC_INLINE;
      inline FormatParameters& setFullHexaDecimal(int uLengthSource) GCC_INLINE;
      inline FormatParameters& setBinary() GCC_INLINE; 
      inline FormatParameters& setIntegerCell() GCC_INLINE; 
      inline FormatParameters& setDecimal() GCC_INLINE; 
      inline const int& queryLength() const GCC_INLINE;
   };

  private:
   typedef TBigCellInt<IntegerTraits> thisType;
   typedef IntegerTraits inherited;
   class HexaChars {
     public:
      inline HexaChars() GCC_INLINE; 
      inline bool acceptChar(char chChar) const GCC_INLINE;
      inline unsigned int queryValue(char cChar) const GCC_INLINE;
   };
   static HexaChars hcHexaChars;

   inline static unsigned int add(unsigned int& uCell, unsigned int uValue) GCC_INLINE;
   inline static unsigned int sub(unsigned int& uCell, unsigned int uValue) GCC_INLINE;
      
   bool verifyAtomicity() const;
   unsigned int retrieveSignificantMidDivide(int uLogBase2, bool& fExact) const;
   unsigned int retrieveSignificantMidDivideNormalized(bool& fExact) const;

#if !defined(__GNUC__) || (GCC_VERSION >= 29600)
   template <class TypeIntegerTraits> friend class TBigCellInt;
#endif
   void writeFullBinary(std::ostream& osOut, const FormatParameters& pParams) const;
   void writeFullHexaDecimal(std::ostream& osOut, const FormatParameters& pParams) const;
   void writeCells(std::ostream& osOut, const FormatParameters& pParams) const;
   void writeDecimal(std::ostream& osOut, const FormatParameters& pParams) const;

  protected:
   inline static int log_base_2(unsigned int uValue) GCC_INLINE;
      
   inline typename inherited::ArrayProperty array(int uIndex) GCC_INLINE; 
   inline unsigned int array(int uIndex) const GCC_INLINE; 
   inline unsigned int carray(int uIndex) const GCC_INLINE; 

  public:
   class MidArray {
     private:
      IntegerTraits* pitArray;
      int uIndex;

     public:
      inline MidArray(TBigCellInt<IntegerTraits>& biSource, int uIndexSource) GCC_INLINE;
      inline MidArray& operator=(unsigned int uValue) GCC_INLINE;
      inline operator unsigned int() const GCC_INLINE;
   };
   friend class MidArray;
   inline MidArray midArray(int uIndex) GCC_INLINE;
   inline unsigned int midArray(int uIndex) const GCC_INLINE;

   inline void setMidArray(int uIndex, unsigned int uValue) GCC_INLINE;

   inline unsigned int cmidArray(int uIndex) const GCC_INLINE; 

   class BitArray {
     private:
      IntegerTraits* pitArray;
      int uIndex;

     public:
      inline BitArray(TBigCellInt<IntegerTraits>& biSource, int uIndexSource) GCC_INLINE;
      inline BitArray(const BitArray& baSource) GCC_INLINE;
      inline BitArray& operator=(const BitArray& baSource) GCC_INLINE;
         
      inline BitArray& operator=(bool fValue) GCC_INLINE;
         
      inline operator bool() const GCC_INLINE;
         
   };
   friend class BitArray;
   inline BitArray bitArray(int uIndex) GCC_INLINE;
   inline bool bitArray(int uIndex) const GCC_INLINE;
   inline bool cbitArray(int uIndex) const GCC_INLINE; 
   inline void setBitArray(int uIndex, bool fValue) GCC_INLINE;
   inline void setTrueBitArray(int uIndex) GCC_INLINE;
   inline void setFalseBitArray(int uIndex) GCC_INLINE;

   inline TBigCellInt() GCC_INLINE;
   inline TBigCellInt(unsigned int uInt) GCC_INLINE;
   inline TBigCellInt(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uInt) GCC_INLINE;

   inline unsigned int operator[](int uIndex) const GCC_INLINE; 
   inline typename inherited::ArrayProperty operator[](int uIndex) GCC_INLINE; 
   ComparisonResult compare(const thisType& biSource) const;
   inline bool operator<(const thisType& biSource) const GCC_INLINE;
   inline bool operator>(const thisType& biSource) const GCC_INLINE;
   inline bool operator<=(const thisType& biSource) const GCC_INLINE;
   inline bool operator>=(const thisType& biSource) const GCC_INLINE;
   inline bool operator==(const thisType& biSource) const GCC_INLINE;
   inline bool operator!=(const thisType& biSource) const GCC_INLINE;
      
   thisType& operator<<=(int uShift);
   thisType& operator>>=(int uShift);
   thisType& operator|=(const thisType& biSource);
   thisType& operator^=(const thisType& biSource);
   thisType& operator&=(const thisType& biSource);
   thisType& neg();
   thisType& neg(int uShift);
   bool isZero() const;
   bool hasZero(int uShift) const;
   
   class Carry {
     private:
      unsigned int uCarry;

     public:
      inline Carry(unsigned int uCarrySource=0U) GCC_INLINE;
      inline Carry(const Carry& cSource) GCC_INLINE;
      inline Carry& operator=(const Carry& cSource) GCC_INLINE; 

      inline void setCarry(bool fCarrySource=true) GCC_INLINE; 
      inline bool hasCarry() const GCC_INLINE; 
      inline void setIntCarry(unsigned int uCarrySource) GCC_INLINE; 
      inline const unsigned int& carry() const GCC_INLINE; 
      inline unsigned int& carry() GCC_INLINE; 
   };
   Carry add(const thisType& biSource);
   Carry sub(const thisType& biSource);
   inline Carry plusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry minusAssign(const thisType& biSource) GCC_INLINE; 
   Carry inc();
   Carry dec();

   inline thisType& operator+=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator+(const thisType& biSource) const GCC_INLINE;
      
   inline thisType& operator-=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator-(const thisType& biSource) const GCC_INLINE;
      
   inline thisType& operator--() GCC_INLINE; 
   inline thisType& operator++() GCC_INLINE; 
   Carry multAssign(unsigned int uSource);
   typedef typename inherited::MultResult MultResult;
   void mult(const thisType& biSource, MultResult& mrResult) const;
   inline thisType& operator*=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator*=(unsigned int uSource) GCC_INLINE;

#ifndef __BORLANDC__
   class DivisionResult;
   class NormalizedDivisionResult;
   class AtomicDivisionResult;
   void div(const thisType& biSource, DivisionResult& drResult) const;
   void divNormalized(const thisType& biSource, NormalizedDivisionResult& drResult) const;
   AtomicDivisionResult divAssign(unsigned int uSource);
#endif
   typedef typename inherited::QuotientResult QuotientResult;
   typedef typename inherited::RemainderResult RemainderResult;
   typedef typename inherited::NormalizedRemainderResult NormalizedRemainderResult;
   inline int querySize() const GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   
   class DivisionResult {
     private:
      typename inherited::QuotientResult qrQuotient;
      typename inherited::RemainderResult rrRemainder;
      int uComma;

#ifndef __BORLANDC__
      friend void TBigCellInt<IntegerTraits>::div(const thisType& biSource, DivisionResult& drResult) const;
#endif

     public:
      inline DivisionResult() GCC_INLINE;
      inline DivisionResult(const DivisionResult& drSource) GCC_INLINE;
      inline DivisionResult& operator=(const DivisionResult& drSource) GCC_INLINE;

      inline const typename inherited::QuotientResult& quotient() const GCC_INLINE; 
      inline typename inherited::QuotientResult& quotient() GCC_INLINE; 
      inline const typename inherited::RemainderResult& remainder() const GCC_INLINE; 
      inline typename inherited::RemainderResult& remainder() GCC_INLINE; 
      inline const int& comma() const GCC_INLINE; 
      inline int& comma() GCC_INLINE; 
   };
   class NormalizedDivisionResult {
    private:
      typename inherited::QuotientResult qrQuotient;
      typename inherited::NormalizedRemainderResult rrRemainder;
      int uComma;

#ifndef __BORLANDC__
      friend void TBigCellInt<IntegerTraits>::divNormalized(const thisType& biSource, NormalizedDivisionResult& drResult) const;
#endif

     public:
      inline NormalizedDivisionResult() GCC_INLINE;
      inline NormalizedDivisionResult(const NormalizedDivisionResult& drSource) GCC_INLINE;
      inline NormalizedDivisionResult& operator=(const NormalizedDivisionResult& drSource) GCC_INLINE;

      inline const typename inherited::QuotientResult& quotient() const GCC_INLINE; 
      inline typename inherited::QuotientResult& quotient() GCC_INLINE; 
      inline const typename inherited::NormalizedRemainderResult& remainder() const GCC_INLINE; 
      inline typename inherited::NormalizedRemainderResult& remainder() GCC_INLINE; 
      inline const int& comma() const GCC_INLINE; 
      inline int& comma() GCC_INLINE;  
   };

   class AtomicDivisionResult {
     private:
      unsigned int uRemainder;
      
#ifndef __BORLANDC__
      friend AtomicDivisionResult TBigCellInt<IntegerTraits>::divAssign(unsigned int uSource);
#endif
     public:
      inline AtomicDivisionResult(unsigned int uRemainderSource=0U) GCC_INLINE;
      inline AtomicDivisionResult(const AtomicDivisionResult& adrSource) GCC_INLINE;
      inline AtomicDivisionResult& operator=(const AtomicDivisionResult& adrSource) GCC_INLINE;
         
      inline const unsigned int& modulo() const GCC_INLINE; 
      inline const unsigned int& remainder() const GCC_INLINE; 
      inline unsigned int& modulo() GCC_INLINE; 
      inline unsigned int& remainder() GCC_INLINE; 
   };

#ifdef __BORLANDC__
   void div(const thisType& biSource, DivisionResult& drResult) const;
   void divNormalized(const thisType& biSource, NormalizedDivisionResult& drResult) const;
   AtomicDivisionResult divAssign(unsigned int uSource);
#endif
   thisType& operator/=(const thisType& biSource);
   inline thisType& operator/=(unsigned int uSource) GCC_INLINE;
      
   typename inherited::QuotientResult operator/(const thisType& biSource) const;
   inline unsigned int operator%(unsigned int uSource) const GCC_INLINE;
      
   thisType& operator%=(const thisType& biSource);

   unsigned int log_base_2() const;
   void write(std::ostream& osOut, const FormatParameters& pParams) const;
   void read(std::istream& isIn, const FormatParameters& pParams);
   inline unsigned int queryValue() const GCC_INLINE;
      
   inline bool isAtomic() const GCC_INLINE; 
   inline const IntegerTraits& implantation() const GCC_INLINE; 
   inline IntegerTraits& implantation() GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
};

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::FormatParameters::FormatParameters()
   : tType(TIntern), uLength(0) {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::FormatParameters::FormatParameters(const FormatParameters& pSource)
   : tType(pSource.tType), uLength(pSource.uLength) {}

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isBinary() const
{ return (tType == TFullBinary) || (tType == TBinary); }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isFullBinary() const
{ return tType == TFullBinary; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isLightBinary() const
{ return tType == TBinary; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isFullHexaDecimal() const
{ return tType == TFullHexaDecimal; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isIntern() const
{ return tType == TIntern; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isIntegerCell() const
{ return tType == TIntegerCell; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::FormatParameters::isDecimal() const
{ return tType == TDecimal; }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setIntern()
{ tType = TIntern; return *this; }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setFullBinary(int uLengthSource)
{  tType = TFullBinary;
   uLength = uLengthSource;
   return *this;
}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setFullHexaDecimal(int uLengthSource)
{  tType = TFullHexaDecimal;
   uLength = uLengthSource;
   return *this;
}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setBinary()
{ tType = TBinary; return *this; }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setIntegerCell()
{ tType = TIntegerCell; return *this; }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::FormatParameters&
TBigCellInt<IntegerTraits>::FormatParameters::setDecimal()
{ tType = TDecimal; return *this; }

template <class IntegerTraits>
inline const int&
TBigCellInt<IntegerTraits>::FormatParameters::queryLength() const
{  assert((tType == TFullBinary) || (tType == TFullHexaDecimal));
   return uLength;
}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::HexaChars::HexaChars()
{}

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::HexaChars::acceptChar(char chChar) const
{  return ((chChar >= '0') && (chChar <= '9'))
       || ((chChar >= 'a') && (chChar <= 'f')) || ((chChar >= 'A') && (chChar <= 'F'));
}

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::HexaChars::queryValue(char cChar) const
{  return ((cChar >= '0') && (cChar <= '9')) ? (cChar-'0')
      : (((cChar >= 'a') && (cChar <= 'f')) ? (cChar-'a'+10)
      : (((cChar >= 'A') && (cChar <= 'F')) ? (cChar-'A'+10)
      : -1));
}

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::add(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
   uCell += uValue;
   return (uCell < uTemp) ? 1U : 0U;
}

template <class IntegerTraits>
inline int
TBigCellInt<IntegerTraits>::log_base_2(unsigned int uValue)
{  return Details::Access::log_base_2(uValue); }
   
template <class IntegerTraits>
inline typename IntegerTraits::ArrayProperty
TBigCellInt<IntegerTraits>::array(int uIndex)
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::array(int uIndex) const
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::carray(int uIndex) const
{ return array(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::sub(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
   uCell -= uValue;
   return (uCell > uTemp) ? 1U : 0U;
}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::MidArray::MidArray(TBigCellInt<IntegerTraits>& biSource, int uIndexSource)
   : pitArray(&biSource), uIndex(uIndexSource) {}   

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::MidArray&
TBigCellInt<IntegerTraits>::MidArray::operator=(unsigned int uValue)
{  register unsigned int uStore = pitArray->array(uIndex >> 1);
   pitArray->array(uIndex >> 1) = (uIndex & 1U)
      ? ((uValue << (4*sizeof(unsigned int)))
         | (uStore & ~(~0U << 4*sizeof(unsigned int))))
      : ((uStore & (~0U << 4*sizeof(unsigned int))) | uValue);
   return *this;
}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::MidArray::operator unsigned int() const
{  return (uIndex & 1U) ? (pitArray->array(uIndex >> 1) >> 4*sizeof(unsigned int))
      :  (pitArray->array(uIndex >> 1) & ~(~0U << 4*sizeof(unsigned int)));
}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::MidArray
TBigCellInt<IntegerTraits>::midArray(int uIndex)
{  return MidArray(*this, uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::midArray(int uIndex) const
{  return (uIndex & 1U) ? (array(uIndex >> 1) >> 4*sizeof(unsigned int))
      :  (array(uIndex >> 1) & ~(~0U << 4*sizeof(unsigned int)));
}
   
template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::cmidArray(int uIndex) const
   { return midArray(uIndex); }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::setMidArray(int uIndex, unsigned int uValue)
{  register unsigned int uStore = array(uIndex >> 1);
   array(uIndex >> 1) = (uIndex & 1U)
      ? ((uValue << (4*sizeof(unsigned int)))
         | (uStore & ~(~0U << 4*sizeof(unsigned int))))
      : ((uStore & (~0U << 4*sizeof(unsigned int))) | uValue);
}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::BitArray::BitArray(TBigCellInt<IntegerTraits>& biSource, int uIndexSource)
   : pitArray(&biSource), uIndex(uIndexSource) {}   

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::BitArray::BitArray(const BitArray& baSource)
   : pitArray(baSource.pitArray), uIndex(baSource.uIndex) {}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::BitArray&
TBigCellInt<IntegerTraits>::BitArray::operator=(const BitArray& baSource)
{  pitArray = baSource.pitArray;
   uIndex = baSource.uIndex;
   return *this;
}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::BitArray&
TBigCellInt<IntegerTraits>::BitArray::operator=(bool fValue)
{  if (fValue)
      pitArray->array(uIndex/(sizeof(unsigned int)*8)) 
         |= (1U << (uIndex%(sizeof(unsigned int)*8)));
   else
      pitArray->array(uIndex/(sizeof(unsigned int)*8))
         &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
   return *this;
}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::BitArray::operator bool() const
{  return (pitArray->array(uIndex/(sizeof(unsigned int)*8))
         & (1U << (uIndex%(sizeof(unsigned int)*8))))
      ? true : false;
}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::BitArray
TBigCellInt<IntegerTraits>::bitArray(int uIndex)
{  return BitArray(*this, uIndex); }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::bitArray(int uIndex) const
{  return (array(uIndex/(sizeof(unsigned int)*8)) & (1U << (uIndex%(sizeof(unsigned int)*8))))
      ? true : false;
}

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::setBitArray(int uIndex, bool fValue)
{  if (fValue)
      array(uIndex/(sizeof(unsigned int)*8)) |= (1U << (uIndex%(sizeof(unsigned int)*8)));
   else
      array(uIndex/(sizeof(unsigned int)*8)) &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
}

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::setTrueBitArray(int uIndex)
{  array(uIndex/(sizeof(unsigned int)*8)) |= (1U << (uIndex%(sizeof(unsigned int)*8))); }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::setFalseBitArray(int uIndex)
{  array(uIndex/(sizeof(unsigned int)*8)) &= ~(1U << (uIndex%(sizeof(unsigned int)*8))); }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::cbitArray(int uIndex) const
{ return bitArray(uIndex); }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::TBigCellInt() : inherited() {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::TBigCellInt(unsigned int uInt) : inherited() { array(0) = uInt; }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::TBigCellInt(const thisType& biSource) : inherited(biSource) {}

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator=(const thisType& biSource)
{  return (thisType&) inherited::operator=(biSource); }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator=(unsigned int uInt)
{  inherited::clear();
   array(0) = uInt;
   return *this;
}

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::operator[](int uIndex) const
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline typename IntegerTraits::ArrayProperty
TBigCellInt<IntegerTraits>::operator[](int uIndex)
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator<(const thisType& biSource) const
{  return compare(biSource) == CRLess; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator>(const thisType& biSource) const
{  return compare(biSource) == CRGreater; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator<=(const thisType& biSource) const
{  return compare(biSource) <= CREqual; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator>=(const thisType& biSource) const
{  return compare(biSource) >= CREqual; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator==(const thisType& biSource) const
{  return compare(biSource) == CREqual; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::operator!=(const thisType& biSource) const
{  return compare(biSource) != CREqual; }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::Carry::Carry(unsigned int uCarrySource) : uCarry(uCarrySource) {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::Carry::Carry(const Carry& cSource) : uCarry(cSource.uCarry) {}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::Carry&
TBigCellInt<IntegerTraits>::Carry::operator=(const Carry& cSource)
{ uCarry = cSource.uCarry; return *this; }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::Carry::setCarry(bool fCarrySource)
{ uCarry = fCarrySource ? 1U : 0U; }

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::Carry::hasCarry() const
{ return uCarry != 0U; }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::Carry::setIntCarry(unsigned int uCarrySource)
{ uCarry = uCarrySource; }

template <class IntegerTraits>
inline const unsigned int&
TBigCellInt<IntegerTraits>::Carry::carry() const
{ return uCarry; }

template <class IntegerTraits>
inline unsigned int&
TBigCellInt<IntegerTraits>::Carry::carry()
{ return uCarry; }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::Carry
TBigCellInt<IntegerTraits>::plusAssign(const thisType& biSource)
{ return add(biSource); }

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::Carry
TBigCellInt<IntegerTraits>::minusAssign(const thisType& biSource)
{ return sub(biSource); }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator+=(const thisType& biSource)
{ add(biSource); return *this; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>
TBigCellInt<IntegerTraits>::operator+(const thisType& biSource) const
{  thisType biResult = *this; biResult += biSource; return biResult; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator-=(const thisType& biSource)
{ sub(biSource); return *this; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>
TBigCellInt<IntegerTraits>::operator-(const thisType& biSource) const
{  thisType biResult = *this; biResult -= biSource; return biResult; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator--()
{ dec(); return *this; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator++()
{ inc(); return *this; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator*=(const thisType& biSource)
{  MultResult mrResult;
   mult(biSource, mrResult);
   inherited::copyLow(mrResult);
   return *this;
}

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator*=(unsigned int uSource)
{  Carry crCarry = multAssign(uSource); assert(!crCarry.hasCarry()); return *this; }

template <class IntegerTraits>
inline int
TBigCellInt<IntegerTraits>::querySize() const
{ return inherited::querySize(); }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::assertSize(int uNewSize)
{ inherited::assertSize(uNewSize); }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::DivisionResult::DivisionResult() : qrQuotient(), rrRemainder(), uComma(0) {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::DivisionResult::DivisionResult(const DivisionResult& drSource)
   : qrQuotient(drSource.qrQuotient), rrRemainder(drSource.rrRemainder), uComma(drSource.uComma) {}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::DivisionResult&
TBigCellInt<IntegerTraits>::DivisionResult::operator=(const DivisionResult& drSource)
{  qrQuotient = drSource.qrQuotient;
   rrRemainder = drSource.rrRemainder;
   uComma = drSource.uComma;
   return *this;
}

template <class IntegerTraits>
inline const typename IntegerTraits::QuotientResult&
TBigCellInt<IntegerTraits>::DivisionResult::quotient() const
{ return qrQuotient; }

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult&
TBigCellInt<IntegerTraits>::DivisionResult::quotient()
{ return qrQuotient; }

template <class IntegerTraits>
inline const typename IntegerTraits::RemainderResult&
TBigCellInt<IntegerTraits>::DivisionResult::remainder() const
{ return rrRemainder; }

template <class IntegerTraits>
inline typename IntegerTraits::RemainderResult&
TBigCellInt<IntegerTraits>::DivisionResult::remainder()
{ return rrRemainder; }

template <class IntegerTraits>
inline const int&
TBigCellInt<IntegerTraits>::DivisionResult::comma() const
{ return uComma; }

template <class IntegerTraits>
inline int&
TBigCellInt<IntegerTraits>::DivisionResult::comma()
{ return uComma; }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::NormalizedDivisionResult()
 : qrQuotient(), rrRemainder(), uComma(0) {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::NormalizedDivisionResult(const NormalizedDivisionResult& drSource)
   : qrQuotient(drSource.qrQuotient), rrRemainder(drSource.rrRemainder), uComma(drSource.uComma) {}

template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::NormalizedDivisionResult&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::operator=(const NormalizedDivisionResult& drSource)
{  qrQuotient = drSource.qrQuotient;
   rrRemainder = drSource.rrRemainder;
   uComma = drSource.uComma;
   return *this;
}

template <class IntegerTraits>
inline const typename IntegerTraits::QuotientResult&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::quotient() const
{ return qrQuotient; }

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::quotient()
{ return qrQuotient; }

template <class IntegerTraits>
inline const typename IntegerTraits::NormalizedRemainderResult&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::remainder() const
{ return rrRemainder; }

template <class IntegerTraits>
inline typename IntegerTraits::NormalizedRemainderResult&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::remainder()
{ return rrRemainder; }

template <class IntegerTraits>
inline const int&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::comma() const
{ return uComma; }

template <class IntegerTraits>
inline int&
TBigCellInt<IntegerTraits>::NormalizedDivisionResult::comma() 
{ return uComma; }

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::AtomicDivisionResult::AtomicDivisionResult(unsigned int uRemainderSource)
 : uRemainder(uRemainderSource) {}

template <class IntegerTraits>
inline
TBigCellInt<IntegerTraits>::AtomicDivisionResult::AtomicDivisionResult(const AtomicDivisionResult& adrSource)
 : uRemainder(adrSource.uRemainder) {}
 
template <class IntegerTraits>
inline typename TBigCellInt<IntegerTraits>::AtomicDivisionResult&
TBigCellInt<IntegerTraits>::AtomicDivisionResult::operator=(const AtomicDivisionResult& adrSource)
{  uRemainder = adrSource.uRemainder;
   return *this;
}
   
template <class IntegerTraits>
inline const unsigned int&
TBigCellInt<IntegerTraits>::AtomicDivisionResult::modulo() const
{ return uRemainder; }

template <class IntegerTraits>
inline const unsigned int&
TBigCellInt<IntegerTraits>::AtomicDivisionResult::remainder() const
{ return uRemainder; }

template <class IntegerTraits>
inline unsigned int&
TBigCellInt<IntegerTraits>::AtomicDivisionResult::modulo()
{ return uRemainder; }

template <class IntegerTraits>
inline unsigned int&
TBigCellInt<IntegerTraits>::AtomicDivisionResult::remainder()
{ return uRemainder; }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator/=(unsigned int uSource)
{  divAssign(uSource);
   return *this;
}

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::operator%(unsigned int uSource) const
{  thisType bciCopy(*this);
   return bciCopy.divAssign(uSource).remainder();
}

template <class IntegerTraits>
inline unsigned int
TBigCellInt<IntegerTraits>::queryValue() const
{  assert(verifyAtomicity());
   return array(0);
}

template <class IntegerTraits>
inline bool
TBigCellInt<IntegerTraits>::isAtomic() const
{ return verifyAtomicity(); }

template <class IntegerTraits>
inline const IntegerTraits&
TBigCellInt<IntegerTraits>::implantation() const
{ return (inherited&) *this; }

template <class IntegerTraits>
inline IntegerTraits&
TBigCellInt<IntegerTraits>::implantation()
{ return (inherited&) *this; }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::swap(thisType& biSource)
{ inherited::swap(biSource); }

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::clear()
{ inherited::clear(); }

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator/=(const thisType& biSource) {
   DivisionResult drResult;
   div(biSource, drResult);
   if (drResult.comma() > 0) {
      inherited::operator=(drResult.quotient());
      operator>>=(inherited::querySize()*sizeof(unsigned int)*8-drResult.comma());
      setTrueBitArray(drResult.comma());
   }
   else if (drResult.comma() == 0)
      *this = 1U;
   else
      *this = 0U;
   return *this;
}

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult
TBigCellInt<IntegerTraits>::operator/(const thisType& biSource) const {
   DivisionResult drResult;
   div(biSource, drResult);
   if (drResult.comma() > 0) {
      TBigCellInt<typename inherited::QuotientResult>& pqrPromotedResult = (TBigCellInt<typename inherited::QuotientResult>&) drResult.quotient();
      int uShift = drResult.quotient().querySize()*sizeof(unsigned int)*8-drResult.comma();
      pqrPromotedResult >>= uShift;
      pqrPromotedResult.setTrueBitArray(drResult.comma());
   }
   else {
      drResult.quotient().clear();
      drResult.quotient()[0] = (drResult.comma() == 0) ? 1U : 0U;
   };
   return drResult.quotient();
}


template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>&
TBigCellInt<IntegerTraits>::operator%=(const thisType& biSource) {
   DivisionResult drResult;
   div(biSource, drResult);
   if (drResult.comma() > 0) {
      TBigCellInt<typename inherited::QuotientResult>& pqrPromotedResult = (TBigCellInt<typename inherited::QuotientResult>&) drResult.quotient();
      pqrPromotedResult >>= (inherited::querySize()*sizeof(unsigned int)*8-drResult.comma());
      drResult.quotient()[drResult.comma()/sizeof(unsigned int)*8]
            |= (1U << (drResult.comma() % sizeof(unsigned int)*8));
      pqrPromotedResult *= biSource;
      *this -= pqrPromotedResult;
   }
   else if (drResult.comma() == 0)
      *this -= biSource;

   return *this;
}

template <class IntegerTraits>
inline std::ostream&
operator<<(std::ostream& osOut, const TBigCellInt<IntegerTraits>& bciInt) {
   bciInt.write(osOut, TBigCellInt<IntegerTraits>::FormatParameters());
   return osOut;
}

template <class IntegerTraits>
inline std::istream&
operator<<(std::istream& isIn, TBigCellInt<IntegerTraits>& bciInt) {
   bciInt.read(isIn, TBigCellInt<IntegerTraits>::FormatParameters());
   return isIn;
}

template <class IntegerTraits>
inline void
TBigCellInt<IntegerTraits>::write(std::ostream& osOut, const FormatParameters& pParams) const {
   if (pParams.isFullBinary())
      writeFullBinary(osOut, pParams);
   else if (pParams.isFullHexaDecimal())
      writeFullHexaDecimal(osOut, pParams);
   else if (!pParams.isDecimal())
      writeCells(osOut, pParams);
   else // pParams.isDecimal()
      writeDecimal(osOut, pParams);
}

} // end of namespace Details

template <class IntegerTraits>
class TBigCellInt : public Details::TBigCellInt<IntegerTraits> {
  private:
   typedef TBigCellInt<IntegerTraits> thisType;
   typedef Details::TBigCellInt<IntegerTraits> inherited;

  public:
   inline TBigCellInt() GCC_INLINE;
   inline TBigCellInt(unsigned int uInt) GCC_INLINE;
   inline TBigCellInt(const thisType& biSource) GCC_INLINE;
};

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>::TBigCellInt() : inherited() {}

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>::TBigCellInt(unsigned int uInt) : inherited(uInt) {}

template <class IntegerTraits>
inline TBigCellInt<IntegerTraits>::TBigCellInt(const thisType& biSource) : inherited(biSource) {}

/* Optimization for the exponent of floating point numbers */

typedef Details::TBigCellInt<Details::TCellIntegerTraits<1> > AloneBigCellInt;

template <>
class TBigCellInt<Details::TCellIntegerTraits<1> > : public AloneBigCellInt {
  private:
   typedef TBigCellInt<Details::TCellIntegerTraits<1> > thisType;
   typedef AloneBigCellInt inherited;

   inline unsigned int& value() GCC_INLINE; 
   inline const unsigned int& value() const GCC_INLINE; 
   inline static unsigned int add(unsigned int& uCell, unsigned int uValue) GCC_INLINE;
   inline static unsigned int sub(unsigned int& uCell, unsigned int uValue) GCC_INLINE;

  protected:
   inline static int log_base_2(unsigned int uValue) GCC_INLINE;
      
   inline unsigned int& array(int uIndex) GCC_INLINE; 
   inline unsigned int array(int uIndex) const GCC_INLINE; 
   inline unsigned int carray(int uIndex) const GCC_INLINE; 

  public:
   class MidArray {
     private:
      unsigned int& uArrayValue;
      int uIndex;

     public:
      inline MidArray(thisType& biSource, int uIndexSource) GCC_INLINE;
      inline MidArray& operator=(unsigned int uValue) GCC_INLINE;
         
      inline operator unsigned int() const GCC_INLINE;
         
   };
   friend class MidArray;
   inline MidArray midArray(int uIndex) GCC_INLINE;
   inline unsigned int midArray(int uIndex) const GCC_INLINE;
   inline unsigned int cmidArray(int uIndex) const GCC_INLINE; 
   inline void setMidArray(int uIndex, unsigned int uValue) GCC_INLINE;

   class BitArray {
     private:
      unsigned int& uArrayValue;
      int uIndex;

     public:
      inline BitArray(thisType& biSource, int uIndexSource) GCC_INLINE;
      inline BitArray(const BitArray& baSource) GCC_INLINE;
      inline BitArray& operator=(const BitArray& baSource) GCC_INLINE;
      inline BitArray& operator=(bool fValue) GCC_INLINE;
      inline operator bool() const GCC_INLINE;
   };
   friend class BitArray;
   inline BitArray bitArray(int uIndex) GCC_INLINE;
   inline bool bitArray(int uIndex) const GCC_INLINE;
   inline bool cbitArray(int uIndex) const GCC_INLINE; 
   inline void setBitArray(int uIndex, bool fValue) GCC_INLINE;
   inline void setTrueBitArray(int uIndex) GCC_INLINE; 
   inline void setFalseBitArray(int uIndex) GCC_INLINE; 

   inline TBigCellInt() GCC_INLINE;
   inline TBigCellInt(unsigned int uInt) GCC_INLINE;
   inline TBigCellInt(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& biSource) GCC_INLINE;
      
   inline thisType& operator=(unsigned int uInt) GCC_INLINE;
      

   inline unsigned int operator[](int uIndex) const GCC_INLINE; 
   inline unsigned int& operator[](int uIndex) GCC_INLINE; 
   inline ComparisonResult compare(const thisType& biSource) const GCC_INLINE;
      
   inline bool operator<(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>(const thisType& biSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator==(const thisType& biSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& biSource) const GCC_INLINE; 
   inline thisType& operator<<=(int uShift) GCC_INLINE; 
   inline thisType& operator>>=(int uShift) GCC_INLINE; 
   inline thisType& operator|=(const thisType& biSource) GCC_INLINE; 
   inline thisType& operator^=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator&=(const thisType& biSource) GCC_INLINE; 
   inline thisType& neg() GCC_INLINE; 
   inline thisType& neg(int uShift) GCC_INLINE;
   inline bool isZero() const GCC_INLINE; 
   inline bool hasZero(int uShift) const GCC_INLINE;

   inline Carry add(const thisType& biSource) GCC_INLINE;
   inline Carry sub(const thisType& biSource) GCC_INLINE;
   inline Carry plusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry minusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry inc() GCC_INLINE; 
   inline Carry dec() GCC_INLINE; 

   inline thisType& operator+=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator+(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator-=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator-(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator--() GCC_INLINE; 
   inline thisType& operator++() GCC_INLINE; 

   inline int querySize() const GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   inline thisType& operator/=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator/=(unsigned int uSource) GCC_INLINE;
   inline unsigned int operator%(unsigned int uSource) const GCC_INLINE;
   inline thisType& operator%=(const thisType& biSource) GCC_INLINE;

   inline unsigned int log_base_2() const GCC_INLINE; 
   inline unsigned int queryValue() const GCC_INLINE; 
   inline bool isAtomic() const GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
};

inline unsigned int&
TBigCellInt<Details::TCellIntegerTraits<1> >::value()
{ return _array()[0]; }

inline const unsigned int&
TBigCellInt<Details::TCellIntegerTraits<1> >::value() const
{ return _array()[0]; }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::add(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
   uCell += uValue;
   return (uCell < uTemp) ? 1U : 0U;
}

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::sub(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
   uCell -= uValue;
   return (uCell > uTemp) ? 1U : 0U;
}

inline int
TBigCellInt<Details::TCellIntegerTraits<1> >::log_base_2(unsigned int uValue)
{  return Details::Access::log_base_2(uValue); }

inline unsigned int&
TBigCellInt<Details::TCellIntegerTraits<1> >::array(int uIndex)
{ return value(); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::array(int uIndex) const
{ return value(); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::carray(int uIndex) const
{ return value(); }

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::MidArray::MidArray(thisType& biSource, int uIndexSource)
   : uArrayValue(biSource.value()), uIndex(uIndexSource) {}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::MidArray&
TBigCellInt<Details::TCellIntegerTraits<1> >::MidArray::operator=(unsigned int uValue)
{  uArrayValue = (uIndex > 0)
      ? ((uValue << (4*sizeof(unsigned int)))
         | (uArrayValue & ~(~0U << 4*sizeof(unsigned int))))
      : ((uArrayValue & (~0U << 4*sizeof(unsigned int))) | uValue);
   return *this;
}

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::MidArray::operator unsigned int() const
{  return (uIndex > 0) ? (uArrayValue >> 4*sizeof(unsigned int))
      :  (uArrayValue & ~(~0U << 4*sizeof(unsigned int)));
}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::MidArray
TBigCellInt<Details::TCellIntegerTraits<1> >::midArray(int uIndex)
{  return MidArray(*this, uIndex); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::midArray(int uIndex) const
{  return (uIndex > 0) ? (value() >> 4*sizeof(unsigned int))
      :  (value() & ~(~0U << 4*sizeof(unsigned int)));
}

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::cmidArray(int uIndex) const
{ return midArray(uIndex); }

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::setMidArray(int uIndex, unsigned int uValue)
{  value() = (uIndex > 0)
      ? ((uValue << (4*sizeof(unsigned int)))
         | (value() & ~(~0U << 4*sizeof(unsigned int))))
      : ((value() & (~0U << 4*sizeof(unsigned int))) | uValue);
}

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray::BitArray(thisType& biSource, int uIndexSource)
   : uArrayValue(biSource.value()), uIndex(uIndexSource) {}

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray::BitArray(const BitArray& baSource)
   : uArrayValue(baSource.uArrayValue), uIndex(baSource.uIndex) {}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray&
TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray::operator=(const BitArray& baSource)
{  uArrayValue = baSource.uArrayValue;
   uIndex = baSource.uIndex;
   return *this;
}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray&
TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray::operator=(bool fValue)
{  if (fValue)
      uArrayValue |= (1U << uIndex);
   else
      uArrayValue &= ~(1U << uIndex);
   return *this;
}

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray::operator bool() const
{  return (uArrayValue & (1U << uIndex)) ? true : false; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::BitArray
TBigCellInt<Details::TCellIntegerTraits<1> >::bitArray(int uIndex)
{  return BitArray(*this, uIndex); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::bitArray(int uIndex) const
{  return (value() & (1U << uIndex)) ? true : false; }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::cbitArray(int uIndex) const
{ return bitArray(uIndex); }

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::setBitArray(int uIndex, bool fValue)
{  if (fValue)
      value() |= (1U << uIndex);
   else
      value() &= ~(1U << uIndex);
}

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::setTrueBitArray(int uIndex)
{ value() |= (1U << uIndex); }

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::setFalseBitArray(int uIndex)
{ value() &= ~(1U << uIndex); }

inline
TBigCellInt<Details::TCellIntegerTraits<1> >::TBigCellInt() : inherited() {}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::TBigCellInt(unsigned int uInt) : inherited(uInt) {}

inline TBigCellInt<Details::TCellIntegerTraits<1> >::TBigCellInt(const thisType& biSource)
: inherited(biSource) {}

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator=(const thisType& biSource)
{  return (thisType&) inherited::operator=(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator=(unsigned int uInt)
{  value() = uInt; return *this; }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::operator[](int uIndex) const
{ return value(); }

inline unsigned int&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator[](int uIndex)
{ return value(); }

inline Details::Access::ComparisonResult
TBigCellInt<Details::TCellIntegerTraits<1> >::compare(const thisType& biSource) const
{  return (value() < biSource.value()) ? CRLess
      : ((value() > biSource.value()) ? CRGreater : CREqual);
}

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator<(const thisType& biSource) const
{ return value() < biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator>(const thisType& biSource) const
{ return value() > biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator<=(const thisType& biSource) const
{ return value() <= biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator>=(const thisType& biSource) const
{ return value() >= biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator==(const thisType& biSource) const
{ return value() == biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::operator!=(const thisType& biSource) const
{ return value() != biSource.value(); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator<<=(int uShift)
{ value() <<= uShift; return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator>>=(int uShift)
{ value() >>= uShift; return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator|=(const thisType& biSource)
{ value() |= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator^=(const thisType& biSource)
{ value() ^= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator&=(const thisType& biSource)
{ value() &= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::neg()
{ value() = ~value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::neg(int uShift)
{  if (uShift > 0)
      value() = (~value() & ~(~0U << uShift));
   return *this;
}

inline bool TBigCellInt<Details::TCellIntegerTraits<1> >::isZero() const
{ return value() == 0U; }

inline bool TBigCellInt<Details::TCellIntegerTraits<1> >::hasZero(int uShift) const
{  assert(uShift <= (int)(8*sizeof(unsigned int)));
         return (uShift <= 0)
            || ((value() << (8*sizeof(unsigned int)-uShift)) == 0U);
      }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::add(const thisType& biSource)
{  value() += biSource.value();
         return Carry((value() < biSource.value()) ? 1U : 0U);
      }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::sub(const thisType& biSource)
{  register Carry cCarry((value() < biSource.value()) ? 1U : 0U);
         value() -= biSource.value();
         return cCarry;
      }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::plusAssign(const thisType& biSource)
{ return add(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::minusAssign(const thisType& biSource)
{ return sub(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::inc()
{ return Carry(++value() == 0U ? 1U : 0U); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >::Carry
TBigCellInt<Details::TCellIntegerTraits<1> >::dec()
{ return Carry(value()-- == 0U ? 1U : 0U); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator+=(const thisType& biSource)
{ add(biSource); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >
TBigCellInt<Details::TCellIntegerTraits<1> >::operator+(const thisType& biSource) const
{  thisType biResult = *this; biResult += biSource; return biResult; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator-=(const thisType& biSource)
{ sub(biSource); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >
TBigCellInt<Details::TCellIntegerTraits<1> >::operator-(const thisType& biSource) const
{  thisType biResult = *this; biResult -= biSource; return biResult; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator--()
{ dec(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator++()
{ inc(); return *this; }

inline int TBigCellInt<Details::TCellIntegerTraits<1> >::querySize() const
{ return 1; }

inline void TBigCellInt<Details::TCellIntegerTraits<1> >::assertSize(int uNewSize)
{ assert(uNewSize <= 1); }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator/=(const thisType& biSource)
{  assert(biSource.value() != 0U);
   value() /= biSource.value();
   return *this;
}

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator/=(unsigned int uSource)
{  assert(uSource != 0U);
   value() /= uSource;
   return *this;
}

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::operator%(unsigned int uSource) const
{  return value() % uSource; }

inline TBigCellInt<Details::TCellIntegerTraits<1> >&
TBigCellInt<Details::TCellIntegerTraits<1> >::operator%=(const thisType& biSource)
{  value() %= biSource.value(); return *this; }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::log_base_2() const
{ return log_base_2(value()); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<1> >::queryValue() const
{ return value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<1> >::isAtomic() const
{ return true; }

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::swap(thisType& biSource)
{ inherited::swap(biSource); }

inline void
TBigCellInt<Details::TCellIntegerTraits<1> >::clear()
{ value() = 0U; }

#ifdef DefineUseLongLongIntForDoubleInt
namespace Details {

template <>
class TBasicCellIntegerTraits<2> : public CellIntegerTraitsContract {
  private:
   unsigned long long int ulValue;
   typedef TBasicCellIntegerTraits<2> thisType;

  protected:
   inline unsigned int* _array() GCC_INLINE; 
   inline const unsigned int* _array() const GCC_INLINE; 
   inline unsigned long long int& value() GCC_INLINE; 
   inline const unsigned long long int& value() const GCC_INLINE; 

  public:
   inline TBasicCellIntegerTraits() GCC_INLINE;
   inline TBasicCellIntegerTraits(unsigned int uValue) GCC_INLINE;
   inline TBasicCellIntegerTraits(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& itSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uSource) GCC_INLINE;
      
   typedef unsigned int& ArrayProperty;
   inline ArrayProperty array(int uIndex) GCC_INLINE;
   inline unsigned int array(int uIndex) const GCC_INLINE;
   inline unsigned int carray(int uIndex) const GCC_INLINE; 
   inline ArrayProperty operator[](int uIndex) GCC_INLINE;
   inline unsigned int operator[](int uIndex) const GCC_INLINE;

   inline static int querySize() GCC_INLINE; 
   inline void normalize() GCC_INLINE; 
   inline void adjustSize(int uNewSize) GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   inline void setSize(int uExactSize) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE;
};

inline unsigned int*
TBasicCellIntegerTraits<2>::_array()
{ return (unsigned int*) &ulValue; }

inline const unsigned int*
TBasicCellIntegerTraits<2>::_array() const
{ return (const unsigned int*) &ulValue; }

inline unsigned long long int&
TBasicCellIntegerTraits<2>::value()
{ return ulValue; }

inline const unsigned long long int&
TBasicCellIntegerTraits<2>::value() const
{ return ulValue; }

inline
TBasicCellIntegerTraits<2>::TBasicCellIntegerTraits() : ulValue(0) {}

inline
TBasicCellIntegerTraits<2>::TBasicCellIntegerTraits(unsigned int uValue) : ulValue(uValue) {}

inline
TBasicCellIntegerTraits<2>::TBasicCellIntegerTraits(const thisType& biSource) : ulValue(biSource.ulValue) {}

inline TBasicCellIntegerTraits<2>&
TBasicCellIntegerTraits<2>::operator=(const thisType& itSource)
{  ulValue = itSource.ulValue;
   return *this;
}

inline TBasicCellIntegerTraits<2>&
TBasicCellIntegerTraits<2>::operator=(unsigned int uSource)
{  ulValue = uSource;
   return *this;
}

inline TBasicCellIntegerTraits<2>::ArrayProperty
TBasicCellIntegerTraits<2>::array(int uIndex)
{  assert(!(uIndex & ~1U));
   return _array()[uIndex];
}

inline unsigned int
TBasicCellIntegerTraits<2>::array(int uIndex) const
{  assert(uIndex >= 0);
   return (uIndex < 2) ? _array()[uIndex] : 0U;
}

inline unsigned int
TBasicCellIntegerTraits<2>::carray(int uIndex) const 
{ return array(uIndex); }

inline TBasicCellIntegerTraits<2>::ArrayProperty
TBasicCellIntegerTraits<2>::operator[](int uIndex)
{  assert(!(uIndex & ~1U));
   return _array()[uIndex];
}

inline unsigned int
TBasicCellIntegerTraits<2>::operator[](int uIndex) const
{  assert(uIndex >= 0);
   return (uIndex < 2) ? _array()[uIndex] : 0U;
}

inline int
TBasicCellIntegerTraits<2>::querySize()
{ return 2; }

inline void
TBasicCellIntegerTraits<2>::normalize()
{}

inline void
TBasicCellIntegerTraits<2>::adjustSize(int uNewSize)
{ assert(false); }

inline void
TBasicCellIntegerTraits<2>::assertSize(int uNewSize)
{ assert(uNewSize <= 2); }

inline void
TBasicCellIntegerTraits<2>::setSize(int uExactSize)
{ assert(uExactSize == 2); }

inline void
TBasicCellIntegerTraits<2>::clear()
{ ulValue = 0; }

inline void
TBasicCellIntegerTraits<2>::swap(thisType& biSource)
{  unsigned long long int ulTemp = ulValue;
   ulValue = biSource.ulValue;
   biSource.ulValue = ulTemp;
}

} // end of namespace Details

typedef Details::TBigCellInt<Details::TCellIntegerTraits<2> > DoubleBigCellInt;

template <>
class TBigCellInt<Details::TCellIntegerTraits<2> > : public DoubleBigCellInt {
  private:
   typedef TBigCellInt<Details::TCellIntegerTraits<2> > thisType;
   typedef DoubleBigCellInt inherited;

   inline unsigned long long int& value() GCC_INLINE;
   inline const unsigned long long int& value() const GCC_INLINE;

   inline static unsigned int add(unsigned int& uCell, unsigned int uValue) GCC_INLINE;
   inline static unsigned int sub(unsigned int& uCell, unsigned int uValue) GCC_INLINE;

  protected:
   inline static int log_base_2(unsigned long long int uValue) GCC_INLINE;
   inline static int log_base_2(unsigned int uValue) GCC_INLINE;

  public:
   class MidArray {
     private:
      unsigned long long int* plluValue;
      int uIndex;

     public:
      inline MidArray(thisType& biSource, int uIndexSource) GCC_INLINE;
      inline MidArray& operator=(unsigned int uValue) GCC_INLINE;
      inline operator unsigned int() const GCC_INLINE;
   };
   friend class MidArray;
   inline MidArray midArray(int uIndex) GCC_INLINE;
   inline unsigned int midArray(int uIndex) const GCC_INLINE;
   inline unsigned int cmidArray(int uIndex) const GCC_INLINE; 
   inline void setMidArray(int uIndex, unsigned int uValue) GCC_INLINE;

   class BitArray {
     private:
      unsigned long long int* plluValue;
      int uIndex;

     public:
      inline BitArray(thisType& biSource, int uIndexSource) GCC_INLINE;
      inline BitArray(const BitArray& baSource) GCC_INLINE;
      inline BitArray& operator=(const BitArray& baSource) GCC_INLINE;
      inline BitArray& operator=(bool fValue) GCC_INLINE;
      inline operator bool() const GCC_INLINE;
   };
   friend class BitArray;
   inline BitArray bitArray(int uIndex) GCC_INLINE;
   inline bool bitArray(int uIndex) const GCC_INLINE;
   inline bool cbitArray(int uIndex) const GCC_INLINE; 
   inline void setBitArray(int uIndex, bool fValue) GCC_INLINE;
   inline void setTrueBitArray(int uIndex) GCC_INLINE;
   inline void setFalseBitArray(int uIndex) GCC_INLINE;

   inline TBigCellInt() GCC_INLINE;
   inline TBigCellInt(unsigned int uInt) GCC_INLINE;
   inline TBigCellInt(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uInt) GCC_INLINE;

   inline ComparisonResult compare(const thisType& biSource) const GCC_INLINE;
   inline bool operator<(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>(const thisType& biSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator==(const thisType& biSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& biSource) const GCC_INLINE; 
   inline thisType& operator<<=(int uShift) GCC_INLINE; 
   inline thisType& operator>>=(int uShift) GCC_INLINE; 
   inline thisType& operator|=(const thisType& biSource) GCC_INLINE; 
   inline thisType& operator^=(const thisType& biSource) GCC_INLINE; 
   inline thisType& operator&=(const thisType& biSource) GCC_INLINE; 
   inline thisType& neg() GCC_INLINE;
   inline thisType& neg(int uShift) GCC_INLINE;
   inline bool isZero() const GCC_INLINE; 
   inline bool hasZero(int uShift) const GCC_INLINE;

   inline Carry add(const thisType& biSource) GCC_INLINE;
   inline Carry sub(const thisType& biSource) GCC_INLINE;
   inline Carry plusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry minusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry inc() GCC_INLINE; 
   inline Carry dec() GCC_INLINE; 

   inline thisType& operator+=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator+(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator-=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator-(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator--() GCC_INLINE; 
   inline thisType& operator++() GCC_INLINE; 

   inline int querySize() const GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   inline thisType& operator/=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator/=(unsigned int uSource) GCC_INLINE;
   inline unsigned long long int operator%(unsigned int uSource) const GCC_INLINE;
   inline thisType& operator%=(const thisType& biSource) GCC_INLINE;

   inline unsigned int log_base_2() const GCC_INLINE; 
   inline unsigned int queryValue() const GCC_INLINE; 
   inline bool isAtomic() const GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
};

inline unsigned long long int&
TBigCellInt<Details::TCellIntegerTraits<2> >::value()
{ return inherited::value(); }  // { return *((unsigned long long int*) _array()); }

inline const unsigned long long int&
TBigCellInt<Details::TCellIntegerTraits<2> >::value() const
{ return inherited::value(); }  // { return *((unsigned long long int*) _array()); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::add(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
         uCell += uValue;
         return (uCell < uTemp) ? 1U : 0U;
      }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::sub(unsigned int& uCell, unsigned int uValue)
{  register unsigned int uTemp = uCell;
         uCell -= uValue;
         return (uCell > uTemp) ? 1U : 0U;
      }

inline int
TBigCellInt<Details::TCellIntegerTraits<2> >::log_base_2(unsigned long long int uValue)
{  register int uResult = 1;
         while ((uValue >>= 1) != 0)
            ++uResult;
         return uResult;
      }

inline int
TBigCellInt<Details::TCellIntegerTraits<2> >::log_base_2(unsigned int uValue)
{  return Details::Access::log_base_2(uValue); }

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::MidArray::MidArray(thisType& biSource, int uIndexSource)
   : plluValue(&biSource.value()), uIndex(uIndexSource) { assert(uIndex < 4); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::MidArray&
TBigCellInt<Details::TCellIntegerTraits<2> >::MidArray::operator=(unsigned int uValue)
{  *plluValue &= (~(~0ULL << 4*sizeof(unsigned int)) << (uIndex*4*sizeof(unsigned int)));
   *plluValue |= (((unsigned long long int) uValue) << (uIndex*4*sizeof(unsigned int)));
   return *this;
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::MidArray::operator unsigned int() const
{  return (*plluValue >> (uIndex*4*sizeof(unsigned int)))
      & ~(~0ULL << 4*sizeof(unsigned int));
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::MidArray
TBigCellInt<Details::TCellIntegerTraits<2> >::midArray(int uIndex)
{  return MidArray(*this, uIndex); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::midArray(int uIndex) const
{  assert(uIndex < 4);
   return (value() >> (uIndex*4*sizeof(unsigned int)))
         & ~(~0ULL << 4*sizeof(unsigned int));
}

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::cmidArray(int uIndex) const
{ return midArray(uIndex); }

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::setMidArray(int uIndex, unsigned int uValue)
{  assert(uIndex < 4);
   value() &= (~(~0ULL << 4*sizeof(unsigned int)) << (uIndex*4*sizeof(unsigned int)));
   value() |= (((unsigned long long int) uValue) << (uIndex*4*sizeof(unsigned int)));
}

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray::BitArray(thisType& biSource, int uIndexSource)
   : plluValue(&biSource.value()), uIndex(uIndexSource) { assert(uIndex < 2*8*sizeof(unsigned int)); }

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray::BitArray(const BitArray& baSource)
   : plluValue(baSource.plluValue), uIndex(baSource.uIndex) {}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray&
TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray::operator=(const BitArray& baSource)
{  plluValue = baSource.plluValue;
   uIndex = baSource.uIndex;
   return *this;
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray&
TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray::operator=(bool fValue)
{  if (fValue)
      *plluValue |= (1ULL << uIndex);
   else
      *plluValue &= ~(1ULL << uIndex);
   return *this;
}

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray::operator bool() const
{  return (*plluValue & (1ULL << uIndex)) != 0ULL; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::BitArray
TBigCellInt<Details::TCellIntegerTraits<2> >::bitArray(int uIndex)
{  return BitArray(*this, uIndex); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::bitArray(int uIndex) const
{  assert(uIndex < 2*8*sizeof(unsigned int));
   return (value() & (1ULL << uIndex)) != 0ULL;
}

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::cbitArray(int uIndex) const
{ return bitArray(uIndex); }

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::setBitArray(int uIndex, bool fValue)
{  assert(uIndex < 2*8*sizeof(unsigned int));
   if (fValue)
      value() |= (1ULL << uIndex);
   else
      value() &= ~(1ULL << uIndex);
}

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::setFalseBitArray(int uIndex)
{  assert(uIndex < 2*8*sizeof(unsigned int));
   value() &= ~(1ULL << uIndex);
}

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::setTrueBitArray(int uIndex)
{  assert(uIndex < 2*8*sizeof(unsigned int));
   value() |= (1ULL << uIndex);
}

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::TBigCellInt() : inherited() {}

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::TBigCellInt(unsigned int uInt) : inherited(uInt) {}

inline
TBigCellInt<Details::TCellIntegerTraits<2> >::TBigCellInt(const thisType& biSource) : inherited(biSource) {}

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator=(const thisType& biSource)
{  return (thisType&) inherited::operator=(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator=(unsigned int uInt)
{  value() = uInt; return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::ComparisonResult
TBigCellInt<Details::TCellIntegerTraits<2> >::compare(const thisType& biSource) const
{  return (value() < biSource.value()) ? CRLess
      : ((value() > biSource.value()) ? CRGreater : CREqual);
}

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator<(const thisType& biSource) const
{ return value() < biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator>(const thisType& biSource) const
{ return value() > biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator<=(const thisType& biSource) const
{ return value() <= biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator>=(const thisType& biSource) const
{ return value() >= biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator==(const thisType& biSource) const
{ return value() == biSource.value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::operator!=(const thisType& biSource) const
{ return value() != biSource.value(); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator<<=(int uShift)
{ value() <<= uShift; return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator>>=(int uShift)
{ value() >>= uShift; return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator|=(const thisType& biSource)
{ value() |= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator^=(const thisType& biSource)
{ value() ^= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator&=(const thisType& biSource)
{ value() &= biSource.value(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::neg()
{  value() = ~value();
   return *this;
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::neg(int uShift)
{  if (uShift > 0)
      value() = (~value() & ~(~0ULL << uShift));
   return *this;
}

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::isZero() const
{ return value() == 0U; }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::hasZero(int uShift) const
{  assert(uShift <= 8*sizeof(unsigned long long int));
   return (uShift <= 0)
      || ((value() << (8*sizeof(unsigned long long int)-uShift)) == 0ULL);
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::add(const thisType& biSource)
{  value() += biSource.value();
   return Carry((value() < biSource.value()) ? 1U : 0U);
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::sub(const thisType& biSource)
{  register Carry cCarry((value() < biSource.value()) ? 1U : 0U);
   value() -= biSource.value();
   return cCarry;
}

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::plusAssign(const thisType& biSource)
{ return add(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::minusAssign(const thisType& biSource)
{ return sub(biSource); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::inc()
{ return Carry(++value() == 0U ? 1U : 0U); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >::Carry
TBigCellInt<Details::TCellIntegerTraits<2> >::dec()
{ return Carry(value()-- == 0U ? 1U : 0U); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator+=(const thisType& biSource)
{ add(biSource); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >
TBigCellInt<Details::TCellIntegerTraits<2> >::operator+(const thisType& biSource) const
{  thisType biResult = *this; biResult += biSource; return biResult; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator-=(const thisType& biSource)
{ sub(biSource); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >
TBigCellInt<Details::TCellIntegerTraits<2> >::operator-(const thisType& biSource) const
{  thisType biResult = *this; biResult -= biSource; return biResult; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator--()
{ dec(); return *this; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator++()
{ inc(); return *this; }

inline int
TBigCellInt<Details::TCellIntegerTraits<2> >::querySize() const
{ return 2; }

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::assertSize(int uNewSize)
{ assert(uNewSize <= 2); }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator/=(const thisType& biSource)
{  assert(biSource.value() != 0U);
   value() /= biSource.value();
   return *this;
}


inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator/=(unsigned int uSource)
{  assert(uSource != 0U);
   value() /= uSource;
   return *this;
}

inline unsigned long long int
TBigCellInt<Details::TCellIntegerTraits<2> >::operator%(unsigned int uSource) const
{  return value() % uSource; }

inline TBigCellInt<Details::TCellIntegerTraits<2> >&
TBigCellInt<Details::TCellIntegerTraits<2> >::operator%=(const thisType& biSource)
{  value() %= biSource.value(); return *this; }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::log_base_2() const
{ return log_base_2(value()); }

inline unsigned int
TBigCellInt<Details::TCellIntegerTraits<2> >::queryValue() const
{ return value(); }

inline bool
TBigCellInt<Details::TCellIntegerTraits<2> >::isAtomic() const
{ return !_array()[1]; }

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::swap(thisType& biSource)
{ inherited::swap(biSource); }

inline void
TBigCellInt<Details::TCellIntegerTraits<2> >::clear()
{ value() = 0U; }

#endif // DefineUseLongIntForDoubleInt

/*********************************/
/* Definition - template TBigInt */
/*********************************/

template <class IntegerTraits>
class TBigInt;

namespace Details {

template <int uSize>
class TIntegerTraits {
  public:
   static const int uCellSize = (uSize-1)/(8*sizeof(unsigned int))+1;
#ifndef __BORLANDC__
   typedef TCellIntegerTraits<uCellSize> CellTraits;
#else
   typedef TCellIntegerTraits<(uSize-1)/(8*sizeof(unsigned int))+1> CellTraits;
#endif

  private:
   typedef TIntegerTraits<uSize> thisType;
   CellTraits ctTraits;

  protected:
   inline CellTraits& cellTraits() GCC_INLINE; 
   inline const CellTraits& cellTraits() const GCC_INLINE; 
  public:
   inline TIntegerTraits() GCC_INLINE;
   inline TIntegerTraits(const thisType& itSource) GCC_INLINE;
   inline TIntegerTraits& operator=(const thisType& itSource) GCC_INLINE;
   inline TIntegerTraits& operator=(unsigned int uSource) GCC_INLINE;

   inline static int lastCellIndex() GCC_INLINE; 
   inline static int lastCellSize() GCC_INLINE; 
   inline static bool isComplete() GCC_INLINE; 
   inline void normalizeLastCell() GCC_INLINE;
   inline void normalize() GCC_INLINE;
   
   typedef unsigned int& ArrayProperty;
   inline ArrayProperty array(int uIndex) GCC_INLINE; 
   inline unsigned int array(int uIndex) const GCC_INLINE; 
   inline unsigned int carray(int uIndex) const GCC_INLINE;
   inline ArrayProperty operator[](int uIndex) GCC_INLINE; 
   inline unsigned int operator[](int uIndex) const GCC_INLINE; 
   inline static int querySize() GCC_INLINE; 

   typedef TIntegerTraits<2*uSize> MultResult;
   typedef TIntegerTraits<uSize> QuotientResult;
   typedef TIntegerTraits<uSize> RemainderResult;
   typedef TIntegerTraits<uSize+1> NormalizedRemainderResult;
   inline void assertSize(int uNewSize) GCC_INLINE; 
};

template <int uSize>
inline void
TIntegerTraits<uSize>::assertSize(int uNewSize)
{ assert(uSize >= uNewSize); }

template <int uSize>
inline typename TIntegerTraits<uSize>::CellTraits& TIntegerTraits<uSize>::cellTraits()
{ return ctTraits; }

template <int uSize>
inline const typename TIntegerTraits<uSize>::CellTraits& TIntegerTraits<uSize>::cellTraits() const
{ return ctTraits; }

template <int uSize>
inline TIntegerTraits<uSize>::TIntegerTraits() : ctTraits() {}

template <int uSize>
inline TIntegerTraits<uSize>::TIntegerTraits(const thisType& itSource) : ctTraits(itSource.ctTraits) {}

template <int uSize>
inline TIntegerTraits<uSize>& TIntegerTraits<uSize>::operator=(const thisType& itSource)
{  ctTraits = itSource.ctTraits;
   return *this;
}

template <int uSize>
inline TIntegerTraits<uSize>& TIntegerTraits<uSize>::operator=(unsigned int uSource)
{  ctTraits = uSource;
   return *this;
}

template <int uSize>
inline int TIntegerTraits<uSize>::lastCellIndex()
{ return (uSize-1)/(8*sizeof(unsigned int)); }

template <int uSize>
inline int TIntegerTraits<uSize>::lastCellSize()
{ return ((uSize-1) % (8*sizeof(unsigned int)) +1); }

template <int uSize>
inline bool TIntegerTraits<uSize>::isComplete()
{ return ((uSize % (8*sizeof(unsigned int))) == 0); }

template <int uSize>
inline void TIntegerTraits<uSize>::normalizeLastCell()
{  if (lastCellSize() < (int)(8*sizeof(unsigned int)))
      ctTraits[lastCellIndex()] &= ~(~0U << lastCellSize());
}

template <int uSize>
inline void TIntegerTraits<uSize>::normalize()
{  if (lastCellSize() < (int) (8*sizeof(unsigned int)))
      ctTraits[lastCellIndex()] &= ~(~0U << lastCellSize());
   // for (register int uIndex = lastCellIndex()+1; uIndex < ctTraits.querySize(); ++uIndex)
   //   ctTraits[uIndex] = 0U;
}

template <int uSize>
inline typename TIntegerTraits<uSize>::ArrayProperty
TIntegerTraits<uSize>::array(int uIndex)
{ return ctTraits.array(uIndex); }

template <int uSize>
inline unsigned int
TIntegerTraits<uSize>::array(int uIndex) const
{ return ctTraits.array(uIndex); }

template <int uSize>
inline unsigned int
TIntegerTraits<uSize>::carray(int uIndex) const
{ return array(uIndex); }

template <int uSize>
inline typename TIntegerTraits<uSize>::ArrayProperty
TIntegerTraits<uSize>::operator[](int uIndex)
{ return ctTraits[uIndex]; }

template <int uSize>
inline unsigned int
TIntegerTraits<uSize>::operator[](int uIndex) const
{ return ctTraits[uIndex]; }

template <int uSize>
inline int
TIntegerTraits<uSize>::querySize()
{ return uSize; }

} // end of namespace Details

template <class IntegerTraits>
class TBigInt : public Details::Access, protected IntegerTraits {
  private:
   typedef IntegerTraits inherited;
   typedef TBigInt<IntegerTraits> thisType;
   typedef TBigCellInt<typename IntegerTraits::CellTraits> ArrayCells;

  protected:
   inline static int log_base_2(unsigned int uValue) GCC_INLINE;
      
   inline ArrayCells& cells() GCC_INLINE; 
   inline const ArrayCells& cells() const GCC_INLINE; 

  public:
   typedef typename ArrayCells::MidArray MidArray;
   inline MidArray midArray(int uIndex) GCC_INLINE; 
   inline unsigned int midArray(int uIndex) const GCC_INLINE; 
   inline unsigned int cmidArray(int uIndex) const GCC_INLINE; 

   typedef typename ArrayCells::BitArray BitArray;
   inline BitArray bitArray(int uIndex) GCC_INLINE; 
   inline bool bitArray(int uIndex) const GCC_INLINE; 
   inline bool cbitArray(int uIndex) const GCC_INLINE; 
   inline void setBitArray(int uIndex, bool fValue) GCC_INLINE;
   inline void setTrueBitArray(int uIndex) GCC_INLINE;
   inline void setFalseBitArray(int uIndex) GCC_INLINE;

   inline TBigInt() GCC_INLINE; 
   inline TBigInt(unsigned int uInt) GCC_INLINE; 
   inline TBigInt(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator=(unsigned int uValue) GCC_INLINE;

   typedef typename IntegerTraits::CellTraits::ArrayProperty ArrayProperty;
   inline unsigned int operator[](int uIndex) const GCC_INLINE; 
   inline ArrayProperty operator[](int uIndex) GCC_INLINE; 
   inline unsigned int array(int uIndex) const GCC_INLINE; 
   inline unsigned int carray(int uIndex) const GCC_INLINE;
   inline ArrayProperty array(int uIndex) GCC_INLINE; 
   inline ComparisonResult compare(const thisType& biSource) const GCC_INLINE; 
   inline bool operator<(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>(const thisType& biSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator>=(const thisType& biSource) const GCC_INLINE; 
   inline bool operator==(const thisType& biSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& biSource) const GCC_INLINE; 
   inline thisType& operator<<=(int uShift) GCC_INLINE;
      
   inline thisType& operator>>=(int uShift) GCC_INLINE; 
   inline thisType& operator|=(const thisType& biSource) GCC_INLINE; 
   inline thisType& operator^=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator&=(const thisType& biSource) GCC_INLINE; 
   inline thisType& neg() GCC_INLINE;
   inline thisType& neg(int uShift) GCC_INLINE;
      
   inline thisType operator&(const thisType& biSource) const GCC_INLINE; 
   inline thisType operator|(const thisType& biSource) const GCC_INLINE; 
   inline thisType operator<<(int uShift) const GCC_INLINE; 
   inline thisType operator>>(int uShift) const GCC_INLINE; 
   inline thisType operator~() const GCC_INLINE; 
   
   inline void normalize() GCC_INLINE; 
   inline int lastCellIndex() const GCC_INLINE; 
   typedef typename ArrayCells::Carry Carry;
   inline Carry add(const thisType& biSource) GCC_INLINE;
   inline Carry sub(const thisType& biSource) GCC_INLINE;
   inline Carry plusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry minusAssign(const thisType& biSource) GCC_INLINE; 
   inline Carry inc() GCC_INLINE;
   inline Carry dec() GCC_INLINE;
      
   inline int querySize() const GCC_INLINE; 
   inline int queryCellSize() const GCC_INLINE; 

   inline thisType& operator+=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator+(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator-=(const thisType& biSource) GCC_INLINE; 
   inline thisType operator-(const thisType& biSource) const GCC_INLINE;
   inline thisType& operator--() GCC_INLINE; 
   inline thisType& operator++() GCC_INLINE; 

   inline Carry multAssign(unsigned int uSource) GCC_INLINE;
   typedef typename inherited::MultResult MultResult;
   inline void mult(const thisType& biSource, MultResult& mrResult) const GCC_INLINE;
   inline thisType& operator*=(const thisType& biSource) GCC_INLINE;
   inline thisType& operator*=(unsigned int uSource) GCC_INLINE;

   typedef typename inherited::QuotientResult QuotientResult;
   typedef typename inherited::RemainderResult RemainderResult;
   class DivisionResult {
     private:
      typename inherited::QuotientResult qrQuotient;
      typename inherited::RemainderResult rrRemainder;
      int uComma;

     public:
      inline DivisionResult() GCC_INLINE;
      inline DivisionResult(const DivisionResult& drSource) GCC_INLINE;
      inline DivisionResult& operator=(const DivisionResult& drSource) GCC_INLINE;

      inline const typename inherited::QuotientResult& quotient() const GCC_INLINE; 
      inline typename inherited::QuotientResult& quotient() GCC_INLINE; 
      inline const typename inherited::RemainderResult& remainder() const GCC_INLINE; 
      inline typename inherited::RemainderResult& remainder() GCC_INLINE; 
      inline const int& comma() const GCC_INLINE; 
      inline int& comma() GCC_INLINE; 
   };
   typedef typename inherited::NormalizedRemainderResult NormalizedRemainderResult;
   class NormalizedDivisionResult {
     private:
      typename inherited::QuotientResult qrQuotient;
      typename inherited::NormalizedRemainderResult rrRemainder;
      int uComma;

     public:
      inline NormalizedDivisionResult() GCC_INLINE;
      inline NormalizedDivisionResult(const NormalizedDivisionResult& drSource) GCC_INLINE;
      inline NormalizedDivisionResult& operator=(const NormalizedDivisionResult& drSource) GCC_INLINE;

      inline const typename inherited::QuotientResult& quotient() const GCC_INLINE; 
      inline typename inherited::QuotientResult& quotient() GCC_INLINE; 
      inline const typename inherited::NormalizedRemainderResult& remainder() const GCC_INLINE; 
      inline typename inherited::NormalizedRemainderResult& remainder() GCC_INLINE; 
      inline const int& comma() const GCC_INLINE; 
      inline int& comma() GCC_INLINE; 
   };
   typedef typename ArrayCells::AtomicDivisionResult AtomicDivisionResult;

   void div(const thisType& biSource, DivisionResult& drResult) const;
   void divNormalized(const thisType& biSource, NormalizedDivisionResult& drResult) const;
   AtomicDivisionResult divAssign(unsigned int uSource) { return cells().divAssign(uSource); }

   thisType& operator/=(const thisType& biSource);
   inline thisType& operator/=(unsigned int uSource) GCC_INLINE;
   typename inherited::QuotientResult operator/(const thisType& biSource) const;
   inline unsigned int operator%(unsigned int uSource) const GCC_INLINE;
   inline thisType& operator%=(const thisType& biSource) GCC_INLINE; 

   inline unsigned int log_base_2() const GCC_INLINE; 
   typedef typename ArrayCells::FormatParameters FormatParameters;
   inline void write(std::ostream& osOut, const FormatParameters& pParams) const GCC_INLINE;
   inline void read(std::istream& isIn, const FormatParameters& pParams) GCC_INLINE;
      
   inline unsigned int queryValue() const GCC_INLINE; 
   inline bool isAtomic() const GCC_INLINE; 
   inline bool isZero() const GCC_INLINE; 
   inline bool hasZero(int uShift) const GCC_INLINE; 
   inline void assertSize(int uNewSize) GCC_INLINE; 
   inline void clear() GCC_INLINE; 
   inline void swap(thisType& biSource) GCC_INLINE; 
};

template <class IntegerTraits>
inline int TBigInt<IntegerTraits>::log_base_2(unsigned int uValue)
{  return Details::Access::log_base_2(uValue); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::ArrayCells&
TBigInt<IntegerTraits>::cells()
{ return (ArrayCells&) inherited::cellTraits(); }

template <class IntegerTraits>
inline const typename TBigInt<IntegerTraits>::ArrayCells&
TBigInt<IntegerTraits>::cells() const
{ return (ArrayCells&) inherited::cellTraits(); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::MidArray
TBigInt<IntegerTraits>::midArray(int uIndex)
{ return cells().midArray(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::midArray(int uIndex) const
{ return cells().midArray(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::cmidArray(int uIndex) const
{ return midArray(uIndex); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::BitArray
TBigInt<IntegerTraits>::bitArray(int uIndex)
{ return cells().bitArray(uIndex); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::bitArray(int uIndex) const
{ return cells().bitArray(uIndex); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::cbitArray(int uIndex) const
{ return bitArray(uIndex); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::setBitArray(int uIndex, bool fValue)
{ cells().setBitArray(uIndex, fValue); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::setTrueBitArray(int uIndex)
{ cells().setTrueBitArray(uIndex); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::setFalseBitArray(int uIndex)
{ cells().setFalseBitArray(uIndex); }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>::TBigInt()
{}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>::TBigInt(unsigned int uInt)
{ cells() = uInt; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>::TBigInt(const thisType& biSource) : inherited(biSource) {}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator=(const thisType& biSource)
{  return (thisType&) inherited::operator=(biSource); }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator=(unsigned int uValue)
{  return (thisType&) inherited::operator=(uValue); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::operator[](int uIndex) const
{ return inherited::operator[](uIndex); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::ArrayProperty
TBigInt<IntegerTraits>::operator[](int uIndex)
{ return inherited::operator[](uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::array(int uIndex) const
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::carray(int uIndex) const
{ return array(uIndex); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::ArrayProperty
TBigInt<IntegerTraits>::array(int uIndex)
{ return inherited::array(uIndex); }

template <class IntegerTraits>
inline Details::Access::ComparisonResult
TBigInt<IntegerTraits>::compare(const thisType& biSource) const
{ return cells().compare(biSource.cells()); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator<(const thisType& biSource) const
{ return cells() < biSource.cells(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator>(const thisType& biSource) const
{ return cells() > biSource.cells(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator<=(const thisType& biSource) const
{ return cells() <= biSource.cells(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator>=(const thisType& biSource) const
{ return cells() >= biSource.cells(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator==(const thisType& biSource) const
{ return cells() == biSource.cells(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::operator!=(const thisType& biSource) const
{ return cells() != biSource.cells(); }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator<<=(int uShift)
{  cells() <<= uShift;
   inherited::normalize();
   return *this;
}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator>>=(int uShift)
{  cells() >>= uShift; return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator|=(const thisType& biSource)
{ cells() |= biSource.cells(); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator^=(const thisType& biSource)
{ cells() ^= biSource.cells(); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator&=(const thisType& biSource)
{ cells() &= biSource.cells(); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::neg()
{  cells().neg();
   inherited::normalize();
   return *this;
}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::neg(int uShift)
{  assert(uShift <= inherited::querySize());
   cells().neg(uShift);
   return *this;
}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator&(const thisType& biSource) const
{ thisType biResult(*this); biResult &= biSource; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator|(const thisType& biSource) const
{ thisType biResult(*this); biResult |= biSource; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator<<(int uShift) const
{ thisType biResult(*this); biResult <<= uShift; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator>>(int uShift) const
{ thisType biResult(*this); biResult >>= uShift; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator~() const
{ thisType biResult(*this); biResult.neg(); return biResult; }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::normalize()
{ inherited::normalize(); }

template <class IntegerTraits>
inline int
TBigInt<IntegerTraits>::lastCellIndex() const
{ return inherited::lastCellIndex(); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::add(const thisType& biSource)
{  Carry cResult = cells().add(biSource.cells());
   if (!inherited::isComplete()) {
      cResult = Carry((cells().cbitArray(inherited::querySize())));
      cells().setFalseBitArray(inherited::querySize());
   };
   return cResult;
}

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::sub(const thisType& biSource)
{  Carry cResult = cells().sub(biSource.cells());
   if (cResult.hasCarry() && !inherited::isComplete())
      inherited::normalize();
   return cResult;
}

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::plusAssign(const thisType& biSource)
{ return add(biSource); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::minusAssign(const thisType& biSource)
{ return sub(biSource); }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::inc()
{  Carry cResult = cells().inc();
   if (!inherited::isComplete()) {
      cResult = Carry((cells().cbitArray(inherited::querySize())));
      cells().setFalseBitArray(inherited::querySize());
   };
   return cResult;
}

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::dec()
{  Carry cResult = cells().dec();
   if (cResult.hasCarry() && !inherited::isComplete())
      inherited::normalize();
   return cResult;
}

template <class IntegerTraits>
inline int TBigInt<IntegerTraits>::querySize() const
{ return inherited::querySize(); }

template <class IntegerTraits>
inline int TBigInt<IntegerTraits>::queryCellSize() const
{ return cells().querySize(); }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator+=(const thisType& biSource)
{ add(biSource); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator+(const thisType& biSource) const
{  thisType biResult = *this; biResult += biSource; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator-=(const thisType& biSource)
{ sub(biSource); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>
TBigInt<IntegerTraits>::operator-(const thisType& biSource) const
{  thisType biResult = *this; biResult -= biSource; return biResult; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator--()
{ dec(); return *this; }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator++()
{ inc(); return *this; }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::Carry
TBigInt<IntegerTraits>::multAssign(unsigned int uSource)
{  Carry crResult = cells().multAssign(uSource);
   if (!inherited::isComplete()) {
      if (inherited::lastCellIndex() < cells().querySize()-1) {
         assert(!crResult.hasCarry());
         crResult.carry() = cells()[inherited::lastCellIndex()+1];
      };

      crResult.carry() <<= (8*sizeof(unsigned int) - inherited::lastCellSize());
      if (inherited::lastCellSize() < 8*sizeof(unsigned int))
         crResult.carry() |= (cells()[inherited::lastCellIndex()] >> inherited::lastCellSize());
      inherited::normalize();
   };
   return crResult;    
}

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::mult(const thisType& biSource, MultResult& mrResult) const
{  typename IntegerTraits::CellTraits::MultResult cmrLocalResult;
   cells().mult(biSource.cells(), cmrLocalResult);
   for (register int uIndex = 0; uIndex < cmrLocalResult.querySize(); ++uIndex)
      mrResult[uIndex] = cmrLocalResult[uIndex];
}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator*=(const thisType& biSource)
{  cells() *= biSource.cells();
   inherited::normalize();
   return *this;
}

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator*=(unsigned int uSource)
{  multAssign(uSource); return *this; }

template <class IntegerTraits>
inline
TBigInt<IntegerTraits>::DivisionResult::DivisionResult() : qrQuotient(), rrRemainder(), uComma(0) {}

template <class IntegerTraits>
inline
TBigInt<IntegerTraits>::DivisionResult::DivisionResult(const DivisionResult& drSource)
   : qrQuotient(drSource.qrQuotient), rrRemainder(drSource.rrRemainder), uComma(drSource.uComma) {}

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::DivisionResult&
TBigInt<IntegerTraits>::DivisionResult::operator=(const DivisionResult& drSource)
{  qrQuotient = drSource.qrQuotient;
   rrRemainder = drSource.rrRemainder;
   uComma = drSource.uComma;
   return *this;
}

template <class IntegerTraits>
inline const typename IntegerTraits::QuotientResult&
TBigInt<IntegerTraits>::DivisionResult::quotient() const
{ return qrQuotient; }

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult&
TBigInt<IntegerTraits>::DivisionResult::quotient()
{ return qrQuotient; }

template <class IntegerTraits>
inline const typename IntegerTraits::RemainderResult&
TBigInt<IntegerTraits>::DivisionResult::remainder() const
{ return rrRemainder; }

template <class IntegerTraits>
inline typename IntegerTraits::RemainderResult&
TBigInt<IntegerTraits>::DivisionResult::remainder()
{ return rrRemainder; }

template <class IntegerTraits>
inline const int&
TBigInt<IntegerTraits>::DivisionResult::comma() const
{ return uComma; }

template <class IntegerTraits>
inline int&
TBigInt<IntegerTraits>::DivisionResult::comma()
{ return uComma; }

template <class IntegerTraits>
inline
TBigInt<IntegerTraits>::NormalizedDivisionResult::NormalizedDivisionResult() : qrQuotient(), rrRemainder(), uComma(0) {}

template <class IntegerTraits>
inline
TBigInt<IntegerTraits>::NormalizedDivisionResult::NormalizedDivisionResult(const NormalizedDivisionResult& drSource)
   : qrQuotient(drSource.qrQuotient), rrRemainder(drSource.rrRemainder), uComma(drSource.uComma) {}

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::NormalizedDivisionResult::NormalizedDivisionResult&
TBigInt<IntegerTraits>::NormalizedDivisionResult::operator=(const NormalizedDivisionResult& drSource)
{  qrQuotient = drSource.qrQuotient;
            rrRemainder = drSource.rrRemainder;
            uComma = drSource.uComma;
            return *this;
         }

template <class IntegerTraits>
inline const typename IntegerTraits::QuotientResult&
TBigInt<IntegerTraits>::NormalizedDivisionResult::quotient() const
{ return qrQuotient; }

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult&
TBigInt<IntegerTraits>::NormalizedDivisionResult::quotient()
{ return qrQuotient; }

template <class IntegerTraits>
inline const typename IntegerTraits::NormalizedRemainderResult&
TBigInt<IntegerTraits>::NormalizedDivisionResult::remainder() const
{ return rrRemainder; }

template <class IntegerTraits>
inline typename IntegerTraits::NormalizedRemainderResult&
TBigInt<IntegerTraits>::NormalizedDivisionResult::remainder()
{ return rrRemainder; }

template <class IntegerTraits>
inline const int&
TBigInt<IntegerTraits>::NormalizedDivisionResult::comma() const
{ return uComma; }

template <class IntegerTraits>
inline int&
TBigInt<IntegerTraits>::NormalizedDivisionResult::comma()
{ return uComma; }

template <class IntegerTraits>
inline typename TBigInt<IntegerTraits>::AtomicDivisionResult
TBigInt<IntegerTraits>::divAssign(unsigned int uSource);

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator/=(unsigned int uSource)
{  divAssign(uSource);
         return *this;
      }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::operator%(unsigned int uSource) const
{  return ArrayCells(cells()).divAssign(uSource).remainder(); }
      

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator%=(const thisType& biSource)
{ cells() %= biSource.cells(); return *this; }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::write(std::ostream& osOut, const FormatParameters& pParams) const
{  cells().write(osOut, pParams); }
   
template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::read(std::istream& isIn, const FormatParameters& pParams)
{  cells().read(isIn, pParams); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::log_base_2() const
{ return cells().log_base_2(); }

template <class IntegerTraits>
inline unsigned int
TBigInt<IntegerTraits>::queryValue() const
{ return cells().queryValue(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::isAtomic() const
{ return cells().isAtomic(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::isZero() const
{ return cells().isZero(); }

template <class IntegerTraits>
inline bool
TBigInt<IntegerTraits>::hasZero(int uShift) const
{ return cells().hasZero(uShift); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::assertSize(int uNewSize)
{ inherited::assertSize(uNewSize); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::clear()
{ cells().clear(); }

template <class IntegerTraits>
inline void
TBigInt<IntegerTraits>::swap(thisType& biSource)
{  cells().swap(biSource.cells()); }

template <class IntegerTraits>
inline TBigInt<IntegerTraits>&
TBigInt<IntegerTraits>::operator/=(const thisType& biSource) {
   DivisionResult drResult;
   div(biSource, drResult);
   if (drResult.comma() > 0) {
      inherited::operator=(drResult.quotient());
      operator>>=(inherited::querySize()-drResult.comma());
      setTrueBitArray(drResult.comma());
   }
   else if (drResult.comma() == 0)
      *this = 1U;
   else
      *this = 0U;
   return *this;
}

template <class IntegerTraits>
inline typename IntegerTraits::QuotientResult
TBigInt<IntegerTraits>::operator/(const thisType& biSource) const {
   DivisionResult drResult;
   div(biSource, drResult);
   if (drResult.comma() > 0) {
      TBigInt<typename IntegerTraits::QuotientResult>& pqrPromotedResult
		  = (TBigInt<typename IntegerTraits::QuotientResult>&) drResult.quotient();
      pqrPromotedResult >>= (inherited::querySize()-drResult.comma());
      pqrPromotedResult.setTrueBitArray(drResult.comma());
   }
   else if (drResult.comma() == 0)
      drResult.quotient()[0] = 1U;
   return drResult.quotient();
}

template <class IntegerTraits>
inline std::ostream&
operator<<(std::ostream& osOut, const TBigInt<IntegerTraits>& biInt) {
#ifndef __BORLANDC__
   biInt.write(osOut, typename TBigInt<IntegerTraits>::FormatParameters());
#else
   biInt.write(osOut, TBigInt<IntegerTraits>::FormatParameters());
#endif
   return osOut;
}

template <class IntegerTraits>
inline std::istream&
operator<<(std::istream& isIn, TBigInt<IntegerTraits>& biInt) {
   biInt.read(isIn, typename TBigInt<IntegerTraits>::FormatParameters());
   return isIn;
}

}} // end of namespace Numerics::Integer

} // end of namespace simfloat
} // end of namespace util
} // end of namespace unisim

#endif // Numerics_Integer_IntegerH

