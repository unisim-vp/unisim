/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Franck Vedrine (franck.vedrine@cea.fr)
 */

#ifndef __UNISIM_UTIL_FORBINT_DEBUG_DEBUG_HH__
#define __UNISIM_UTIL_FORBINT_DEBUG_DEBUG_HH__

#include <unisim/util/forbint/debug/instruction_callback.h>
#include <cassert>
#include <cstring>
#include <iostream>

namespace unisim {
namespace util {
namespace forbint {
namespace debug {

enum DefaultTargetEndianness { DTEUndefined, DTELittleEndian, DTEBigEndian };

/*! \class Flags
 * \brief class defining an environment for the application of a scalar operation.
 *   Before the operation, it is possible to indicate a level of approximation
 *   for the result. After the operation, the class contains indications of what
 *   happen during the operation: overflow, division by zero, approximations.
 */

class Flags {
public:
  class Parameters : public ScalarParameters {
  private:
    Flags* _parent;

  public:
    Parameters(Flags& flags) : _parent(&flags)
    { floatingPart = flags._floatingFlags.asUnsigned();
      errorPart = 0U;
      evaluationPart = 0U;
      unsigned creationHeuristics = flags._creationHeuristics.getCode();
      evaluationPart |= (creationHeuristics << 4);
      unsigned isLowLevel = flags._lowLevel ? 1U : 0U;
      evaluationPart |= (isLowLevel << 10);
    }
    ~Parameters()
    { _parent->_floatingFlags.setFrom(floatingPart);
      _parent->_mayError = (errorPart & ~(~0U << (sizeof(unsigned short) * 8)));
      _parent->_sureError = (errorPart >> (sizeof(unsigned short) * 8));
      _parent->_areInvalidFloatingFlags = evaluationPart & 0x1U;
      _parent->_isEmpty = evaluationPart & (0x1U << 1);
      _parent->_isApproximateApplication = evaluationPart & (0x1U << 2);
      _parent->_isUnstable = evaluationPart & (0x1U << 3);
    }
  };

  enum ErrorCode {
    EENoError=0, EEPositiveOverflow=1<<1, EENegativeOverflow=1<<2,
    EEPositiveUnderflow=1<<3, EENegativeUnderflow=1<<4, EEDivisionByZero=1<<5,
    EENegativeModulo=1<<6, EENaN=1<<7
  };

  class MachineStatus {
  public:
    enum Context { CMinDown, CMaxDown, CMinUp, CMaxUp };

    unsigned avoidInfty : 1;
    unsigned keepNaNSign : 1;
    unsigned produceDivNaNPositive : 1;
    unsigned roundToEven : 1;
    unsigned positiveZeroMAdd : 1;
    unsigned upApproximateInfty : 1;
    unsigned upApproximateInversionForNear : 1;
    unsigned chooseNaNAddBeforeMult : 1;
    unsigned convertNaNNegative : 1;
    unsigned refuseMinusZero : 1;
    Context context : 2;
    Context inverseContext : 2;

    bool isPositiveZeroMAdd() { return positiveZeroMAdd; }
    bool isInftyAvoided() const { return avoidInfty; }

    bool isContextUp() const { return context & CMinUp; }
    bool isContextMax() const { return context & CMaxDown; }
    bool doesInverseContext() const { return inverseContext != 0; }
    int size() const { return 14; }
    unsigned asUnsigned() const { unsigned result = 0; memcpy(&result, this, sizeof(unsigned)); return result; }
    void setFrom(unsigned source) { memcpy(this, &source, sizeof(unsigned)); }
  };

  class MachineMode {
  public:
    enum RoundMode { RMNearest, RMLowest, RMHighest, RMZero };
    RoundMode round : 2;
    unsigned keepSignalingConversion : 1;

    bool isRoundToEven(const MachineStatus& status) const
    {  return status.roundToEven && round == RMNearest; }
    bool doesAvoidInfty(const MachineStatus& status, bool isNegative) const
    {  assert(status.avoidInfty);
       return isNegative ? (round >= RMHighest) : (round & RMLowest);
    }

    bool isLowestRound() const  { return round == RMLowest; }
    bool isNearestRound() const { return round == RMNearest; }
    bool isHighestRound() const { return round == RMHighest; }
    bool isZeroRound() const    { return round == RMZero; }

    MachineMode& setKeepSignalingConversion()   { keepSignalingConversion = 1; return *this; }
    MachineMode& clearKeepSignalingConversion() { keepSignalingConversion = 0; return *this; }
    bool getKeepSignalingConversion() const  { return keepSignalingConversion; }
    void clear()
    {  round = RMNearest;
       keepSignalingConversion = 0;
    }
    int size() const { return 3; }
    unsigned asUnsigned() const { unsigned result = 0; memcpy(&result, this, sizeof(unsigned)); return result; }
    void setFrom(unsigned source) { memcpy(this, &source, sizeof(unsigned)); }
  };

  class FloatingFlags {
  public:
    enum Approximation { AUnknown, AExact, ADownApproximate, AUpApproximate };
    enum ReadResult { RRUnknown, RRTotal, RRPartial };
    enum FlowException { FEUnknown, FENoException, FEOverflow, FEUnderflow };
    enum QNaNResult
    {  QNNRUnknown, QNNRUndefined, QNNRInftyMinusInfty, QNNRInftyOnInfty, QNNRZeroOnZero,
       QNNRInftyMultZero
    };
    enum Boolean { BUnknown, BFalse, BTrue };

    MachineStatus status;
    MachineMode mode;
    Approximation approximation : 2;
    ReadResult readResult : 2;
    Boolean effectiveRoundToEven : 2;
    Boolean sNaNOperand : 2;
    QNaNResult qNaNResult : 3;
    FlowException flowException : 2;
    Boolean divisionByZero : 2;

    void clear()
    {  approximation = AUnknown;
       readResult = RRTotal;
       effectiveRoundToEven = BFalse;
       sNaNOperand = BFalse;
       qNaNResult = QNNRUndefined;
       flowException = FEUnknown;
       divisionByZero = BFalse;
    }
    int size() const { return 15 + status.size() + mode.size(); } // = 32
    unsigned asUnsigned() const
    {  unsigned result = 0;
       memcpy(&result, this, sizeof(unsigned));
       result &= ~(~0U << 15);
       result |= (status.asUnsigned() >> 15);
       result |= (mode.asUnsigned() >> (15 + status.size()));
       return result;
    }
    void setFrom(unsigned source)
    {  memcpy(this, &source, sizeof(unsigned));
       status.setFrom((source >> 15) & ~(~0U << status.size()));
       mode.setFrom((source >> (15+status.size())) & ~(~0U << mode.size()));
    }
  };

  class CreationHeuristics {
  public:
    enum Creation { CUndefined, CFormal, CDisjunction, CInterval, CShareTop, CTop };

  private:
    Creation _sureCreation;
    Creation _mayCreation;

  public:
    CreationHeuristics() : _sureCreation(CUndefined), _mayCreation(CUndefined) {}
    CreationHeuristics(Creation cCreationSource) : _sureCreation(cCreationSource), _mayCreation(cCreationSource) {}
    CreationHeuristics(Creation _sureCreationSource, Creation _mayCreationSource)
      : _sureCreation(_sureCreationSource), _mayCreation(_mayCreationSource) {}
    CreationHeuristics(const CreationHeuristics& source) : _sureCreation(source._sureCreation), _mayCreation(source._mayCreation) {}

    unsigned getCode() const { return ((_sureCreation << 3) | _mayCreation); }
    void setFromCode(unsigned code) { _sureCreation = (Creation) (code >> 3); _mayCreation = (Creation) (code & 0x7); }
    
    bool isFormal() const { return _sureCreation == CFormal; }
    bool isDisjunction() const { return _sureCreation == CDisjunction; }
    bool isInterval() const { return _sureCreation == CInterval; }
    bool isShareTop() const { return _sureCreation == CShareTop; }
    bool isTop() const { return _sureCreation == CTop; }
    const Creation& creation() const { return _sureCreation; }
    const Creation& sureCreation() const { return _sureCreation; }
    const Creation& mayCreation() const { return _mayCreation; }

    bool mayBeFormal() const { return _mayCreation <= CFormal; }
    bool mayBeDisjunction() const { return _mayCreation <= CDisjunction; }
    bool mayBeInterval() const { return _mayCreation <= CInterval; }
    bool mayBeShareTop() const { return _mayCreation <= CShareTop; }
    bool mayBeTop() const { return _mayCreation <= CTop; }

    void setFormal() { _sureCreation = _mayCreation = CFormal; }
    void setDisjunction() { _sureCreation = _mayCreation = CDisjunction; }
    void setInterval() { _sureCreation = _mayCreation = CInterval; }
    void setShareTop() { _sureCreation = _mayCreation = CShareTop; }
    void setTop() { _sureCreation = _mayCreation = CTop; }

    void authorizeFormal() { _mayCreation = CFormal; }
    void authorizeDisjunction() { _mayCreation = CDisjunction; }
    void authorizeInterval() { _mayCreation = CInterval; }
    void authorizeShareTop() { _mayCreation = CShareTop; }
    void authorizeTop() { _mayCreation = CTop; }

    bool isValid() const { return _sureCreation >= _mayCreation; }
  };

private:
  unsigned short _mayError;
  unsigned short _sureError;
  FloatingFlags _floatingFlags;
  bool _areInvalidFloatingFlags;
  bool _isEmpty;
  bool _isApproximateApplication;
  bool _isUnstable;

  CreationHeuristics _creationHeuristics;
  bool _lowLevel;

public:
  Flags()
    :  _mayError(0), _sureError(0), _areInvalidFloatingFlags(false), _isEmpty(false),
      _isApproximateApplication(false), _isUnstable(false), _lowLevel(false) {}
  Flags(const Flags& source)
    :  _mayError(source._mayError), _sureError(source._sureError), _floatingFlags(source._floatingFlags),
      _areInvalidFloatingFlags(source._areInvalidFloatingFlags), _isEmpty(source._isEmpty),
      _isApproximateApplication(source._isApproximateApplication), _isUnstable(source._isUnstable),
      _creationHeuristics(source._creationHeuristics), _lowLevel(source._lowLevel) {}

  void clear()
  {  _mayError = _sureError = 0;
     _floatingFlags.clear();
     _areInvalidFloatingFlags = false;
     _isEmpty = _isApproximateApplication = _isUnstable = false;
  }
  bool isEmpty() const { return _isEmpty; }
  bool isStable() const { return !_isUnstable; }
  bool isUnstable() const { return _isUnstable; }
  bool mayHaveError() const { return _mayError != 0; }
  bool mayHaveDivisionByZero() const { return (bool) (_mayError & EEDivisionByZero); }
  bool mayHavePositiveOverflow() const { return (bool) (_mayError & EEPositiveOverflow); }
  bool hasSureDivisionByZero() const { return (bool) (_sureError & EEDivisionByZero); }
  bool hasSurePositiveOverflow() const { return (bool) (_sureError & EEPositiveOverflow); }
  FloatingFlags& floatingPart() { return _floatingFlags; }
  CreationHeuristics& creationHeuristics() { return _creationHeuristics; }
  const CreationHeuristics& creationHeuristics() const { return _creationHeuristics; }
  bool isLowLevel() const { return _lowLevel; }
  void setLowLevel(bool lowLevel=true) { _lowLevel = lowLevel; }

