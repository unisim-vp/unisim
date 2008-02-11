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

#include <endian.h>
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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_CellIntegerTraitsContract
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_CellIntegerTraitsContract
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_TBasicCellIntegerTraits
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_TBasicCellIntegerTraits
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_TCellIntegerTraits
#include <unisim/util/simfloat/Integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_TCellIntegerTraits
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_TBigCellInt
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_TBigCellInt
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_TBigCellInt
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_TBigCellInt
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_TBigCellInt_One
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_TBigCellInt_One
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_TBasicCellIntegerTraits_Two
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_TBasicCellIntegerTraits_Two
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_TBigCellInt_Two
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_TBigCellInt_Two
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_Details_TIntegerTraits
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_Details_TIntegerTraits
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Integer_TBigInt
#include <unisim/util/simfloat/integer_gccopt.inl>
#undef DefineGCC_Inline_Integer_TBigInt
#endif
}} // end of namespace Numerics::Integer

} // end of namespace simfloat
} // end of namespace util
} // end of namespace unisim

#endif // Numerics_Integer_IntegerH