  void setEmpty() { _isEmpty = true; }
  Parameters asParameters() { return Parameters(*this); }
};

class ConstraintFlags : public Flags {
 private:
  struct _InterpretParameters* _implementation;
  struct _InterpretParametersFunctions* _functions;
  struct _ConstraintEnvironment* _environment;
  struct _ModificationDate* _modificationDate;

 public:
  ConstraintFlags(const Flags& source,
      struct _InterpretParameters* implementation,
      struct _InterpretParametersFunctions* functions)
    : Flags(source), _implementation(implementation), _functions(functions), _environment(nullptr), _modificationDate(nullptr) {}
  ConstraintFlags(const ConstraintFlags& source)
    : Flags(source), _implementation(source._implementation), _functions(source._functions),
      _environment(source._environment), _modificationDate(source._modificationDate)
    { const_cast<ConstraintFlags&>(source)._implementation = nullptr;
      const_cast<ConstraintFlags&>(source)._functions = nullptr;
      const_cast<ConstraintFlags&>(source)._environment = nullptr;
      const_cast<ConstraintFlags&>(source)._modificationDate = nullptr;
    }
  struct _ConstraintEnvironment* getConstraintEnvironment() const { return _environment; }
  struct _ModificationDate* getModificationDate() const { return _modificationDate; }
  void setConstraintEnvironment(struct _ConstraintEnvironment* environment)
    { _environment = environment; }
  void setModificationDate(struct _ModificationDate* modificationDate)
    { _modificationDate = modificationDate; }
  ~ConstraintFlags()
    { if (_functions && _implementation) (*_functions->clear_modification_target)(_implementation); }
};


/*! \class Operation
 * \brief Defines all the possible scalar operations.
 */

enum TypeInfo { TIUndefined = 0, TIBit = 1, TIFloat = 2, TIMultiBit = 3 };

class Operation {
private:
  TypeInfo _typeArg    : 3; // to avoid clang warning -Wbitfield-constant-conversion

protected:
  TypeInfo _typeSndArg : 3;
  TypeInfo _typeThdArg : 3;
  TypeInfo _typeResult : 3;
  int      _arity      : 3;

protected:
  Operation(TypeInfo type)
    : _typeArg(type), _typeSndArg(TIUndefined), _typeThdArg(TIUndefined),
      _typeResult(TIUndefined), _arity(0) {}
  Operation(const Operation& source)
    : _typeArg(source._typeArg), _typeSndArg(source._typeSndArg),
      _typeThdArg(source._typeThdArg), _typeResult(source._typeResult),
      _arity(source._arity) {}

public:
  bool isBit() const { return _typeArg == TIBit; }
  bool isFloat() const { return _typeArg == TIFloat; }
  bool isMultiBit() const { return _typeArg == TIMultiBit; }

  TypeInfo getTypeArgument() const { return _typeArg; }
  TypeInfo getTypeSecondArgument() const { return _typeSndArg; }
  TypeInfo getTypeThirdArgument() const { return _typeThdArg; }
  TypeInfo getTypeResult() const { return _typeResult; }
};

namespace Details {

template <class TypeBase>
class TCastMultiBitOperation : public TypeBase {
private:
  typedef TCastMultiBitOperation<TypeBase> thisType;
  typedef TypeBase inherited;
  int _resultSize;
  bool _isSigned;

public:
  TCastMultiBitOperation() : _resultSize(0), _isSigned(false)
    { TypeBase::_typeResult = TIMultiBit; TypeBase::_arity = 0; }
  TCastMultiBitOperation(const thisType& source)
    : inherited(source), _resultSize(source._resultSize), _isSigned(source._isSigned) {}

  thisType& setSize(int resultSize) { _resultSize = resultSize; return *this; }
  thisType& setSigned() { _isSigned = true; return *this; }
  thisType& setUnsigned() { _isSigned = false; return *this; }
  thisType& setSigned(bool sign) { _isSigned = sign; return *this; }
  bool isSigned() const { return _isSigned; }
  int getResultSize() const { return _resultSize; }
};

template <class TypeBase>
class TCastMultiFloatOperation : public TypeBase {
private:
  typedef TCastMultiFloatOperation<TypeBase> thisType;
  typedef TypeBase inherited;
  int _sizeMantissa;
  int _sizeExponent;
  bool _isUnsigned;

public:
  TCastMultiFloatOperation() : _sizeMantissa(0), _sizeExponent(0), _isUnsigned(false)
    { TypeBase::_typeResult = TIFloat; TypeBase::_arity = 0; }
  TCastMultiFloatOperation(const thisType& source)
    :  inherited(source), _sizeMantissa(source._sizeMantissa), _sizeExponent(source._sizeExponent),
      _isUnsigned(source._isUnsigned) {}

  thisType& setSizeExponent(int sizeExponent) { _sizeExponent = sizeExponent; return *this; }
  thisType& setSizeMantissa(int sizeMantissa) { _sizeMantissa = sizeMantissa; return *this; }

  int getSizeExponent() const { return _sizeExponent; }
  int getSizeMantissa() const { return _sizeMantissa; }

  thisType& setUnsigned() { _isUnsigned = true; return *this; }
  thisType& setSigned(bool sign) { _isUnsigned = !sign; return *this; }
  bool isUnsigned() const { return _isUnsigned; }
};

} // end of namespace Details

#define DefineUnaryOperation(Type, TypeResult)                                                   \
  Operation& set##Type() { _arity = 0; _typeResult = TI##TypeResult; return setField(T##Type); }

#define DefineBinaryOperation(Type, TypeArg, TypeResult)                                         \
  Operation& set##Type() { _arity = 1; _typeSndArg = TI##TypeArg; _typeResult = TI##TypeResult; return setField(T##Type); }

#define DefineTernaryOperation(Type, TypeSndArg, TypeThdArg, TypeResult)                         \
  Operation& set##Type() { _arity = 2; _typeSndArg = TI##TypeSndArg; _typeThdArg = TI##TypeThdArg; _typeResult = TI##TypeResult; return setField(T##Type); }

namespace Bit {

class Operation : public debug::Operation {
public:
  enum Type {
    TUndefined, TCastMultiBit, EndOfCast,
    TPrev = EndOfCast, TNext, EndOfUnary,
    TPlus = EndOfUnary, TMinus, EndOfBinary,
    StartOfCompare = EndOfBinary, TCompareLess = StartOfCompare, TCompareLessOrEqual,
    TCompareEqual, TCompareDifferent, TCompareGreaterOrEqual, TCompareGreater, EndOfCompare,
    TMin = EndOfCompare, TMax, EndOfAtomic,
    TOr = EndOfAtomic, TAnd, TExclusiveOr, TNegate, EndOfType
  };

private:
  typedef debug::Operation inherited;
  Type _type;

  Operation& setField(Type type)
  { assert(!_type); _type = type; return *this; }

protected:
  Operation& setCastMultiBit()  { return setField(TCastMultiBit); }

public:
  Operation() : inherited(TIBit), _type(TUndefined) {}
  // Operation(Type type) : inherited(TIBit), _type(type) {}
  Operation(const Operation& source) : inherited(source), _type(source._type) {}

  Type getType() const { return _type; }
  void setUnaryType(Type type, TypeInfo result) { _type = type; _arity = 0; _typeResult = result; }
  void setBinaryType(Type type, TypeInfo arg, TypeInfo result)
  { _type = type; _arity = 1; _typeSndArg = arg; _typeResult = result; }
  bool isValid() const { return _type != TUndefined; }

  DefineUnaryOperation(Prev, Bit)
  DefineUnaryOperation(Next, Bit)
  DefineBinaryOperation(Plus, Bit, Bit)
  DefineBinaryOperation(Minus, Bit, Bit)
  DefineBinaryOperation(Min, Bit, Bit)
  DefineBinaryOperation(Max, Bit, Bit)
  DefineBinaryOperation(Or, Bit, Bit)
  DefineBinaryOperation(And, Bit, Bit)
  DefineBinaryOperation(ExclusiveOr, Bit, Bit)
  DefineUnaryOperation(Negate, Bit)

  DefineBinaryOperation(CompareLess, Bit, Bit)
  DefineBinaryOperation(CompareLessOrEqual, Bit, Bit)
  DefineBinaryOperation(CompareEqual, Bit, Bit)
  DefineBinaryOperation(CompareDifferent, Bit, Bit)
  DefineBinaryOperation(CompareGreater, Bit, Bit)
  DefineBinaryOperation(CompareGreaterOrEqual, Bit, Bit)
};

class CastMultiBitOperation : public Details::TCastMultiBitOperation<Operation> {
public:
  CastMultiBitOperation() { Details::TCastMultiBitOperation<Operation>::setCastMultiBit(); }
  CastMultiBitOperation(const CastMultiBitOperation& source) : Details::TCastMultiBitOperation<Operation>(source) {}
};

} // end of namespace Bit

namespace Floating {

class Operation;

}

namespace MultiBit {

class Operation : public debug::Operation {
public:
  enum Type {
    TUndefined, TExtendWithZero, TExtendWithSign, TConcat, TReduce, TBitSet, EndOfExtension,
    TCastMultiBit = EndOfExtension, TCastBit, TCastShiftBit, TCastMultiFloat,
    TCastMultiFloatPointer, EndOfCast,
    TPrevSigned = EndOfCast, TPrevUnsigned, TNextSigned, TNextUnsigned,
    EndOfUnary,
    StartOfBinary = EndOfUnary, TPlusSigned = StartOfBinary, TPlusUnsigned,
    TPlusUnsignedWithSigned, TPlusFloat, TMinusSigned, TMinusUnsigned,
    TMinusFloat, EndOfBinary,
    StartOfCompare = EndOfBinary, TCompareLessSigned = StartOfCompare, TCompareLessOrEqualSigned,
    TCompareLessUnsigned, TCompareLessOrEqualUnsigned, TCompareLessFloat, TCompareLessOrEqualFloat,
    TCompareEqual, TCompareEqualFloat, TCompareDifferentFloat, TCompareDifferent,
    TCompareGreaterOrEqualFloat, TCompareGreaterFloat, TCompareGreaterOrEqualUnsigned,
    TCompareGreaterUnsigned, TCompareGreaterOrEqualSigned, TCompareGreaterSigned, EndOfCompare,
    TMinSigned = EndOfCompare, TMinUnsigned,
    TMinFloat, TMaxSigned, TMaxUnsigned, TMaxFloat, EndOfAtomic,
    TTimesSigned = EndOfAtomic, TTimesUnsigned, TTimesFloat,
    TDivideSigned, TDivideUnsigned, TDivideFloat, TOppositeSigned,
    TOppositeFloat, EndOfNumeric,
    TModuloSigned = EndOfNumeric, TModuloUnsigned, TBitOr, TBitAnd,
    TBitExclusiveOr, TBitNegate, TLeftShift, TLogicalRightShift,
    TArithmeticRightShift, TLeftRotate, TRightRotate, EndOfInteger,
    TAcos = EndOfInteger, TAsin, TAtan, TAtan2, TCeil, TCos, TCosh, TExp, TFabs, TFloor, TFmod,
    TFrexp, TLdexp, TLog, TLog10, TModf, TPow, TSin, TSinh, TSqrt, TTan, TTanh, EndOfType
  };

private:
  Type _type;

public:
  bool isLessCompare() const { return (_type >= TCompareLessSigned) && (_type <= TCompareLessOrEqualFloat); }
  bool isGreaterCompare() const { return (_type >= TCompareGreaterOrEqualFloat) && (_type <= TCompareGreaterSigned); }
  bool isEqualCompare() const { return (_type == TCompareEqual) || (_type == TCompareEqualFloat); }
  bool isDifferentCompare() const { return (_type == TCompareDifferent) || (_type == TCompareDifferentFloat); }

private:
  typedef debug::Operation inherited;
  Operation& setField(Type type) { assert(!_type); _type = type; return *this; }

protected:
  DefineUnaryOperation(CastMultiBit, MultiBit)
  DefineUnaryOperation(ExtendWithZero, MultiBit)
  DefineUnaryOperation(ExtendWithSign, MultiBit)
  DefineUnaryOperation(Reduce, MultiBit)
  DefineBinaryOperation(BitSet, MultiBit, MultiBit)

  DefineUnaryOperation(CastShiftBit, Bit)
  DefineUnaryOperation(CastMultiFloat, Float)
  DefineUnaryOperation(CastMultiFloatPointer, Float)
  DefineBinaryOperation(PlusFloat, MultiBit, MultiBit)
  DefineBinaryOperation(MinusFloat, MultiBit, MultiBit)
  DefineBinaryOperation(MinFloat, MultiBit, MultiBit)
  DefineBinaryOperation(MaxFloat, MultiBit, MultiBit)
  DefineBinaryOperation(TimesFloat, MultiBit, MultiBit)
  DefineBinaryOperation(DivideFloat, MultiBit, MultiBit)
  DefineUnaryOperation(OppositeFloat, MultiBit)
  DefineBinaryOperation(CompareLessFloat, MultiBit, Bit)
  DefineBinaryOperation(CompareLessOrEqualFloat, MultiBit, Bit)
  DefineBinaryOperation(CompareEqualFloat, MultiBit, Bit)
  DefineBinaryOperation(CompareDifferentFloat, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterFloat, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterOrEqualFloat, MultiBit, Bit)

public:
  Operation() : inherited(TIMultiBit), _type(TUndefined) {}
  // Operation(Type type) : inherited(TIMultiBit), _type(type) {}
  Operation(const Operation& source) : inherited(source), _type(source._type) {}

  Type getType() const { return _type; }
  Operation& setUnaryType(Type type, TypeInfo result)
    { _arity = 0; _typeResult = result; _type = type; return *this; }
  Operation& setBinaryType(Type type, TypeInfo argument, TypeInfo result)
    { _arity = 1; _typeSndArg = argument; _typeResult = result; _type = type; return *this; }
  bool isValid() const { return _type > TUndefined; }

  Floating::Operation queryFloatOperation() const;
  Operation& setFloatOperation(const Floating::Operation& operation);

  DefineUnaryOperation(CastBit, Bit)
  DefineUnaryOperation(PrevSigned, MultiBit)
  DefineUnaryOperation(PrevUnsigned, MultiBit)
  DefineUnaryOperation(NextSigned, MultiBit)
  DefineUnaryOperation(NextUnsigned, MultiBit)

  DefineBinaryOperation(Concat, MultiBit, MultiBit)

  DefineBinaryOperation(PlusSigned, MultiBit, MultiBit)
  DefineBinaryOperation(PlusUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(PlusUnsignedWithSigned, MultiBit, MultiBit)
  DefineBinaryOperation(MinusSigned, MultiBit, MultiBit)
  DefineBinaryOperation(MinusUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(MinSigned, MultiBit, MultiBit)
  DefineBinaryOperation(MinUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(MaxSigned, MultiBit, MultiBit)
  DefineBinaryOperation(MaxUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(TimesSigned, MultiBit, MultiBit)
  DefineBinaryOperation(TimesUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(DivideSigned, MultiBit, MultiBit)
  DefineBinaryOperation(DivideUnsigned, MultiBit, MultiBit)
  DefineUnaryOperation(OppositeSigned, MultiBit)
  DefineBinaryOperation(ModuloSigned, MultiBit, MultiBit)
  DefineBinaryOperation(ModuloUnsigned, MultiBit, MultiBit)
  DefineBinaryOperation(BitOr, MultiBit, MultiBit)
  DefineBinaryOperation(BitAnd, MultiBit, MultiBit)
  DefineBinaryOperation(BitExclusiveOr, MultiBit, MultiBit)
  DefineUnaryOperation(BitNegate, MultiBit)
  DefineBinaryOperation(LeftShift, MultiBit, MultiBit)
  DefineBinaryOperation(LogicalRightShift, MultiBit, MultiBit)
  DefineBinaryOperation(ArithmeticRightShift, MultiBit, MultiBit)
  DefineBinaryOperation(LeftRotate, MultiBit, MultiBit)
  DefineBinaryOperation(RightRotate, MultiBit, MultiBit)

  DefineUnaryOperation(Acos, MultiBit)
  DefineUnaryOperation(Asin, MultiBit)
  DefineUnaryOperation(Atan, MultiBit)
  DefineBinaryOperation(Atan2, MultiBit, MultiBit)
  DefineUnaryOperation(Ceil, MultiBit)
  DefineUnaryOperation(Cos, MultiBit)
  DefineUnaryOperation(Cosh, MultiBit)
  DefineUnaryOperation(Exp, MultiBit)
  DefineUnaryOperation(Fabs, MultiBit)
  DefineUnaryOperation(Floor, MultiBit)
  DefineBinaryOperation(Fmod, MultiBit, MultiBit)
  DefineBinaryOperation(Frexp, MultiBit, MultiBit)
  DefineBinaryOperation(Ldexp, MultiBit, MultiBit)
  DefineUnaryOperation(Log, MultiBit)
  DefineUnaryOperation(Log10, MultiBit)
  DefineBinaryOperation(Modf, MultiBit, MultiBit)
  DefineUnaryOperation(Pow, MultiBit)
  DefineUnaryOperation(Sin, MultiBit)
  DefineUnaryOperation(Sinh, MultiBit)
  DefineUnaryOperation(Sqrt, MultiBit)
  DefineUnaryOperation(Tan, MultiBit)
  DefineUnaryOperation(Tanh, MultiBit)

  DefineBinaryOperation(CompareLessSigned, MultiBit, Bit)
  DefineBinaryOperation(CompareLessOrEqualSigned, MultiBit, Bit)
  DefineBinaryOperation(CompareLessUnsigned, MultiBit, Bit)
  DefineBinaryOperation(CompareLessOrEqualUnsigned, MultiBit, Bit)
  DefineBinaryOperation(CompareEqual, MultiBit, Bit)
  DefineBinaryOperation(CompareDifferent, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterSigned, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterUnsigned, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterOrEqualSigned, MultiBit, Bit)
  DefineBinaryOperation(CompareGreaterOrEqualUnsigned, MultiBit, Bit)
};

#define DefineBasicFloatOperation(Kind)                                                            \
  class Kind##Operation : public Details::TCastMultiFloatOperation<Operation> {                    \
  public:                                                                                          \
  Kind##Operation() { set##Kind(); }                                                               \
  Kind##Operation(const Kind##Operation& source) : Details::TCastMultiFloatOperation<Operation>(source) {}\
};

DefineBasicFloatOperation(CastMultiFloat)
DefineBasicFloatOperation(CastMultiFloatPointer)
DefineBasicFloatOperation(PlusFloat)
DefineBasicFloatOperation(MinusFloat)
DefineBasicFloatOperation(CompareLessFloat)
DefineBasicFloatOperation(CompareLessOrEqualFloat)
DefineBasicFloatOperation(CompareEqualFloat)
DefineBasicFloatOperation(CompareDifferentFloat)
DefineBasicFloatOperation(CompareGreaterFloat)
DefineBasicFloatOperation(CompareGreaterOrEqualFloat)
DefineBasicFloatOperation(MinFloat)
DefineBasicFloatOperation(MaxFloat)
DefineBasicFloatOperation(TimesFloat)
DefineBasicFloatOperation(DivideFloat)
DefineBasicFloatOperation(OppositeFloat)
DefineBasicFloatOperation(Acos)
DefineBasicFloatOperation(Asin)
DefineBasicFloatOperation(Atan)
DefineBasicFloatOperation(Atan2)
DefineBasicFloatOperation(Ceil)
DefineBasicFloatOperation(Cos)
DefineBasicFloatOperation(Cosh)
DefineBasicFloatOperation(Exp)
DefineBasicFloatOperation(Fabs)
DefineBasicFloatOperation(Floor)
DefineBasicFloatOperation(Fmod)
DefineBasicFloatOperation(Frexp)
DefineBasicFloatOperation(Ldexp)
DefineBasicFloatOperation(Log)
DefineBasicFloatOperation(Log10)
DefineBasicFloatOperation(Modf)
DefineBasicFloatOperation(Pow)
DefineBasicFloatOperation(Sin)
DefineBasicFloatOperation(Sinh)
DefineBasicFloatOperation(Sqrt)
DefineBasicFloatOperation(Tan)
DefineBasicFloatOperation(Tanh)

#undef DefineBasicFloatOperation

class CastMultiBitOperation : public Details::TCastMultiBitOperation<Operation> {
  public:
  CastMultiBitOperation() { setCastMultiBit(); }
};

class CastShiftBitOperation : public Operation {
private:
  int _shift;

public:
  CastShiftBitOperation(int shift) : _shift(shift) { setCastShiftBit(); }
  CastShiftBitOperation(const CastShiftBitOperation& source) : Operation(source), _shift(source._shift) {}

  int& sshift() { return _shift; }
  int getShift() const { return _shift; }
};

class ExtendOperation : public Operation {
private:
  typedef Operation inherited;
  int _extension;

public:
  ExtendOperation() : _extension(0) {}
  ExtendOperation(const ExtendOperation& source) : inherited(source), _extension(source._extension) {}

  ExtendOperation& setExtendWithZero(int extension)
  {  assert(_extension == 0); inherited::setExtendWithZero(); _extension = extension; return *this; }
  ExtendOperation& setExtendWithSign(int extension)
  {  assert(_extension == 0); inherited::setExtendWithSign(); _extension = extension; return *this; }
  const int& getExtension() const { return _extension; }
};

class ReduceOperation : public Operation {
private:
  typedef Operation inherited;
  int _lowBit, _highBit;

public:
  ReduceOperation() : _lowBit(0), _highBit(0) { inherited::setReduce(); }
  ReduceOperation(const ReduceOperation& roSource)
    : inherited(roSource), _lowBit(roSource._lowBit), _highBit(roSource._highBit) {}

  ReduceOperation& setLowBit(int lowBit)
  {  assert(_lowBit == 0); _lowBit = lowBit; return *this; }
  ReduceOperation& setHighBit(int highBit)
  {  assert(_highBit == 0); _highBit = highBit; return *this; }
  int& slowBit() { return _lowBit; }
  int& shighBit() { return _highBit; }
  const int& getLowBit() const { return _lowBit; }
  const int& getHighBit() const { return _highBit; }
};

class BitSetOperation : public Operation {
private:
  typedef Operation inherited;
  int _lowBit, _highBit;

public:
  BitSetOperation() : _lowBit(0), _highBit(0) { inherited::setBitSet(); }
  BitSetOperation(const BitSetOperation& source)
    :  inherited(source), _lowBit(source._lowBit), _highBit(source._highBit) {}

  int& slowBit() { return _lowBit; }
  int& shighBit() { return _highBit; }
  const int& getLowBit() const { return _lowBit; }
  const int& getHighBit() const { return _highBit; }

  BitSetOperation& setLowBit(int lowBit) { _lowBit = lowBit; return *this; }
  BitSetOperation& setHighBit(int highBit) { _highBit = highBit; return *this; }
  bool isValid() const { return inherited::isValid() && (_lowBit < _highBit); }
};

} // end of namespace MultiBit

namespace Floating {

class Operation : public debug::Operation {
private:
  typedef debug::Operation inherited;

public:
  enum Type {
    TUndefined, TCastMultiFloat, TCastMultiBit, TCastMultiBitPointer, EndOfCast,
    TPlus = EndOfCast, TMinus, EndOfBinary,
    StartOfCompare = EndOfBinary, TCompareLess = StartOfCompare, TCompareLessOrEqual, TCompareEqual,
    TCompareDifferent, TCompareGreaterOrEqual, TCompareGreater, EndOfCompare,
    StartOfQuery = EndOfCompare, TIsInftyExponent = StartOfQuery, TIsNaN, TIsQNaN, TIsSNaN,
    TIsPositive, TIsZeroExponent, TIsNegative, TCastMantissa, EndOfQuery,
    TMin = EndOfQuery, TMax, EndOfAtomic,
    TTimes = EndOfAtomic, TDivide, EndOfSecondBinary, TOpposite = EndOfSecondBinary,
    TAbs, EndOfSimple,
    StartOfTernary = EndOfSimple, TMultAdd = StartOfTernary, TMultSub, TNegMultAdd, TNegMultSub, EndOfNumeric,
    TAcos = EndOfNumeric, TAsin, TAtan, TAtan2, TCeil, TCos, TCosh, TExp, TFabs, TFloor, TFmod,
    TFrexp, TLdexp, TLog, TLog10, TModf, TPow, TSin, TSinh, TSqrt, TTan, TTanh, EndOfType
  };

private:
  Type _type;

private:
  Operation& setField(Type type) { assert(!_type); _type = type; return *this; }

protected:
  DefineUnaryOperation(CastMultiBit, MultiBit)
  DefineUnaryOperation(CastMultiBitPointer, MultiBit)
  DefineUnaryOperation(CastMultiFloat, Float)

public:
  Operation() : inherited(TIFloat), _type(TUndefined) {}
  // Operation(Type type) : inherited(TIFloat), _type(type) {}
  Operation(const Operation& source) : inherited(source), _type(source._type) {}

  bool isValid() const { return _type > TUndefined; }

  Type getType() const { return _type; }
  void setUnaryType(Type type, TypeInfo result) { _arity = 0; _typeResult = result; _type = type; }
  void setBinaryType(Type type, TypeInfo argument, TypeInfo result)
  { _arity = 1; _typeSndArg = argument; _typeResult = result; _type = type; }

  Operation& setCompare(Type type)
  { assert((type >= StartOfCompare) && (type < EndOfCompare));
    _arity = 1; _typeSndArg = TIFloat; _typeResult = TIBit; 
    return setField(type);
  }
  Operation& setTranscendental(Type type)
  { assert((type >= EndOfNumeric) && (type < EndOfType));
    _arity = 0; _typeResult = TIFloat;
    return setField(type);
  }

  DefineBinaryOperation(Plus, Float, Float)
  DefineBinaryOperation(Minus, Float, Float)
  DefineUnaryOperation(Opposite, Float)
  DefineUnaryOperation(Abs, Float)
  DefineBinaryOperation(CompareLess, Float, Bit)
  DefineBinaryOperation(CompareLessOrEqual, Float, Bit)
  DefineBinaryOperation(CompareEqual, Float, Bit)
  DefineBinaryOperation(CompareDifferent, Float, Bit)
  DefineBinaryOperation(CompareGreater, Float, Bit)
  DefineBinaryOperation(CompareGreaterOrEqual, Float, Bit)
  DefineUnaryOperation(IsInftyExponent, Bit)
  DefineUnaryOperation(IsNaN, Bit)
  DefineUnaryOperation(IsQNaN, Bit)
  DefineUnaryOperation(IsSNaN, Bit)
  DefineUnaryOperation(IsPositive, Bit)
  DefineUnaryOperation(IsZeroExponent, Bit)
  DefineUnaryOperation(IsNegative, Bit)
  DefineUnaryOperation(CastMantissa, MultiBit)
  DefineBinaryOperation(Min, Float, Float)
  DefineBinaryOperation(Max, Float, Float)
  DefineBinaryOperation(Times, Float, Float)
  DefineBinaryOperation(Divide, Float, Float)
  DefineTernaryOperation(MultAdd, Float, Float, Float)
  DefineTernaryOperation(MultSub, Float, Float, Float)
  DefineTernaryOperation(NegMultAdd, Float, Float, Float)
  DefineTernaryOperation(NegMultSub, Float, Float, Float)
  DefineUnaryOperation(Acos, Float)
  DefineUnaryOperation(Asin, Float)
  DefineUnaryOperation(Atan, Float)
  DefineBinaryOperation(Atan2, Float, Float)
  DefineUnaryOperation(Ceil, MultiBit)
  DefineUnaryOperation(Cos, Float)
  DefineUnaryOperation(Cosh, Float)
  DefineUnaryOperation(Exp, Float)
  DefineUnaryOperation(Fabs, Float)
  DefineUnaryOperation(Floor, MultiBit)
  DefineBinaryOperation(Fmod, Float, Float)
  DefineBinaryOperation(Frexp, Float, Float)
  DefineBinaryOperation(Ldexp, MultiBit, Float)
  DefineUnaryOperation(Log, Float)
  DefineUnaryOperation(Log10, Float)
  DefineBinaryOperation(Modf, Float, Float)
  DefineBinaryOperation(Pow, Float, Float)
  DefineUnaryOperation(Sin, Float)
  DefineUnaryOperation(Sinh, Float)
  DefineUnaryOperation(Sqrt, Float)
  DefineUnaryOperation(Tan, Float)
  DefineUnaryOperation(Tanh, Float)
};

class CastMultiBitOperation : public Details::TCastMultiBitOperation<Operation> {
public:
  CastMultiBitOperation() { Details::TCastMultiBitOperation<Operation>::setCastMultiBit(); }
};

class CastMultiBitPointerOperation : public Details::TCastMultiBitOperation<Operation> {
public:
  CastMultiBitPointerOperation() { Details::TCastMultiBitOperation<Operation>::setCastMultiBitPointer(); }
};

class CastMultiFloatOperation : public Details::TCastMultiFloatOperation<Operation> {
public:
  CastMultiFloatOperation() { setCastMultiFloat(); }
};

} // end of namespace Floating

#undef DefineUnaryOperation
#undef DefineBinaryOperation
#undef DefineTernaryOperation

namespace MultiBit {

inline Floating::Operation
Operation::queryFloatOperation() const {
  Floating::Operation result;
  if (_type == TPlusFloat)
    result.setPlus();
  else if (_type == TMinusFloat)
    result.setMinus();
  else if ((_type >= TCompareLessFloat) && (_type <= TCompareGreaterFloat))
    result.setCompare((Floating::Operation::Type) ((_type - TCompareLessFloat) + Floating::Operation::StartOfCompare));
  else if (_type == TMinFloat)
    result.setMin();
  else if (_type == TMaxFloat)
    result.setMax();
  else if (_type == TTimesFloat)
    result.setTimes();
  else if (_type == TDivideFloat)
    result.setDivide();
  else if (_type == TOppositeFloat)
    result.setOpposite();
  else if ((_type >= EndOfInteger) && (_type < EndOfType))
    result.setTranscendental((Floating::Operation::Type) ((_type - EndOfInteger) + Floating::Operation::EndOfNumeric));
  return result;
}

inline Operation&
Operation::setFloatOperation(const Floating::Operation& operation) {
  Floating::Operation::Type type = operation.getType();
  if (type == Floating::Operation::TPlus)
    setPlusFloat();
  else if (type == Floating::Operation::TMinus)
    setMinusFloat();
  else if ((type >= Floating::Operation::StartOfCompare) && (type < Floating::Operation::EndOfCompare))
    setField((Type) ((type - Floating::Operation::StartOfCompare) + TCompareLessFloat));
  else if (type == Floating::Operation::TMin)
    setMinFloat();
  else if (type == Floating::Operation::TMax)
    setMaxFloat();
  else if (type == Floating::Operation::TTimes)
    setTimesFloat();
  else if (type == Floating::Operation::TDivide)
    setDivideFloat();
  else if (type == Floating::Operation::TOpposite)
    setOppositeFloat();
  else if ((type >= Floating::Operation::EndOfNumeric) && (type < Floating::Operation::EndOfType))
    setUnaryType((Type) ((type - Floating::Operation::EndOfNumeric) + EndOfType), TIMultiBit);
  return *this;
};

} // end of namespace MultiBit

/*! \class ScalarElement
 * \brief Defines an approximate element. It can be an interval, a disjunction, a single
 *   element, a formal element. The semantics is the one of the std::unique_ptr
 */

class MemoryFlags;
class MemoryState;
class Iteration;

class ScalarElement {
public:
  class ZeroResult {
  private:
    enum Result { ZRUndefined, ZRZero, ZRDifferentZero };
    Result _result;
    friend class ScalarElement;

  public:
    ZeroResult() : _result(ZRUndefined) {}
    ZeroResult(const ZeroResult& zrSource) : _result(zrSource._result) {}

    ZeroResult& setZero()   { _result = ZRZero; return *this; }
    ZeroResult& setDifferentZero() { _result = ZRDifferentZero; return *this; }
    ZeroResult& setFalse()  { _result = ZRZero; return *this; }
    ZeroResult& setTrue()   { _result = ZRDifferentZero; return *this; }
    ZeroResult& setBool(bool fValue) { _result = fValue ? ZRDifferentZero : ZRZero; return *this; }

    void mergeWith(const ZeroResult& source)
    { if (_result != source._result)
        _result = ZRUndefined;
    }
    void intersectWithOnZero(const ZeroResult& source)
    { if (_result != source._result) {
        if (_result == ZRDifferentZero || source._result == ZRDifferentZero)
          _result = ZRDifferentZero;
        else
          _result = ZRUndefined;
      };
    }
    bool isValid() const    { return _result != ZRUndefined; }
    bool isExact() const    { return _result != ZRUndefined; }
    bool isZero() const     { return _result == ZRZero; }
    bool isDifferentZero() const { return _result == ZRDifferentZero; }
    bool isFalse() const    { return _result == ZRZero; }
    bool isTrue() const     { return _result == ZRDifferentZero; }
    bool mayBeFalse() const { return _result != ZRDifferentZero; }
    bool mayBeTrue() const  { return _result != ZRZero; }
    bool mayBeZero() const  { return _result != ZRDifferentZero; }
    bool mayBeDifferentZero() const { return _result != ZRZero; }
    operator bool() const { return (_result == ZRDifferentZero); }
    bool operator!() const { return (_result == ZRZero); }
  };
  typedef class ZeroResult BooleanResult;

private:
  TypeInfo _type : 3;
  struct _Element* _implementation;
  struct _ElementFunctions* _functions;
  friend class MemoryFlags;
  friend class MemoryState;
  friend class Iteration;

public:
  ScalarElement() : _type(TIUndefined), _implementation(nullptr), _functions(nullptr) {}
  ScalarElement(TypeInfo type, struct _Element* implementation,
      struct _ElementFunctions* functions)
    : _type(type), _implementation(implementation), _functions(functions) {}
  ScalarElement(const ScalarElement& source)
    : _type(source._type), _implementation(source._implementation), _functions(source._functions)
  { const_cast<ScalarElement&>(source)._type = TIUndefined;
    const_cast<ScalarElement&>(source)._implementation = nullptr;
    const_cast<ScalarElement&>(source)._functions = nullptr;
  }
  class Duplicate {};
  ScalarElement(const ScalarElement& source, Duplicate)
    : _type(source._type), _implementation(nullptr), _functions(nullptr)
  { if (source._implementation) {
      _functions = source._functions;
      _implementation = (*_functions->copy_element)(source._implementation);
      _type = source._type;
    };
  }
  ~ScalarElement()
  { if (_implementation) {
      (*_functions->free_element)(_implementation);
      _implementation = nullptr;
      _type = TIUndefined;
      _functions = nullptr;
    }
  }

  std::size_t getSize(void) const { return (*_functions->get_size)(_implementation); }
  void clear()
  { if (_implementation) {
      (*_functions->free_element)(_implementation);
      _implementation = nullptr;
      _type = TIUndefined;
      _functions = nullptr;
    }
  }

  ScalarElement clone(void) const
  { return ScalarElement(*this, Duplicate()); }
  ScalarElement& operator=(const ScalarElement& source)
  { if (this != &source) {
      clear();
      assert(_type == TIUndefined);
      _implementation = source._implementation;
      _type = source._type;
      _functions = source._functions;
      const_cast<ScalarElement&>(source)._type = TIUndefined;
      const_cast<ScalarElement&>(source)._implementation = nullptr;
      const_cast<ScalarElement&>(source)._functions = nullptr;
    }

    return *this;
  }

  void setCopy(const ScalarElement& source)
  { if (this != &source && _implementation) {
      (*_functions->free_element)(_implementation);
      _implementation = nullptr;
      _type = TIUndefined;
    };
    if (this != &source && source._implementation) {
      _functions = source._functions;
      _implementation = (*_functions->copy_element)(source._implementation);
      _type = source._type;
    };
  }
  bool isAtomic() const
  { assert(_implementation);
    return (bool) (*_functions->is_atomic)(_implementation);
  }
  void swap(ScalarElement& source)
  { struct _Element* temp = _implementation;
    TypeInfo typeTemp = _type;
    _implementation = source._implementation;
    _type = source._type;
    source._implementation = temp;
    source._type = typeTemp;
    struct _ElementFunctions* functions = _functions;
    _functions = source._functions;
    source._functions = functions;
  }
  bool isValid() const { return _implementation; }
  TypeInfo getTypeInfo() const { return _type; }

  void applyAssign(const Operation& operation, Flags& flags);
  ScalarElement apply(const Operation& operation, Flags& flags) const;
  void applyAssign(const Operation& operation, const ScalarElement& source, Flags& flags);
  ScalarElement apply(const Operation& operation, const ScalarElement& source, Flags& flags) const;
  void applyAssign(const Operation& operation, const ScalarElement& second, const ScalarElement& third, Flags& flags);
  ScalarElement apply(const Operation& operation, const ScalarElement& second, const ScalarElement& third, Flags& flags) const;
  BooleanResult applyBoolean(const Operation& operation, const ScalarElement& source, Flags& flags) const
  { ScalarElement result = apply(operation, source, flags);
    return result.convertBoolean();
  }
  BooleanResult convertBoolean() const;
  ZeroResult queryZeroResult() const { return convertBoolean(); }
  void guardAssign(ScalarElement& thenPart, ScalarElement& elsePart, Flags& flags);
  ScalarElement guard(const ScalarElement& thenPart, const ScalarElement& elsePart, Flags& flags) const;
  static ScalarElement guardConstant(bool condition, ScalarElement& thenPart, ScalarElement& elsePart, Flags& flags);

  void mergeWith(ScalarElement& source, Flags& flags);
  class IntersectResult {
  private:
    bool _isEmpty;
    bool _isStable;

    friend class ScalarElement;
    friend class MemoryState;

  public:
    IntersectResult() : _isEmpty(true), _isStable(false) {}
    IntersectResult(const IntersectResult& source) : _isEmpty(source._isEmpty), _isStable(source._isStable) {}

    bool isEmpty() const { return _isEmpty; }
    bool isStable() const { return _isStable; }
    operator bool() const { return !_isEmpty; }
  };
  class ContainResult : public IntersectResult {
  private:
    friend class ScalarElement;

    ContainResult() {}

  public:
    bool isIntersectionEmpty() const { return isEmpty(); }
    bool isFull() const { return isStable(); }
    operator bool() const { return isFull(); }
  };
  ContainResult contain(const ScalarElement& source, Flags& flags) const;
  IntersectResult intersectWith(ScalarElement& source, Flags& flags, struct _ModificationDate* modification_date);
  IntersectResult intersectWith(ScalarElement& source, ConstraintFlags& flags);

  IntersectResult constraint(const Operation& operation, const ScalarElement& result, Flags& flags,
     struct _ModificationDate* modification_date, struct _ConstraintEnvironment* env);
  IntersectResult constraint(const Operation& operation, ScalarElement& source, const ScalarElement& result, Flags& flags,
     struct _ModificationDate* modification_date, struct _ConstraintEnvironment* env);
  IntersectResult constraint(const Operation& operation, ScalarElement& second, ScalarElement& third, const ScalarElement& result,
     Flags& flags, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* env);

  IntersectResult constraint(const Operation& operation, const ScalarElement& result, Flags& flags, struct _ModificationDate* modification_date)
    { return constraint(operation, result, flags, modification_date, nullptr); }
  IntersectResult constraint(const Operation& operation, ScalarElement& source, const ScalarElement& result, Flags& flags, struct _ModificationDate* modification_date)
    { return constraint(operation, source, result, flags, modification_date, nullptr); }
  IntersectResult constraint(const Operation& operation, ScalarElement& second, ScalarElement& third, const ScalarElement& result, Flags& flags, struct _ModificationDate* modification_date)
    { return constraint(operation, second, third, result, flags, modification_date, nullptr); }
  IntersectResult constraint(const Operation& operation, const ScalarElement& result, ConstraintFlags& flags)
    { return constraint(operation, result, flags, flags.getModificationDate(), flags.getConstraintEnvironment()); }
  IntersectResult constraint(const Operation& operation, ScalarElement& source, const ScalarElement& result, ConstraintFlags& flags)
    { return constraint(operation, source, result, flags, flags.getModificationDate(), flags.getConstraintEnvironment()); }
  IntersectResult constraint(const Operation& operation, ScalarElement& second, ScalarElement& third, const ScalarElement& result, ConstraintFlags& flags)
    { return constraint(operation, second, third, result, flags, flags.getModificationDate(), flags.getConstraintEnvironment()); }

  class CompareSpecialIntegerResult {
  public:
    enum Behaviour
    {  BEqualMinint=1, BEqualMinintP1=2, BLessM1=4, BEqualM1=8, BEqual0=16,
       BEqual1=32, BGreater1=64, BEqualMaxint=128, BEND=256
    };

  private:
    unsigned int _result;
    friend class ScalarElement;

  public:
    CompareSpecialIntegerResult() : _result(0) {}
    CompareSpecialIntegerResult(const CompareSpecialIntegerResult& source) : _result(source._result) {}

    void mergeWith(const CompareSpecialIntegerResult& source) { _result |= source._result; }
    void intersectWith(const CompareSpecialIntegerResult& source) { _result &= source._result; }
    void mergeBehaviour(Behaviour behaviour) { _result |= behaviour; }
    CompareSpecialIntegerResult& opposite();
    bool isNotZero() const                 { return !(_result & BEqual0); }
    bool isZero() const                    { return _result == BEqual0; }
    bool mayBeZero() const                 { return _result & BEqual0; }
    bool mayBeDifferentZero() const        { return _result & ~BEqual0; }

    bool isOne() const                     { return _result == BEqual1; }
    bool mayBeOne() const                  { return _result & BEqual1; }
    bool isMinusOne() const                { return _result == BEqualM1; }
    bool mayBeMinusOne() const             { return _result & BEqualM1; }
    bool isMinint() const                  { return _result == BEqualMinint; }
    bool mayBeMinint() const               { return _result & BEqualMinint; }
    bool isMinintPlusOne() const           { return _result == BEqualMinintP1; }
    bool mayBeMinintPlusOne() const        { return _result & BEqualMinintP1; }
    bool isMaxint() const                  { return _result == BEqualMaxint; }
    bool mayBeMaxint() const               { return _result & BEqualMaxint; }

    bool isGreaterZero() const             { return !(_result & (BEqual1-1)); }
    bool mayBeGreaterZero() const          { return _result & ~(BEqual1-1); }
    bool isGreaterOrEqualZero() const      { return !(_result & (BEqual0-1)); }
    bool mayBeGreaterOrEqualZero() const   { return _result & ~(BEqual0-1); }
    bool isLessZero() const                { return !(_result & ~(BEqual0-1)); }
    bool mayBeLessZero() const             { return _result & (BEqual0-1); }
    bool isLessOrEqualZero() const         { return !(_result & ~(BEqual1-1)); }
    bool mayBeLessOrEqualZero() const      { return _result & (BEqual1-1); }

    bool isLessMinusOne() const            { return !(_result & ~(BEqualM1-1)); }
    bool mayBeLessMinusOne() const         { return _result & (BEqualM1-1); }
    bool isGreaterOne() const              { return !(_result & (BGreater1-1)); }
    bool mayBeGreaterOne() const           { return _result & ~(BGreater1-1); }
    bool isEmpty() const                   { return _result == 0; }
  };

  class CompareSpecialFloatingResult {
  public:
    enum Behaviour { BMInfty = 1, BMin=2, BLessM1=4, BEqualM1=8, BBetweenM1_0=16, BEqual0=32, BBetween0_1=64,
                     BEqual1=128, BGreater1=256, BMax=512, BInfty=1024, BNaN=2048 };

  private:
    unsigned _result;
    friend class ScalarElement;

  public:
    CompareSpecialFloatingResult() : _result(0) {}

    void mergeWith(const CompareSpecialFloatingResult& source) { _result |= source._result; }
    void intersectWith(const CompareSpecialFloatingResult& source) { _result &= source._result; }
    void mergeBehaviour(Behaviour behaviour) { _result |= behaviour; }

    bool isNotZero() const     { return !(_result & BEqual0); }
    bool isZero() const        { return _result == BEqual0; }
    bool mayBeZero() const  	{ return (_result & BEqual0); }
    bool isOne() const         { return _result == BEqual1; }
    bool isMinusOne() const    { return _result == BEqualM1; }
    bool isGreaterZero() const
    {  return !(_result & ~(BBetween0_1|BEqual1|BGreater1|BMax|BInfty)); }
    bool isContractant() const
    {  return !(_result & ~(BEqualM1|BBetweenM1_0|BEqual0|BBetween0_1|BEqual1)); }
    bool isStrictContractant() const
    {  return !(_result & ~(BBetweenM1_0|BEqual0|BBetween0_1)); }
    bool mayBeStrictContractant() const { return (_result & (BBetweenM1_0|BEqual0|BBetween0_1)); }
    bool mayBeExpensive() const { return (_result & (BMInfty|BMin|BLessM1|BGreater1|BMax|BInfty)); }
    bool mayBeGreaterZero() const { return (_result & (BBetween0_1|BEqual1|BGreater1|BMax|BInfty)); }
    bool isGreaterOrEqualZero() const
    {  return !(_result & ~(BEqual0|BBetween0_1|BEqual1|BGreater1|BMax|BInfty)); }
    bool isLessZero() const
    {  return !(_result & ~(BBetweenM1_0|BEqualM1|BLessM1|BMin|BMInfty)); }
    bool mayBeLessZero() const { return (_result & (BLessM1|BEqualM1|BBetweenM1_0|BMin|BMInfty)); }
    bool isLessOrEqualZero() const
    {  return !(_result & ~(BEqual0|BBetweenM1_0|BEqualM1|BLessM1|BMin|BMInfty)); }
    bool isMinusInfty() const { return _result == BMInfty; }
    bool mayBeMinusInfty() const { return _result & BMInfty; }
    bool isInfty() const { return _result == BInfty; }
    bool mayBeInfty() const { return _result & BInfty; }
    bool isMin() const { return _result == BMin; }
    bool mayBeMin() const { return _result & BMin; }
    bool isMax() const { return _result == BMax; }
    bool mayBeMax() const { return _result & BMax; }
    bool isEmpty() const { return _result == 0; }
  };

  CompareSpecialIntegerResult compareSpecialInteger() const;
  CompareSpecialFloatingResult compareSpecialFloating() const;
  const char* print() const
  { const char* result = nullptr;
    if (_implementation)
      result = (*_functions->print_element)(_implementation);
    return result;
  }
};

/*! \class MemoryFlags
 * \brief Defines an approximate element. It can be an interval, a disjunction, a single
 *   element, a formal element.
 */

class Target;
class VirtualInstruction;
class MemoryFlags {
private:
  enum EvaluationError
  {  EENoError=0, EEImpreciseAccess=1<<1, EEUndefinedAccess=1<<2, EENullAccess=1<<3,
     EEUnallocatedAccess=1<<4, EEMemoryExhausted=1<<5, EEOSDependantAccess=1<<6
  };
  struct _InterpretParameters* _implementation;
  struct _InterpretParametersFunctions* _functions;
  Flags _scalarFlags;
  unsigned _error;
  friend class MemoryState;

  void retrieveParameters(struct ScalarParameters& parameters);

public:
  MemoryFlags()
    : _implementation(nullptr), _functions(nullptr), _error(0) {}
  MemoryFlags(struct _InterpretParameters* implementation, struct _InterpretParametersFunctions* functions)
    : _implementation(implementation), _functions(functions), _error(0)
    { Flags::Parameters parameters(_scalarFlags);
      retrieveParameters(parameters);
      unsigned creationHeuristics = parameters.evaluationPart >> 4;
      creationHeuristics &= ~(~0U << 6);
      _scalarFlags.creationHeuristics().setFromCode(parameters.evaluationPart >> 4);
      _scalarFlags.setLowLevel(parameters.evaluationPart >> 10);
    }
  MemoryFlags(const MemoryFlags& source)
    : _implementation(source._implementation), _functions(source._functions),
      _scalarFlags(source._scalarFlags), _error(source._error) {}

  ScalarElement newDisjunction(int size) const;
  void addMayDisjunction(ScalarElement& disjunction, ScalarElement& element) const;
  void addSureDisjunction(ScalarElement& disjunction, ScalarElement& element) const;
  ScalarElement newMultiBitConstant(uint32_t low, uint32_t high, int size /* <= 64 */)
    { uint64_t value = (uint64_t) low | ((uint64_t) high << 32);
      return newMultiBitConstant(&value, size);
    }
  ScalarElement newMultiBitConstant(uint32_t avalue, int size /* <= 32 */) const
    { uint64_t value = avalue;
      return newMultiBitConstant(&value, size);
    }
  ScalarElement newMultiBitConstant(uint64_t* value, int size) const;
  ScalarElement newFloatConstant(double val, int sizeExponent, int sizeMantissa) const;
  ScalarElement newBitConstant(bool value) const;
  ScalarElement newZeroFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newOneFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newTwoFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newByte(char value) const;
  ScalarElement newTop(int size) const;
  ScalarElement newTopFloat(int sizeExponent, int sizeMantissa) const;
  ScalarElement newTopBit() const;
  ScalarElement newInterval(ScalarElement& min, ScalarElement& max) const;
  ScalarElement newByteInterval(char min, char max) const;
  bool isConstant(const ScalarElement& element, int& size) const;
  bool isBounded(const ScalarElement& element, int& size) const;
  bool isConstantDisjunction(const ScalarElement& element, int& numberOfElements, int& size) const;
  void retrieveConstant(const ScalarElement& element, uint64_t* result, int size);
  void retrieveBounds(const ScalarElement& element, uint64_t* min, uint64_t* max, int size);
  void retrieveConstantDisjunction(const ScalarElement& element, uint64_t** result, int size, int numberOfElements);
  
  void setAcceptRequirements(int index)
    { (*_functions->set_accept_requirements)(_implementation, index); }
  void clearRequirements()
    { (*_functions->clear_requirements)(_implementation); }
  void notifySystemCall(const char* name)
    { (*_functions->notify_system_call)(_implementation, const_cast<char*>(name)); }
  void notifySpecialFunctionCall(const char* functionName)
    { (*_functions->notify_special_function_call)(_implementation, const_cast<char*>(functionName)); }
  // void retrieveCurrentAddress(unsigned* address, int sizeAddress)
  //   { (*_functions->get_current_instruction)(_implementation, address, sizeAddress); }
  // void retrieveAddress(const VirtualInstruction& instruction, unsigned* address, int sizeAddress);
  ConstraintFlags setModificationTarget(uint64_t* targetAddress, int sizeAddress)
    { ConstraintFlags result(_scalarFlags, _implementation, _functions);
      struct _ModificationDate* modificationDate = nullptr;
      result.setConstraintEnvironment(
        (*_functions->set_modification_target)(_implementation, targetAddress, sizeAddress, &modificationDate));
      result.setModificationDate(modificationDate);
      return result;
    }
  struct _ModificationDate* getModificationDate() const
   { return (*_functions->get_modification_date)(_implementation); }
   
  class ControlCondition {
    private:
     unsigned _implementation;
     
    public:
     ControlCondition(unsigned implementation=0) : _implementation(implementation) {}
     ControlCondition(const ControlCondition& source) : _implementation(source._implementation) {}
     
     bool isSimple() const { return _implementation == 0; }
     bool isCondition() const { return _implementation >= DIThen && _implementation <= DIElse; }
     bool isThen() const { return _implementation == DIThen; }
     bool isElse() const { return _implementation == DIElse; }
     bool isIdentified() const { return _implementation >= DIIdentified; }

     unsigned queryIdentification() const { assert(_implementation >= DIIdentified); return _implementation - DIIdentified; }
  };
  bool isThenOrigin() const { return (bool) (*_functions->is_then_origin)(_implementation); }
  bool isElseOrigin() const { return (bool) (*_functions->is_else_origin)(_implementation); }
  ControlCondition queryControlCondition() const { return ControlCondition((*_functions->get_control_origin)(_implementation)); }
  VirtualInstruction& queryOriginInstruction(uint64_t* address, int sizeAddress) const 
    { VirtualInstruction* result = reinterpret_cast<VirtualInstruction*>((*_functions->query_origin_instruction)(
         _implementation, address, sizeAddress));
      assert(result); return *result;
    }

  bool isEmpty() const { return _scalarFlags.isEmpty(); }
  void setEmpty() { return _scalarFlags.setEmpty(); }
  bool hasMemoryEvaluationError() const  { return _error != 0; }
  Flags& scalarPart() { return _scalarFlags; }
  void clearScalarPart() { _scalarFlags.clear(); }
  void clearMemoryPart() { _error = 0; }
};

class LocalProperties {
private:
  MemoryFlags& _flags;
  enum Activation { AMemory=0, AScalar=1, ARequirements=2, ACreationHeuristics=3 };
  unsigned _activations;
  Flags::CreationHeuristics _originalCreationHeuristics;

public:
  LocalProperties(MemoryFlags& flags) : _flags(flags), _activations(0) {}
  ~LocalProperties() { clear(); }

  LocalProperties& clear()
  { if (_activations & (1U << AMemory))
      _flags.clearMemoryPart();
    if (_activations & (1U << AScalar))
      _flags.clearScalarPart();
    if (_activations & (1U << ARequirements))
      _flags.clearRequirements();
    if (_activations & (1U << ACreationHeuristics))
      _flags.scalarPart().creationHeuristics() = _originalCreationHeuristics;
    _activations = 0;
    return *this;
  }
  LocalProperties& setAcceptRequirements(int index)
  { assert(!(_activations & (1U << ARequirements)));
    _flags.setAcceptRequirements(index);
    _activations |= (1U << ARequirements);
    return *this;
  }
  LocalProperties& setScalar()
  { assert(!(_activations & (1U << AScalar)));
    _activations |= (1U << AScalar);
    return *this;
  }
  LocalProperties& setMemory()
  { assert(!(_activations & (1U << AMemory)));
    _activations |= (1U << AMemory);
    return *this;
  }
  LocalProperties& setAuthorizeFormalCreation()
  { assert(!(_activations & (1U << ACreationHeuristics)));
    _activations |= (1U << ACreationHeuristics);
    _originalCreationHeuristics = _flags.scalarPart().creationHeuristics();
    _flags.scalarPart().creationHeuristics().authorizeFormal();
    return *this;
  }
  LocalProperties& setDisjunctionCreation()
  { assert(!(_activations & (1U << ACreationHeuristics)));
    _activations |= (1U << ACreationHeuristics);
    _originalCreationHeuristics = _flags.scalarPart().creationHeuristics();
    _flags.scalarPart().creationHeuristics().setDisjunction();
    return *this;
  }
};

class LocalScalarProperties {
private:
  Flags& _flags;
  unsigned _activations;
  Flags::CreationHeuristics _originalCreationHeuristics;

public:
  LocalScalarProperties(Flags& flags) : _flags(flags), _activations(1U) {}
  ~LocalScalarProperties() { clear(); }

  LocalScalarProperties& clear()
  { if (_activations & 1U)
      _flags.clear();
    if (_activations & 2U)
      _flags.creationHeuristics() = _originalCreationHeuristics;
    _activations = 0;
    return *this;
  }
  LocalScalarProperties& setAuthorizeFormalCreation()
  { assert(!(_activations & 2U));
    _activations = 3U;
    _originalCreationHeuristics = _flags.creationHeuristics();
    _flags.creationHeuristics().authorizeFormal();
    return *this;
  }
  LocalScalarProperties& setDisjunctionCreation()
  { assert(!(_activations & 2U));
    _activations = 3U;
    _originalCreationHeuristics = _flags.creationHeuristics();
    _flags.creationHeuristics().setDisjunction();
    return *this;
  }
};

class MemoryState {
private:
  struct _MemoryState* _implementation;
  struct _MemoryStateFunctions* _functions;

public:
  MemoryState(struct _MemoryState* implementation, struct _MemoryStateFunctions* functions)
    : _implementation(implementation), _functions(functions) {}
  MemoryState(const MemoryState& source)
    : _implementation(source._implementation), _functions(source._functions) {}

  void setNumberOfRegisters(int number);
  void setRegisterValue(int registerIndex, ScalarElement& value, MemoryFlags& flags);
  ScalarElement getRegisterValue(int registerIndex, MemoryFlags& flags, TypeInfo type) const;
  ScalarElement loadValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const;
  ScalarElement loadDisjunctiveValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const;
  ScalarElement loadMultiBitValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const;
  ScalarElement loadFloatingValue(ScalarElement& indirectAddress, int size_exponent, int size_mantissa, MemoryFlags& flags) const;
  void storeValue(ScalarElement& indirectAddress, ScalarElement& value, MemoryFlags& flags);
  ScalarElement::IntersectResult constraintStoreValue(ScalarElement& indirectAddress, ScalarElement& value,
    unsigned indirectRegister, MemoryFlags& flags);
  ScalarElement::IntersectResult constraintAddress(ScalarElement& indirectAddress, const ScalarElement& value,
    MemoryFlags& flags);
  ScalarElement createMallocZone(ScalarElement& size, MemoryFlags& flags, int sizeAddress);
  void freeMallocZone(ScalarElement& address, MemoryFlags& flags);
  void specializeForShare()
  { (*_functions->specialize_for_share)(_implementation); }
  int getExternalState() const { return (*_functions->get_external_state)(_implementation); }
};

class Iteration {
public:
  class FamilyInstruction {
  public:
    enum Kind { KUndefined, KSequential, KBranch, KJump, KCall, KReturn, KBranchAndReturn, KEnd };
    enum JumpKind { JKUndefined, JKForward, JKBackward, JKDisjunctive, JKComputed, JKEnd };

  private:
    Kind _kind;
    JumpKind _jumpKind;
    bool _isIncomplete;
    unsigned* _targetValidation;
    int _targetSize;

  public:
    FamilyInstruction()
      :  _kind(KUndefined), _jumpKind(JKUndefined), _isIncomplete(false),
        _targetValidation(nullptr), _targetSize(0) {}
    FamilyInstruction(unsigned* targetPlace, int sizeTarget)
      :  _kind(KUndefined), _jumpKind(JKUndefined), _isIncomplete(false),
        _targetValidation(targetPlace), _targetSize(sizeTarget) {}
    FamilyInstruction(const FamilyInstruction& source)
      :  _kind(source._kind), _jumpKind(source._jumpKind), _isIncomplete(source._isIncomplete),
        _targetValidation(source._targetValidation), _targetSize(source._targetSize) {}

    ::FamilyInstruction getComputedFamily() const
      { return (_kind > 0 ? (_kind - 1) : 0)
            | (_jumpKind > 0 ? ((_jumpKind-1) << 3) : 0)
            | (_isIncomplete << 5);
      }
    void setFromComputedFamily(::FamilyInstruction family)
      {  _kind = (Kind) ((family & 0x7) + 1);
         if (_kind == KBranch || _kind == KJump || _kind == KCall || _kind == KBranchAndReturn)
            _jumpKind = (JumpKind)(((family >> 3) & 0x3) + 1);
         else
            _jumpKind = JKUndefined;
         _isIncomplete = (family >> 5) & 0x1;
         _targetValidation = nullptr;
         _targetSize = 0;
      }
    Kind getKind() const { return _kind; }
    JumpKind getJumpKind() const { return _jumpKind; }

    bool isIncomplete() const { return _isIncomplete; }
    bool isIncompleteReturn() const  { return _isIncomplete && _kind == KReturn; }
    bool isIncompleteJump() const  { return _isIncomplete && _kind == KJump; }

    bool isBranch() const      { return _kind == KBranch || _kind == KBranchAndReturn; }
    bool isJump() const        { return _kind == KJump; }
    bool isCall() const        { return _kind == KCall; }
    bool isReturn(unsigned* target) const
    {  assert(_targetValidation);
       bool result = _kind == KReturn;
        if (!result && (_kind == KBranchAndReturn)) {
          result = true;
          for (int i = 0; result && i < _targetSize; ++i)
            result = _targetValidation[i] == target[i];
        };
        return result;
    }
    bool isSequential() const  { return _kind == KSequential; }
    bool isEnd() const         { return _kind == KEnd; }

    bool isBackwardJump() const { return _jumpKind >= JKBackward; }
    bool isForwardJump() const { return (_kind == KJump && _jumpKind == JKForward); }
    bool isDisjunctiveJump() const { return _jumpKind >= JKDisjunctive; }
    bool isComputedJump() const { return _jumpKind == JKComputed; }
    bool hasTarget() const
    {  return _kind == KBranch || _kind == KBranchAndReturn || (_kind == KJump && _jumpKind >= JKDisjunctive); }

    FamilyInstruction& setIncomplete() { _isIncomplete = true; return *this; }
    FamilyInstruction& setForwardBranch()  { _kind = KBranch; _jumpKind = JKForward; return *this; }
    FamilyInstruction& setBackwardBranch()  { _kind = KBranch; _jumpKind = JKBackward; return *this; }
    FamilyInstruction& setSequential() { _kind = KSequential; return *this; }
    FamilyInstruction& setJump()    { _kind = KJump; return *this; }
    FamilyInstruction& setCall()    { _kind = KCall; return *this; }
    FamilyInstruction& setReturn()  { _kind = KReturn; return *this; }
    FamilyInstruction& setBranch()  { _kind = KBranch; return *this; }
    FamilyInstruction& setBranchAndReturn(unsigned* target, int targetSize)
    {  _kind = KBranchAndReturn;
       if (_targetValidation) {
         assert(_targetSize >= targetSize);
         for (int i = 0; i < targetSize; ++i)
           _targetValidation[i] = target[i];
         if (_targetSize > targetSize) {
           for (int i = targetSize; i < _targetSize; ++i)
             _targetValidation[i] = 0;
         };
       };
       return *this;
    }
    void setEnd()  { _kind = KEnd; }

    void setForwardJump()   { _kind = KJump; _jumpKind = JKForward; }
    void setBackwardJump()  { _kind = KJump; _jumpKind = JKBackward; }
    void setDisjunctiveJump() { _kind = KJump; _jumpKind = JKDisjunctive; }
    void setComputedJump()  { _kind = KJump; _jumpKind = JKComputed; }
    void clear() { _kind = KUndefined; _jumpKind = JKUndefined; _isIncomplete = false; }
  };
  class Target {
    private:
     struct _IterationTarget* _implementation;
     struct _IterationTargetFunctions* _functions;
     
    public:
     Target() : _implementation(nullptr), _functions(nullptr) {}
     Target(struct _IterationTarget* implementation, struct _IterationTargetFunctions* functions)
        :  _implementation(implementation), _functions(functions) {}
     Target(const Target& source)
        :  _implementation(source._implementation), _functions(source._functions)
        {  const_cast<Target&>(source)._implementation = nullptr;
           const_cast<Target&>(source)._functions = nullptr;
        }
     Target& operator=(const Target& source)
        {  _implementation = source._implementation;
           _functions = source._functions;
           const_cast<Target&>(source)._implementation = nullptr;
           const_cast<Target&>(source)._functions = nullptr;
           return *this;
        }
     bool hasAddress(uint64_t* address, int size_address) const
        {  return (bool) (*_functions->has_address)(_implementation, address, size_address); }
     bool isValid() const { return _implementation != nullptr; }
     struct _IterationTarget* getImplementation() const { return _implementation; }
  };
  
  class Exit {
    private:
     struct _IterationExit* _implementation;
     
    public:
     Exit() : _implementation(nullptr) {}
     Exit(struct _IterationExit* implementation) : _implementation(implementation) {}
     Exit(const Exit& source) : _implementation(source._implementation)
        {  const_cast<Exit&>(source)._implementation = nullptr; }
     Exit& operator=(const Exit& source)
        {  _implementation = source._implementation;
           const_cast<Exit&>(source)._implementation = nullptr;
           return *this;
        }
     bool isValid() const { return _implementation != nullptr; }
     struct _IterationExit* getImplementation() const { return _implementation; }
  };
  
  class TargetCursor {
    private:
     struct _IterationTargetCursor* _implementation;
     struct _IterationTargetCursorFunctions* _functions;
     
    public:
     TargetCursor() : _implementation(nullptr), _functions(nullptr) {}
     TargetCursor(struct _IterationTargetCursor* implementation, struct _IterationTargetCursorFunctions* functions)
        :  _implementation(implementation), _functions(functions) {}
     TargetCursor(const TargetCursor& source)
        :  _implementation(source._implementation), _functions(source._functions)
        {  const_cast<TargetCursor&>(source)._implementation = nullptr;
           const_cast<TargetCursor&>(source)._functions = nullptr;
        }
     TargetCursor& operator=(const TargetCursor& source)
        {  if (this != &source && _implementation && _functions)
              (*_functions->free_cursor)(_implementation);
           _implementation = source._implementation;
           _functions = source._functions;
           const_cast<TargetCursor&>(source)._implementation = nullptr;
           const_cast<TargetCursor&>(source)._functions = nullptr;
           return *this;
        }
     ~TargetCursor() { if (_implementation && _functions) (*_functions->free_cursor)(_implementation); }
     bool isValid() const { return _implementation != nullptr; }
     bool setToNext()
        {  assert(_implementation && _functions);
           return (bool) (*_functions->set_to_next)(_implementation);
        }
     Target elementAt(bool& isFirst, bool& isThen, bool& isElse) const
        {  assert(_implementation && _functions);
           struct _IterationTargetFunctions* targetFunctions = nullptr;
           struct _IterationTarget* result = (*_functions->element_at)(_implementation,
              &targetFunctions, &isFirst, &isThen, &isElse);
           return Target(result, targetFunctions);
        }
     Target elementAt() const
        {  assert(_implementation && _functions);
           struct _IterationTargetFunctions* targetFunctions = nullptr;
           bool aisFirst = false, aisThen = false, aisElse = false;
           struct _IterationTarget* result = (*_functions->element_at)(_implementation,
              &targetFunctions, &aisFirst, &aisThen, &aisElse);
           return Target(result, targetFunctions);
        }
     Exit exitAt() const
        {  assert(_implementation && _functions);
           struct _IterationExit* result = (*_functions->exit_at)(_implementation);
           return Exit(result);
        }
  };

  class CalleeCursor {
    private:
     struct _IterationCalleeCursor* _implementation;
     struct _IterationCalleeCursorFunctions* _functions;
     
    public:
     CalleeCursor() : _implementation(nullptr), _functions(nullptr) {}
     CalleeCursor(struct _IterationCalleeCursor* implementation, struct _IterationCalleeCursorFunctions* functions)
        :  _implementation(implementation), _functions(functions) {}
     CalleeCursor(const CalleeCursor& source)
        :  _implementation(source._implementation), _functions(source._functions)
        {  const_cast<CalleeCursor&>(source)._implementation = nullptr;
           const_cast<CalleeCursor&>(source)._functions = nullptr;
        }
     CalleeCursor& operator=(const CalleeCursor& source)
        {  if (this != &source) {
              if (_implementation && _functions)
                 (*_functions->free_cursor)(_implementation);
              _implementation = source._implementation;
              _functions = source._functions;
              const_cast<CalleeCursor&>(source)._implementation = nullptr;
              const_cast<CalleeCursor&>(source)._functions = nullptr;
           }
           return *this;
        }

     ~CalleeCursor() { if (_implementation && _functions) (*_functions->free_cursor)(_implementation); }
     bool isValid() const { return _implementation != nullptr; }
     bool setToNext()
        {  assert(_implementation && _functions);
           return (bool) (*_functions->set_to_next)(_implementation);
        }
     Target elementAt() const
        {  assert(_implementation && _functions);
           struct _IterationTargetFunctions* targetFunctions = nullptr;
           struct _IterationTarget* result = (*_functions->element_at)(_implementation, &targetFunctions);
           return Target(result, targetFunctions);
        }
  };

private:
  struct _Iteration* _implementation;
  bool _mayFollowGraph;
  bool _isFamilyRequired;
  struct _IterationFunctions* _functions;

public:
  Iteration(struct _Iteration* implementation, bool mayFollowGraph,
     bool isFamilyRequired, struct _IterationFunctions* functions)
    : _implementation(implementation), _mayFollowGraph(mayFollowGraph),
      _isFamilyRequired(isFamilyRequired), _functions(functions) {}
  Iteration(const Iteration& source)
    : _implementation(source._implementation), _mayFollowGraph(source._mayFollowGraph),
      _isFamilyRequired(source._isFamilyRequired), _functions(source._functions) {}

  bool mayFollowGraph() const { return _mayFollowGraph; }
  bool isFamilyRequired() const { return _isFamilyRequired; }

  void addTarget(uint64_t* address, int sizeAddress)
    { (*_functions->add_target)(_implementation, address, sizeAddress); }
  void addNextTarget()
    { (*_functions->add_next_target)(_implementation); }
  void addElseTarget(uint64_t* targetAddress, int sizeAddress)
    { (*_functions->add_else_target)(_implementation, targetAddress, sizeAddress); }
  void addThenTarget(uint64_t* targetAddress, int sizeAddress)
    { (*_functions->add_then_target)(_implementation, targetAddress, sizeAddress); }
  void addIdentifiedTarget(uint64_t* address, int sizeAddress, unsigned identification)
    { (*_functions->add_identified_target)(_implementation, address, sizeAddress, identification); }
  void addTarget(const Target& target)
    { (*_functions->add_own_target)(_implementation, target.getImplementation()); }
  void addElseTarget(const Target& target)
    { (*_functions->add_else_own_target)(_implementation, target.getImplementation()); }
  void addThenTarget(const Target& target)
    { (*_functions->add_then_own_target)(_implementation, target.getImplementation()); }
  void addIdentifiedTarget(const Target& target, unsigned identification)
    { (*_functions->add_identified_own_target)(_implementation, target.getImplementation(), identification); }
  bool requiresRegisterValue(int indirectRegister, RequirementLevel requirementLevel)
    { return (bool) (*_functions->requires_register_value)(_implementation, indirectRegister, requirementLevel); }
  bool requiresRegisterValuePart(int indirectRegister, int lowBit, int highBit, RequirementLevel requirementLevel)
    { return (bool) (*_functions->requires_register_value_part)(_implementation, indirectRegister, lowBit, highBit, requirementLevel); }
  bool requiresAddressValue(ScalarElement& address, int size, RequirementLevel requirementLevel)
    { bool result = (*_functions->requires_address_value)(_implementation, address._implementation, size, requirementLevel);
      address._type = TIUndefined;
      address._implementation = nullptr;
      address._functions = nullptr;
      return result;
    }

  Target getLocalTarget()
    { struct _IterationTargetFunctions* functions = nullptr;
      struct _IterationTarget* target = (*_functions->get_local_target)(_implementation, &functions);
      return Target(target, functions);
    }
  bool localAdvanceOnInstruction()
    { return (bool) (*_functions->local_advance_on_instruction)(_implementation); }
  bool advanceOnNextGraphInstruction()
    { return (bool) (*_functions->advance_on_next_graph_instruction)(_implementation); }
  Exit advanceOnExit()
    { return Exit((*_functions->advance_on_exit)(_implementation)); }
  Target advanceOnExitReturn(Exit exit)
    { struct _IterationTargetFunctions* functions = nullptr;
      struct _IterationTarget* target = (*_functions->advance_on_exit_return)(_implementation, exit.getImplementation(), &functions);
      return Target(target, functions);
    }
  void clearFollowGraph() { _mayFollowGraph = false; }
  bool isCallInstruction()
    { return (bool) (*_functions->is_call_instruction)(_implementation); }
  bool advanceOnCallInstruction()
    { return (bool) (*_functions->advance_on_call_instruction)(_implementation); }
  void assumeAcceptEmptyDestination()
    { (*_functions->assume_accept_empty_destination)(_implementation); }
  TargetCursor newCursorOnExistingTargets() const
    { struct _IterationTargetCursorFunctions* functions = nullptr;
      struct _IterationTargetCursor* cursor = (*_functions->new_cursor_on_existing_targets)(_implementation, &functions);
      return TargetCursor(cursor, functions);
    }
  CalleeCursor newCursorOnExistingCallees() const
    { struct _IterationCalleeCursorFunctions* functions = nullptr;
      struct _IterationCalleeCursor* cursor = (*_functions->new_cursor_on_existing_callees)(_implementation, &functions);
      return CalleeCursor(cursor, functions);
    }
  void setFamily(const FamilyInstruction& family)
    { (*_functions->set_family_instruction)(_implementation, family.getComputedFamily()); }

  /* start -- only for DBA */
  ScalarElement newDisjunction(int size) const;
  void addMayDisjunction(ScalarElement& disjunction, ScalarElement& element) const;
  void addSureDisjunction(ScalarElement& disjunction, ScalarElement& element) const;
  ScalarElement newZeroFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newOneFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newTwoFloat(int size_exponent, int size_mantissa) const;
  ScalarElement newByte(char value) const;
  ScalarElement newTop(int size) const;
  ScalarElement newTopFloat(int sizeExponent, int sizeMantissa) const;
  ScalarElement newTopBit() const;
  ScalarElement newInterval(ScalarElement& min, ScalarElement& max) const;
  ScalarElement newByteInterval(char min, char max) const;
  /* end -- only for DBA */

  ScalarElement newMultiBitConstant(uint32_t low, uint32_t high, int size /* <= 64 */)
    { uint64_t value = (uint64_t) low | ((uint64_t) high << 32);
      return newMultiBitConstant(&value, size);
    }
  ScalarElement newMultiBitConstant(uint32_t avalue, int size /* <= 32 */) const
    { uint64_t value = avalue;
      return newMultiBitConstant(&value, size);
    }
  ScalarElement newMultiBitConstant(uint64_t* value, int size) const;
  ScalarElement newFloatConstant(double val, int sizeExponent, int sizeMantissa) const;
  ScalarElement newBitConstant(bool value) const;
  ScalarElement getRegisterValue(int registerIndex, TypeInfo type) const;
  ScalarElement loadValue(ScalarElement& indirectAddress, int size) const;
  ScalarElement loadDisjunctiveValue(ScalarElement& indirectAddress, int size) const;
  ScalarElement loadMultiBitValue(ScalarElement& indirectAddress, int size) const;
  ScalarElement loadFloatingValue(ScalarElement& indirectAddress, int size_exponent, int size_mantissa) const;
  bool isConstant(const ScalarElement& element, int& size) const;
  bool isBounded(const ScalarElement& element, int& size) const;
  bool isConstantDisjunction(const ScalarElement& element, int& numberOfElements, int& size) const;
  void retrieveConstant(const ScalarElement& element, uint64_t* result, int size);
  void retrieveBounds(const ScalarElement& element, uint64_t* min, uint64_t* max, int size);
  void retrieveConstantDisjunction(const ScalarElement& element, uint64_t** result, int size, int numberOfElements);

  /* only for DBA - simplification of expressions */
  void setRegisterValue(int registerIndex, ScalarElement& value);
  void storeValue(ScalarElement& indirectAddress, ScalarElement& value);
  /* end -- only for DBA */

  // void retrieveCurrentAddress(unsigned* address, int sizeAddress)
  //   { (*_functions->retrieve_current_address)(_implementation, address, sizeAddress); }
  const char* getFunctionNameAt(uint64_t* address, int sizeAddress)
    { return (*_functions->get_function_name_at)(_implementation, address, sizeAddress); }
  VirtualInstruction* getPreviousInstruction(uint64_t* address, int sizeAddress) const;
  VirtualInstruction& getCurrentInstruction(uint64_t* address, int sizeAddress) const;
  void notifyDynamicTargets(const ScalarElement& targetElement)
    { (*_functions->notify_dynamic_targets)(_implementation, targetElement._implementation); }
  void notifyUndefinedTarget()
    { (*_functions->notify_undefined_target)(_implementation); }
  int getExternalState() const { return (*_functions->get_external_state)(_implementation); }
  void setExternalStateForTargets(int newState)
    { (*_functions->set_external_state_for_targets)(_implementation, newState); }
};

class Target {
private:
  uint64_t* _address;

public:
  Target(uint64_t* address) : _address(address) { assert(address); }
  Target(const Target& source) : _address(source._address) {}
  
  uint64_t* queryAddress() const { return _address; }
};

class VirtualInstruction {
public:
  VirtualInstruction(void) {}
  virtual ~VirtualInstruction() {}
  virtual VirtualInstruction* clone() const = 0;

  virtual void retrieveTargets(Iteration& iteration) const;
  virtual void interpretForward(uint64_t* address,
      MemoryState& memory, const Target& destination, MemoryFlags& flags) const = 0;
  virtual void interpretBackward(uint64_t* address,
      MemoryState& afterMemory, MemoryState& beforeMemory, uint64_t* afterAddress, MemoryFlags& flags) const = 0;
  virtual void retrieveFamilyInstruction(Iteration::FamilyInstruction& family, uint64_t* target_address_validation) const = 0;

  virtual void write(std::ostream& out, uint64_t* address) const = 0;
  virtual unsigned queryOpcodeAsUnsigned() const = 0;
};

inline void
VirtualInstruction::retrieveTargets(Iteration& iteration) const {
   bool mayFollow = iteration.mayFollowGraph();
   if (mayFollow) {
      if (!iteration.localAdvanceOnInstruction()) {
         if (!iteration.advanceOnNextGraphInstruction()) {
            iteration.clearFollowGraph();
            mayFollow = false;
         };
      }
   }
   if (!mayFollow)
      iteration.addNextTarget();
   if (iteration.isFamilyRequired())
      iteration.setFamily(Iteration::FamilyInstruction().setSequential());
}

} /* namespace debug */
} /* namespace forbint */
} /* namespace util */
} /* namespace unisim */

#endif /* __UNISIM_UTIL_FORBINT_DEBUG_DEBUG_HH__ */
