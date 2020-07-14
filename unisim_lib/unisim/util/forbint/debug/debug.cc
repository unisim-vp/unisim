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

#include <unisim/util/forbint/debug/debug.hh>

namespace unisim {
namespace util {
namespace forbint {
namespace debug {

void
ScalarElement::applyAssign(const Operation& operation, Flags& flags)
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument());
  if (operation.isBit()) {
    Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
    if (typeOperation == Bit::Operation::TCastMultiBit) {
      const Bit::CastMultiBitOperation& realOperation = (const Bit::CastMultiBitOperation&) operation;
      _implementation = (*_functions->apply_assign_bit_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), &parameters);
    }
    else if (typeOperation >= Bit::Operation::TPrev && typeOperation <= Bit::Operation::TNext)
      _implementation = (*_functions->apply_assign_bit_unary)(_implementation,
          (UnaryBitOperationType) (typeOperation-Bit::Operation::TPrev), &parameters);
    else if (typeOperation == Bit::Operation::TNegate)
      _implementation = (*_functions->apply_assign_bit_unary)(_implementation, UBOTNegate, &parameters);
  }
  else if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::StartOfQuery && typeOperation < Floating::Operation::EndOfQuery)
      _implementation = (*_functions->apply_assign_float_unary)(_implementation,
          (UnaryFloatOperationType) (typeOperation-Floating::Operation::StartOfQuery), &parameters);
    else if (typeOperation == Floating::Operation::TOpposite)
      _implementation = (*_functions->apply_assign_float_unary)(_implementation,
          UFOTOpposite, &parameters);
    else if (typeOperation == Floating::Operation::TAbs)
      _implementation = (*_functions->apply_assign_float_unary)(_implementation,
          UFOTAbs, &parameters);
    else if (typeOperation >= Floating::Operation::EndOfNumeric)
      _implementation = (*_functions->apply_assign_float_unary)(_implementation,
          (UnaryFloatOperationType) (UFOTAbs + typeOperation-Floating::Operation::EndOfNumeric), &parameters);
    else if (typeOperation == Floating::Operation::TCastMultiFloat) {
      const Floating::CastMultiFloatOperation& realOperation = (const Floating::CastMultiFloatOperation&) operation;
      _implementation = (*_functions->apply_assign_float_cast_multifloat)(_implementation,
          realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
          realOperation.isUnsigned(), &parameters);
    }
    else if (typeOperation == Floating::Operation::TCastMultiBit) {
      const Floating::CastMultiBitOperation& realOperation = (const Floating::CastMultiBitOperation&) operation;
      _implementation = (*_functions->apply_assign_float_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), false,
          &parameters);
    }
    else if (typeOperation == Floating::Operation::TCastMultiBitPointer) {
      const Floating::CastMultiBitPointerOperation& realOperation = (const Floating::CastMultiBitPointerOperation&) operation;
      _implementation = (*_functions->apply_assign_float_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), true,
          &parameters);
    }
    else
      assert(false);
  }
  else if (operation.isMultiBit()) {
    MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
    if (typeOperation >= MultiBit::Operation::TPrevSigned && typeOperation < MultiBit::Operation::EndOfUnary)
      _implementation = (*_functions->apply_assign_multibit_unary)(_implementation,
          (UnaryMultiBitOperationType) (typeOperation - MultiBit::Operation::TPrevSigned),
          &parameters);
    else if (typeOperation >= MultiBit::Operation::EndOfInteger) {
      const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
      _implementation = (*_functions->apply_assign_multibit_float_unary)(_implementation,
          (UnaryMultiBitFloatOperationType) (typeOperation - MultiBit::Operation::EndOfInteger),
          realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
          realOperation.isUnsigned(), &parameters);
    }
    else {
      switch (typeOperation) {
        case MultiBit::Operation::TOppositeSigned:
          _implementation = (*_functions->apply_assign_multibit_unary)(_implementation,
              UMBOTOppositeSigned, &parameters);
          break;
        case MultiBit::Operation::TOppositeFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_unary)(_implementation,
               UMBOTOppositeFloat, realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
               realOperation.isUnsigned(), &parameters);
          }
          break;
        case MultiBit::Operation::TBitNegate:
           _implementation = (*_functions->apply_assign_multibit_unary)(_implementation,
              UMBOTBitNegate, &parameters);
           break;
        case MultiBit::Operation::TCastMultiBit:
          { const MultiBit::CastMultiBitOperation& realOperation = (const MultiBit::CastMultiBitOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_cast_multibit)(_implementation,
                realOperation.getResultSize(), realOperation.isSigned(),
                &parameters);
          };
          break;
        case MultiBit::Operation::TCastBit:
          _implementation = (*_functions->apply_assign_multibit_cast_bit)(_implementation,
              &parameters);
          break;
        case MultiBit::Operation::TCastShiftBit:
          { const MultiBit::CastShiftBitOperation& realOperation = (const MultiBit::CastShiftBitOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_cast_shift_bit)(_implementation,
                realOperation.getShift(), &parameters);
          };
          break;
        case MultiBit::Operation::TCastMultiFloat:
          { const MultiBit::CastMultiFloatOperation& realOperation = (const MultiBit::CastMultiFloatOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_cast_multifloat)(_implementation,
                realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                realOperation.isUnsigned(), false, &parameters);
          };
          break;
        case MultiBit::Operation::TCastMultiFloatPointer:
          { const MultiBit::CastMultiFloatPointerOperation& realOperation = (const MultiBit::CastMultiFloatPointerOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_cast_multifloat)(_implementation,
                realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                realOperation.isUnsigned(), true, &parameters);
          };
          break;
        case MultiBit::Operation::TExtendWithZero:
          { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_extend)(_implementation,
                realOperation.getExtension(), false, &parameters);
          };
          break;
        case MultiBit::Operation::TExtendWithSign:
          { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_extend)(_implementation,
                realOperation.getExtension(), true, &parameters);
          };
          break;
        case MultiBit::Operation::TReduce:
          { const MultiBit::ReduceOperation& realOperation = (const MultiBit::ReduceOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_reduce)(_implementation,
                realOperation.getLowBit(), realOperation.getHighBit(), &parameters);
          };
          break;
        default:
          assert(false);
      };
    };
  }
  else
    assert(false);
  _type = operation.getTypeResult();
  assert(_type);
}

ScalarElement
ScalarElement::apply(const Operation& operation, Flags& flags) const
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument());
  ScalarElement result;
  if (operation.isBit()) {
    Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
    if (typeOperation == Bit::Operation::TCastMultiBit) {
      const Bit::CastMultiBitOperation& realOperation = (const Bit::CastMultiBitOperation&) operation;
      result._implementation = (*_functions->apply_bit_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), &parameters);
      result._functions = _functions;
    }
    else if (typeOperation >= Bit::Operation::TPrev && typeOperation <= Bit::Operation::TNext) {
      result._implementation = (*_functions->apply_bit_unary)(_implementation,
          (UnaryBitOperationType) (typeOperation-Bit::Operation::TPrev), &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Bit::Operation::TNegate) {
      result._implementation = (*_functions->apply_bit_unary)(_implementation, UBOTNegate, &parameters);
      result._functions = _functions;
    }
  }
  else if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::StartOfQuery && typeOperation < Floating::Operation::EndOfQuery) {
      result._implementation = (*_functions->apply_float_unary)(_implementation,
          (UnaryFloatOperationType) (typeOperation-Floating::Operation::StartOfQuery), &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Floating::Operation::TOpposite) {
      result._implementation = (*_functions->apply_float_unary)(_implementation,
          UFOTOpposite, &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Floating::Operation::TAbs) {
      result._implementation = (*_functions->apply_float_unary)(_implementation,
          UFOTAbs, &parameters);
      result._functions = _functions;
    }
    else if (typeOperation >= Floating::Operation::EndOfNumeric) {
      result._implementation = (*_functions->apply_float_unary)(_implementation,
          (UnaryFloatOperationType) (UFOTAbs + typeOperation-Floating::Operation::EndOfNumeric), &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Floating::Operation::TCastMultiFloat) {
      const Floating::CastMultiFloatOperation& realOperation = (const Floating::CastMultiFloatOperation&) operation;
      result._implementation = (*_functions->apply_float_cast_multifloat)(_implementation,
          realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
          realOperation.isUnsigned(), &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Floating::Operation::TCastMultiBit) {
      const Floating::CastMultiBitOperation& realOperation = (const Floating::CastMultiBitOperation&) operation;
      result._implementation = (*_functions->apply_float_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), false,
          &parameters);
      result._functions = _functions;
    }
    else if (typeOperation == Floating::Operation::TCastMultiBitPointer) {
      const Floating::CastMultiBitPointerOperation& realOperation = (const Floating::CastMultiBitPointerOperation&) operation;
      result._implementation = (*_functions->apply_float_cast_multibit)(_implementation,
          realOperation.getResultSize(), realOperation.isSigned(), true,
          &parameters);
      result._functions = _functions;
    }
    else
      assert(false);
  }
  else if (operation.isMultiBit()) {
    MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
    if (typeOperation >= MultiBit::Operation::TPrevSigned && typeOperation < MultiBit::Operation::EndOfUnary) {
      result._implementation = (*_functions->apply_multibit_unary)(_implementation,
          (UnaryMultiBitOperationType) (typeOperation - MultiBit::Operation::TPrevSigned),
          &parameters);
      result._functions = _functions;
    }
    else if (typeOperation >= MultiBit::Operation::EndOfInteger) {
      const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
      result._implementation = (*_functions->apply_multibit_float_unary)(_implementation,
          (UnaryMultiBitFloatOperationType) (typeOperation - MultiBit::Operation::EndOfInteger),
          realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
          realOperation.isUnsigned(), &parameters);
      result._functions = _functions;
    }
    else {
      switch (typeOperation) {
        case MultiBit::Operation::TOppositeSigned:
          result._implementation = (*_functions->apply_multibit_unary)(_implementation,
              UMBOTOppositeSigned, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TOppositeFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_unary)(_implementation,
                UMBOTOppositeFloat, realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                realOperation.isUnsigned(), &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TBitNegate:
           result._implementation = (*_functions->apply_multibit_unary)(_implementation,
              UMBOTBitNegate, &parameters);
           result._functions = _functions;
           break;
        case MultiBit::Operation::TCastMultiBit:
          { const MultiBit::CastMultiBitOperation& realOperation = (const MultiBit::CastMultiBitOperation&) operation;
            result._implementation = (*_functions->apply_multibit_cast_multibit)(_implementation,
                realOperation.getResultSize(), realOperation.isSigned(),
                &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TCastBit:
          result._implementation = (*_functions->apply_multibit_cast_bit)(_implementation,
              &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCastShiftBit:
          { const MultiBit::CastShiftBitOperation& realOperation = (const MultiBit::CastShiftBitOperation&) operation;
            result._implementation = (*_functions->apply_multibit_cast_shift_bit)(_implementation,
                realOperation.getShift(), &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TCastMultiFloat:
          { const MultiBit::CastMultiFloatOperation& realOperation = (const MultiBit::CastMultiFloatOperation&) operation;
            result._implementation = (*_functions->apply_multibit_cast_multifloat)(_implementation,
                realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                realOperation.isUnsigned(), false, &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TCastMultiFloatPointer:
          { const MultiBit::CastMultiFloatPointerOperation& realOperation = (const MultiBit::CastMultiFloatPointerOperation&) operation;
            result._implementation = (*_functions->apply_multibit_cast_multifloat)(_implementation,
                realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                realOperation.isUnsigned(), true, &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TExtendWithZero:
          { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
            result._implementation = (*_functions->apply_multibit_extend)(_implementation,
                realOperation.getExtension(), false, &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TExtendWithSign:
          { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
            result._implementation = (*_functions->apply_multibit_extend)(_implementation,
                realOperation.getExtension(), true, &parameters);
            result._functions = _functions;
          };
          break;
        case MultiBit::Operation::TReduce:
          { const MultiBit::ReduceOperation& realOperation = (const MultiBit::ReduceOperation&) operation;
            result._implementation = (*_functions->apply_multibit_reduce)(_implementation,
                realOperation.getLowBit(), realOperation.getHighBit(), &parameters);
            result._functions = _functions;
          };
          break;
        default:
          assert(false);
      };
    };
  }
  else
    assert(false);
  result._type = operation.getTypeResult();
  return result;
}

void
ScalarElement::applyAssign(const Operation& operation, const ScalarElement& source, Flags& flags)
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument() && source._type == operation.getTypeSecondArgument());
  if (operation.isBit()) {
    Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
    _implementation = (*_functions->apply_assign_bit_binary)(_implementation,
        (BinaryBitOperationType) (typeOperation-Bit::Operation::EndOfUnary),
        source._implementation, &parameters);
  }
  else if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::EndOfCast && typeOperation < Floating::Operation::EndOfCompare)
      _implementation = (*_functions->apply_assign_float_binary)(_implementation,
          (BinaryFloatOperationType) (typeOperation-Floating::Operation::EndOfCast),
          source._implementation, &parameters);
    else if (typeOperation >= Floating::Operation::EndOfQuery && typeOperation < Floating::Operation::EndOfSecondBinary)
      _implementation = (*_functions->apply_assign_float_binary)(_implementation,
          (BinaryFloatOperationType) (BFOTMin + typeOperation-Floating::Operation::EndOfQuery),
          source._implementation, &parameters);
    else
      assert(false);
  }
  else if (operation.isMultiBit()) {
    MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
    if (typeOperation >= MultiBit::Operation::EndOfNumeric && typeOperation < MultiBit::Operation::EndOfInteger)
      _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
          (BinaryMultiBitOperationType) (BMBOTModuloSigned + typeOperation-MultiBit::Operation::EndOfNumeric),
          source._implementation, &parameters);
    else {
      switch (typeOperation) {
        case MultiBit::Operation::TConcat:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              BMBOTConcat, source._implementation, &parameters);
          break;
        case MultiBit::Operation::TBitSet:
          { const MultiBit::BitSetOperation& realOperation = (const MultiBit::BitSetOperation&) operation;
            _implementation = (*_functions->apply_assign_multibit_bitset)(_implementation,
                source._implementation, realOperation.getLowBit(), realOperation.getHighBit(), &parameters);
          };
          break;
        case MultiBit::Operation::TPlusSigned:
        case MultiBit::Operation::TPlusUnsigned:
        case MultiBit::Operation::TPlusUnsignedWithSigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTPlusSigned + (typeOperation - MultiBit::Operation::TPlusSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TPlusFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTPlusFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TMinusSigned:
        case MultiBit::Operation::TMinusUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTMinusSigned + (typeOperation - MultiBit::Operation::TMinusSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TMinusFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTMinusFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TCompareLessSigned:
        case MultiBit::Operation::TCompareLessOrEqualSigned:
        case MultiBit::Operation::TCompareLessUnsigned:
        case MultiBit::Operation::TCompareLessOrEqualUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTCompareLessSigned + (typeOperation - MultiBit::Operation::TCompareLessSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TCompareLessFloat:
        case MultiBit::Operation::TCompareLessOrEqualFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareLessFloat + typeOperation - MultiBit::Operation::TCompareLessFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TCompareEqual:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              BMBOTCompareEqual, source._implementation, &parameters);
          break;
        case MultiBit::Operation::TCompareEqualFloat:
        case MultiBit::Operation::TCompareDifferentFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareEqualFloat + typeOperation - MultiBit::Operation::TCompareEqualFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TCompareDifferent:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              BMBOTCompareDifferent, source._implementation, &parameters);
          break;
        case MultiBit::Operation::TCompareGreaterOrEqualFloat:
        case MultiBit::Operation::TCompareGreaterFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareGreaterOrEqualFloat + typeOperation - MultiBit::Operation::TCompareGreaterOrEqualFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TCompareGreaterOrEqualUnsigned:
        case MultiBit::Operation::TCompareGreaterUnsigned:
        case MultiBit::Operation::TCompareGreaterOrEqualSigned:
        case MultiBit::Operation::TCompareGreaterSigned:
        case MultiBit::Operation::TMinSigned:
        case MultiBit::Operation::TMinUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTCompareGreaterOrEqualSigned + (typeOperation - MultiBit::Operation::TCompareGreaterOrEqualSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TMinFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTMinFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TMaxSigned:
        case MultiBit::Operation::TMaxUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTMaxSigned + (typeOperation - MultiBit::Operation::TMaxSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TMaxFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTMaxFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TTimesSigned:
        case MultiBit::Operation::TTimesUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTTimesSigned + (typeOperation - MultiBit::Operation::TTimesSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TTimesFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTTimesFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        case MultiBit::Operation::TDivideSigned:
        case MultiBit::Operation::TDivideUnsigned:
          _implementation = (*_functions->apply_assign_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTDivideSigned + (typeOperation - MultiBit::Operation::TDivideSigned)),
              source._implementation, &parameters);
          break;
        case MultiBit::Operation::TDivideFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            _implementation = (*_functions->apply_assign_multibit_float_binary)(_implementation,
                BMBOTDivideFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          break;
        default:
          assert(false);
      };
    };
  }
  else
    assert(false);
  _type = operation.getTypeResult();
}

ScalarElement
ScalarElement::apply(const Operation& operation, const ScalarElement& source, Flags& flags) const
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument() && source._type == operation.getTypeSecondArgument());
  ScalarElement result;
  if (operation.isBit()) {
    Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
    result._implementation = (*_functions->apply_bit_binary)(_implementation,
        (BinaryBitOperationType) (typeOperation-Bit::Operation::EndOfUnary),
        source._implementation, &parameters);
    result._functions = _functions;
  }
  else if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::EndOfCast && typeOperation < Floating::Operation::EndOfCompare) {
      result._implementation = (*_functions->apply_float_binary)(_implementation,
          (BinaryFloatOperationType) (typeOperation-Floating::Operation::EndOfCast),
          source._implementation, &parameters);
      result._functions = _functions;
    }
    else if (typeOperation >= Floating::Operation::EndOfQuery && typeOperation < Floating::Operation::EndOfSecondBinary) {
      result._implementation = (*_functions->apply_float_binary)(_implementation,
          (BinaryFloatOperationType) (BFOTMin + typeOperation-Floating::Operation::EndOfQuery),
          source._implementation, &parameters);
      result._functions = _functions;
    }
    else
      assert(false);
  }
  else if (operation.isMultiBit()) {
    MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
    if (typeOperation >= MultiBit::Operation::EndOfNumeric && typeOperation < MultiBit::Operation::EndOfInteger) {
      result._implementation = (*_functions->apply_multibit_binary)(_implementation,
          (BinaryMultiBitOperationType) (BMBOTModuloSigned + typeOperation-MultiBit::Operation::EndOfNumeric),
          source._implementation, &parameters);
      result._functions = _functions;
    }
    else {
      switch (typeOperation) {
        case MultiBit::Operation::TConcat:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              BMBOTConcat, source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TBitSet:
          { const MultiBit::BitSetOperation& realOperation = (const MultiBit::BitSetOperation&) operation;
            result._implementation = (*_functions->apply_multibit_bitset)(_implementation,
                source._implementation, realOperation.getLowBit(), realOperation.getHighBit(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TPlusSigned:
        case MultiBit::Operation::TPlusUnsigned:
        case MultiBit::Operation::TPlusUnsignedWithSigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTPlusSigned + (typeOperation - MultiBit::Operation::TPlusSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TPlusFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTPlusFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TMinusSigned:
        case MultiBit::Operation::TMinusUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTMinusSigned + (typeOperation - MultiBit::Operation::TMinusSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TMinusFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTMinusFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareLessSigned:
        case MultiBit::Operation::TCompareLessOrEqualSigned:
        case MultiBit::Operation::TCompareLessUnsigned:
        case MultiBit::Operation::TCompareLessOrEqualUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTCompareLessSigned + (typeOperation - MultiBit::Operation::TCompareLessSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareLessFloat:
        case MultiBit::Operation::TCompareLessOrEqualFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareLessFloat + typeOperation - MultiBit::Operation::TCompareLessFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareEqual:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              BMBOTCompareEqual, source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareEqualFloat:
        case MultiBit::Operation::TCompareDifferentFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareEqualFloat + typeOperation - MultiBit::Operation::TCompareEqualFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareDifferent:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              BMBOTCompareDifferent, source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareGreaterOrEqualFloat:
        case MultiBit::Operation::TCompareGreaterFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                (BinaryMultiBitFloatOperationType) (BMBOTCompareGreaterOrEqualFloat + typeOperation - MultiBit::Operation::TCompareGreaterOrEqualFloat),
                source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TCompareGreaterOrEqualUnsigned:
        case MultiBit::Operation::TCompareGreaterUnsigned:
        case MultiBit::Operation::TCompareGreaterOrEqualSigned:
        case MultiBit::Operation::TCompareGreaterSigned:
        case MultiBit::Operation::TMinSigned:
        case MultiBit::Operation::TMinUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTCompareGreaterOrEqualSigned + (typeOperation - MultiBit::Operation::TCompareGreaterOrEqualSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TMinFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTMinFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TMaxSigned:
        case MultiBit::Operation::TMaxUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTMaxSigned + (typeOperation - MultiBit::Operation::TMaxSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TMaxFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTMaxFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TTimesSigned:
        case MultiBit::Operation::TTimesUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTTimesSigned + (typeOperation - MultiBit::Operation::TTimesSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TTimesFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTTimesFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        case MultiBit::Operation::TDivideSigned:
        case MultiBit::Operation::TDivideUnsigned:
          result._implementation = (*_functions->apply_multibit_binary)(_implementation,
              (BinaryMultiBitOperationType) (BMBOTDivideSigned + (typeOperation - MultiBit::Operation::TDivideSigned)),
              source._implementation, &parameters);
          result._functions = _functions;
          break;
        case MultiBit::Operation::TDivideFloat:
          { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
            result._implementation = (*_functions->apply_multibit_float_binary)(_implementation,
                BMBOTDivideFloat, source._implementation, realOperation.getSizeExponent(),
                realOperation.getSizeMantissa(), realOperation.isUnsigned(), &parameters);
          };
          result._functions = _functions;
          break;
        default:
          assert(false);
      };
    };
  }
  else
    assert(false);
  result._type = operation.getTypeResult();
  return result;
}

void
ScalarElement::applyAssign(const Operation& operation, const ScalarElement& second, const ScalarElement& third, Flags& flags)
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument() && second._type == operation.getTypeSecondArgument()
      && third._type == operation.getTypeThirdArgument());
  if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::StartOfTernary && typeOperation < Floating::Operation::EndOfNumeric)
      _implementation = (*_functions->apply_assign_float_ternary)(_implementation,
          (TernaryFloatOperationType) (typeOperation-Floating::Operation::StartOfTernary),
          second._implementation, third._implementation, &parameters);
    else
      assert(false);
  }
  else
    assert(false);
  _type = operation.getTypeResult();
}

ScalarElement
ScalarElement::apply(const Operation& operation, const ScalarElement& second, const ScalarElement& third, Flags& flags) const
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == operation.getTypeArgument() && second._type == operation.getTypeSecondArgument()
      && third._type == operation.getTypeThirdArgument());
  ScalarElement result;
  if (operation.isFloat()) {
    Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
    if (typeOperation >= Floating::Operation::StartOfTernary && typeOperation < Floating::Operation::EndOfNumeric) {
      result._implementation = (*_functions->apply_float_ternary)(_implementation,
          (TernaryFloatOperationType) (typeOperation-Floating::Operation::StartOfTernary),
          second._implementation, third._implementation, &parameters);
      result._functions = _functions;
    }
    else
      assert(false);
  }
  else
    assert(false);
  result._type = operation.getTypeResult();
  return result;
}

ScalarElement::BooleanResult
ScalarElement::convertBoolean() const
{ ZeroComparisonResult compare = (*_functions->convert_boolean)(_implementation);
  assert(_type == TIBit);
  ScalarElement::BooleanResult result;
  result._result = (ScalarElement::BooleanResult::Result) compare;
  return result;
}

void
ScalarElement::guardAssign(ScalarElement& thenPart, ScalarElement& elsePart, Flags& flags)
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == TIBit && thenPart._type == elsePart._type);
  _implementation = (*_functions->guard_assign)(_implementation, thenPart._implementation, elsePart._implementation,
      &parameters);
  _type = thenPart._type;
  thenPart._implementation = nullptr;
  thenPart._type = TIUndefined;
  thenPart._functions = nullptr;
  elsePart._implementation = nullptr;
  elsePart._type = TIUndefined;
  elsePart._functions = nullptr;
}

ScalarElement
ScalarElement::guard(const ScalarElement& thenPart, const ScalarElement& elsePart, Flags& flags) const
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == TIBit && thenPart._type == elsePart._type);
  ScalarElement result;
  ScalarElement thisCopy(*this, ScalarElement::Duplicate());
  ScalarElement thenCopy(thenPart, ScalarElement::Duplicate());
  ScalarElement elseCopy(elsePart, ScalarElement::Duplicate());
  result._implementation = (*_functions->guard)(_implementation, thenCopy._implementation,
      elseCopy._implementation, &parameters);
  thenCopy._implementation = nullptr;
  thenCopy._functions = nullptr;
  elseCopy._implementation = nullptr;
  elseCopy._functions = nullptr;
  result._functions = _functions;
  result._type = thenPart._type;
  return result;
}

ScalarElement
ScalarElement::guardConstant(bool condition, ScalarElement& thenPart, ScalarElement& elsePart, Flags& flags)
{ assert(thenPart._type == elsePart._type);
  if (condition)
     return thenPart;
  return elsePart;
}

void
ScalarElement::mergeWith(ScalarElement& source, Flags& flags)
{ Flags::Parameters parameters = flags.asParameters();
  assert(_type == source._type);
  _implementation = (*_functions->merge_with)(_implementation, source._implementation, &parameters);
  source._implementation = nullptr;
  source._type = TIUndefined;
  source._functions = nullptr;
}

ScalarElement::ContainResult
ScalarElement::contain(const ScalarElement& source, Flags& flags) const
{ assert(_type == source._type);
  { Flags::Parameters parameters = flags.asParameters();
    (*_functions->contain)(_implementation, source._implementation, &parameters);
  };
  ContainResult result;
  result._isEmpty = flags.isEmpty();
  result._isStable = flags.isStable();
  return result;
}

ScalarElement::IntersectResult
ScalarElement::intersectWith(ScalarElement& source, Flags& flags, struct _ModificationDate* modification_date)
{ assert(_type == source._type);
  { Flags::Parameters parameters = flags.asParameters();
    _implementation = (*_functions->intersect_with)(_implementation, source._implementation, &parameters, modification_date, nullptr);
    source._implementation = nullptr;
    source._type = TIUndefined;
    source._functions = nullptr;
  };
  IntersectResult result;
  result._isEmpty = flags.isEmpty();
  result._isStable = flags.isStable();
  return result;
}

ScalarElement::IntersectResult
ScalarElement::intersectWith(ScalarElement& source, ConstraintFlags& flags)
{ assert(_type == source._type);
  { Flags::Parameters parameters = flags.asParameters();
    _implementation = (*_functions->intersect_with)(_implementation, source._implementation, &parameters, flags.getModificationDate(), flags.getConstraintEnvironment());
    source._implementation = nullptr;
    source._type = TIUndefined;
    source._functions = nullptr;
  };
  IntersectResult result;
  result._isEmpty = flags.isEmpty();
  result._isStable = flags.isStable();
  return result;
}

ScalarElement::IntersectResult
ScalarElement::constraint(const Operation& operation, const ScalarElement& result, Flags& flags,
      struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraintEnvironment)
{ assert(_type == operation.getTypeArgument() && result._type == operation.getTypeResult());
  { Flags::Parameters parameters = flags.asParameters();
    if (operation.isBit()) {
      Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
      if (typeOperation == Bit::Operation::TCastMultiBit) {
        const Bit::CastMultiBitOperation& realOperation = (const Bit::CastMultiBitOperation&) operation;
        (*_functions->constraint_assign_bit_cast_multibit)(&_implementation, realOperation.getResultSize(),
            realOperation.isSigned(), result._implementation, &parameters, modification_date, constraintEnvironment);
      }
      else if (typeOperation >= Bit::Operation::TPrev && typeOperation <= Bit::Operation::TNext)
        (*_functions->constraint_assign_bit_unary)(&_implementation,
            (UnaryBitOperationType) (typeOperation-Bit::Operation::TPrev),
            result._implementation, &parameters, modification_date, constraintEnvironment);
      else if (typeOperation == Bit::Operation::TNegate)
        (*_functions->constraint_assign_bit_unary)(&_implementation, UBOTNegate,
            result._implementation, &parameters, modification_date, constraintEnvironment);
    }
    else if (operation.isFloat()) {
      Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
      if (typeOperation >= Floating::Operation::StartOfQuery && typeOperation < Floating::Operation::EndOfQuery)
        (*_functions->constraint_assign_float_unary)(&_implementation,
            (UnaryFloatOperationType) (typeOperation-Floating::Operation::StartOfQuery),
            result._implementation, &parameters, modification_date, constraintEnvironment);
      else if (typeOperation == Floating::Operation::TOpposite)
        (*_functions->constraint_assign_float_unary)(&_implementation,
            UFOTOpposite, result._implementation, &parameters, modification_date, constraintEnvironment);
      else if (typeOperation == Floating::Operation::TAbs)
        (*_functions->constraint_assign_float_unary)(&_implementation,
            UFOTAbs, result._implementation, &parameters, modification_date, constraintEnvironment);
      else if (typeOperation >= Floating::Operation::EndOfNumeric)
        (*_functions->constraint_assign_float_unary)(&_implementation,
            (UnaryFloatOperationType) (UFOTAbs + typeOperation-Floating::Operation::EndOfNumeric), result._implementation, &parameters,
            modification_date, constraintEnvironment);
      else if (typeOperation == Floating::Operation::TCastMultiFloat) {
        const Floating::CastMultiFloatOperation& realOperation = (const Floating::CastMultiFloatOperation&) operation;
        (*_functions->constraint_assign_float_cast_multifloat)(&_implementation,
            realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
            realOperation.isUnsigned(), result._implementation, &parameters, modification_date, constraintEnvironment);
      }
      else if (typeOperation == Floating::Operation::TCastMultiBit) {
        const Floating::CastMultiBitOperation& realOperation = (const Floating::CastMultiBitOperation&) operation;
        (*_functions->constraint_assign_float_cast_multibit)(&_implementation,
            realOperation.getResultSize(), realOperation.isSigned(), false, result._implementation,
            &parameters, modification_date, constraintEnvironment);
      }
      else if (typeOperation == Floating::Operation::TCastMultiBitPointer) {
        const Floating::CastMultiBitPointerOperation& realOperation = (const Floating::CastMultiBitPointerOperation&) operation;
        (*_functions->constraint_assign_float_cast_multibit)(&_implementation,
            realOperation.getResultSize(), realOperation.isSigned(), true, result._implementation,
            &parameters, modification_date, constraintEnvironment);
      }
      else
        assert(false);
    }
    else if (operation.isMultiBit()) {
      MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
      if (typeOperation >= MultiBit::Operation::TPrevSigned && typeOperation < MultiBit::Operation::EndOfUnary)
        (*_functions->constraint_assign_multibit_unary)(&_implementation,
            (UnaryMultiBitOperationType) (typeOperation - MultiBit::Operation::TPrevSigned), result._implementation,
            &parameters, modification_date, constraintEnvironment);
      else if (typeOperation >= MultiBit::Operation::EndOfInteger) {
        const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
        (*_functions->constraint_assign_multibit_float_unary)(&_implementation,
            (UnaryMultiBitFloatOperationType) (typeOperation - MultiBit::Operation::EndOfInteger),
            realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
            realOperation.isUnsigned(), result._implementation, &parameters,
            modification_date, constraintEnvironment);
      }
      else {
        switch (typeOperation) {
          case MultiBit::Operation::TOppositeSigned:
            (*_functions->constraint_assign_multibit_unary)(&_implementation,
                UMBOTOppositeSigned, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TOppositeFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_unary)(&_implementation,
                 UMBOTOppositeFloat, realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                 realOperation.isUnsigned(), result._implementation, &parameters,
                 modification_date, constraintEnvironment);
            }
            break;
          case MultiBit::Operation::TBitNegate:
            (*_functions->constraint_assign_multibit_unary)(&_implementation,
                UMBOTBitNegate, result._implementation, &parameters, modification_date, constraintEnvironment);
           break;
          case MultiBit::Operation::TCastMultiBit:
            { const MultiBit::CastMultiBitOperation& realOperation = (const MultiBit::CastMultiBitOperation&) operation;
              (*_functions->constraint_assign_multibit_cast_multibit)(&_implementation,
                  realOperation.getResultSize(), realOperation.isSigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCastBit:
            (*_functions->constraint_assign_multibit_cast_bit)(&_implementation, result._implementation,
                &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TCastShiftBit:
            { const MultiBit::CastShiftBitOperation& realOperation = (const MultiBit::CastShiftBitOperation&) operation;
              (*_functions->constraint_assign_multibit_cast_shift_bit)(&_implementation,
                  realOperation.getShift(), result._implementation, &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCastMultiFloat:
            { const MultiBit::CastMultiFloatOperation& realOperation = (const MultiBit::CastMultiFloatOperation&) operation;
              (*_functions->constraint_assign_multibit_cast_multifloat)(&_implementation,
                  realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                  realOperation.isUnsigned(), false, result._implementation, &parameters,
                  modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCastMultiFloatPointer:
            { const MultiBit::CastMultiFloatPointerOperation& realOperation = (const MultiBit::CastMultiFloatPointerOperation&) operation;
              (*_functions->constraint_assign_multibit_cast_multifloat)(&_implementation,
                  realOperation.getSizeExponent(), realOperation.getSizeMantissa(),
                  realOperation.isUnsigned(), true, result._implementation, &parameters,
                  modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TExtendWithZero:
            { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
              (*_functions->constraint_assign_multibit_extend)(&_implementation,
                  realOperation.getExtension(), false, result._implementation, &parameters,
                  modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TExtendWithSign:
            { const MultiBit::ExtendOperation& realOperation = (const MultiBit::ExtendOperation&) operation;
              (*_functions->constraint_assign_multibit_extend)(&_implementation,
                  realOperation.getExtension(), true, result._implementation, &parameters,
                  modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TReduce:
            { const MultiBit::ReduceOperation& realOperation = (const MultiBit::ReduceOperation&) operation;
              (*_functions->constraint_assign_multibit_reduce)(&_implementation,
                  realOperation.getLowBit(), realOperation.getHighBit(), result._implementation, &parameters,
                  modification_date, constraintEnvironment);
            };
            break;
          default:
            assert(false);
        };
      };
    }
    else
      assert(false);
  };
  IntersectResult returnResult;
  returnResult._isEmpty = flags.isEmpty();
  returnResult._isStable = flags.isStable();
  return returnResult;
}

ScalarElement::IntersectResult
ScalarElement::constraint(const Operation& operation, ScalarElement& source, const ScalarElement& result, Flags& flags,
      struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraintEnvironment)
{ assert(_type == operation.getTypeArgument()
    && source._type == operation.getTypeSecondArgument()
    && result._type == operation.getTypeResult());
  { Flags::Parameters parameters = flags.asParameters();
    if (operation.isBit()) {
      Bit::Operation::Type typeOperation = ((const Bit::Operation&) operation).getType();
      (*_functions->constraint_assign_bit_binary)(&_implementation,
          (BinaryBitOperationType) (typeOperation-Bit::Operation::EndOfUnary),
          &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
    }
    else if (operation.isFloat()) {
      Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
      if (typeOperation >= Floating::Operation::EndOfCast && typeOperation < Floating::Operation::EndOfCompare)
        (*_functions->constraint_assign_float_binary)(&_implementation,
            (BinaryFloatOperationType) (typeOperation-Floating::Operation::EndOfCast),
            &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
      else if (typeOperation >= Floating::Operation::EndOfQuery && typeOperation < Floating::Operation::EndOfSecondBinary)
        (*_functions->constraint_assign_float_binary)(&_implementation,
            (BinaryFloatOperationType) (BFOTMin + typeOperation-Floating::Operation::EndOfQuery),
            &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
      else
        assert(false);
    }
    else if (operation.isMultiBit()) {
      MultiBit::Operation::Type typeOperation = ((const MultiBit::Operation&) operation).getType();
      if (typeOperation >= MultiBit::Operation::EndOfNumeric && typeOperation < MultiBit::Operation::EndOfInteger)
        (*_functions->constraint_assign_multibit_binary)(&_implementation,
            (BinaryMultiBitOperationType) (BMBOTModuloSigned + typeOperation-MultiBit::Operation::EndOfNumeric),
            &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
      else {
        switch (typeOperation) {
          case MultiBit::Operation::TConcat:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                BMBOTConcat, &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TBitSet:
            { const MultiBit::BitSetOperation& realOperation = (const MultiBit::BitSetOperation&) operation;
              (*_functions->constraint_assign_multibit_bitset)(&_implementation,
                  &source._implementation, realOperation.getLowBit(), realOperation.getHighBit(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TPlusSigned:
          case MultiBit::Operation::TPlusUnsigned:
          case MultiBit::Operation::TPlusUnsignedWithSigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTPlusSigned + (typeOperation - MultiBit::Operation::TPlusSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TPlusFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTPlusFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TMinusSigned:
          case MultiBit::Operation::TMinusUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTMinusSigned + (typeOperation - MultiBit::Operation::TMinusSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TMinusFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTMinusFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCompareLessSigned:
          case MultiBit::Operation::TCompareLessOrEqualSigned:
          case MultiBit::Operation::TCompareLessUnsigned:
          case MultiBit::Operation::TCompareLessOrEqualUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTCompareLessSigned + (typeOperation - MultiBit::Operation::TCompareLessSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TCompareLessFloat:
          case MultiBit::Operation::TCompareLessOrEqualFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  (BinaryMultiBitFloatOperationType) (BMBOTCompareLessFloat + typeOperation - MultiBit::Operation::TCompareLessFloat),
                  &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCompareEqual:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                BMBOTCompareEqual, &source._implementation, result._implementation, &parameters,
                modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TCompareEqualFloat:
          case MultiBit::Operation::TCompareDifferentFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  (BinaryMultiBitFloatOperationType) (BMBOTCompareEqualFloat + typeOperation - MultiBit::Operation::TCompareEqualFloat),
                  &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCompareDifferent:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                BMBOTCompareDifferent, &source._implementation, result._implementation,
                &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TCompareGreaterOrEqualFloat:
          case MultiBit::Operation::TCompareGreaterFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  (BinaryMultiBitFloatOperationType) (BMBOTCompareGreaterOrEqualFloat + typeOperation - MultiBit::Operation::TCompareGreaterOrEqualFloat),
                  &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TCompareGreaterOrEqualUnsigned:
          case MultiBit::Operation::TCompareGreaterUnsigned:
          case MultiBit::Operation::TCompareGreaterOrEqualSigned:
          case MultiBit::Operation::TCompareGreaterSigned:
          case MultiBit::Operation::TMinSigned:
          case MultiBit::Operation::TMinUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTCompareGreaterOrEqualSigned + (typeOperation - MultiBit::Operation::TCompareGreaterOrEqualSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TMinFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTMinFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TMaxSigned:
          case MultiBit::Operation::TMaxUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTMaxSigned + (typeOperation - MultiBit::Operation::TMaxSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TMaxFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTMaxFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TTimesSigned:
          case MultiBit::Operation::TTimesUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTTimesSigned + (typeOperation - MultiBit::Operation::TTimesSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TTimesFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTTimesFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation,
                  &parameters, modification_date, constraintEnvironment);
            };
            break;
          case MultiBit::Operation::TDivideSigned:
          case MultiBit::Operation::TDivideUnsigned:
            (*_functions->constraint_assign_multibit_binary)(&_implementation,
                (BinaryMultiBitOperationType) (BMBOTDivideSigned + (typeOperation - MultiBit::Operation::TDivideSigned)),
                &source._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
            break;
          case MultiBit::Operation::TDivideFloat:
            { const Details::TCastMultiFloatOperation<MultiBit::Operation>& realOperation = (const Details::TCastMultiFloatOperation<MultiBit::Operation>&) operation;
              (*_functions->constraint_assign_multibit_float_binary)(&_implementation,
                  BMBOTDivideFloat, &source._implementation, realOperation.getSizeExponent(),
                  realOperation.getSizeMantissa(), realOperation.isUnsigned(), result._implementation, &parameters, modification_date, constraintEnvironment);
            };
            break;
          default:
            assert(false);
        };
      };
    }
    else
      assert(false);
  };
  IntersectResult returnResult;
  returnResult._isEmpty = flags.isEmpty();
  returnResult._isStable = flags.isStable();
  return returnResult;
}

ScalarElement::IntersectResult
ScalarElement::constraint(const Operation& operation, ScalarElement& second, ScalarElement& third, const ScalarElement& result, Flags& flags,
      struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraintEnvironment)
{ assert(_type == operation.getTypeArgument()
    && second._type == operation.getTypeSecondArgument()
    && third._type == operation.getTypeThirdArgument()
    && result._type == operation.getTypeResult());
  { Flags::Parameters parameters = flags.asParameters();
    if (operation.isFloat()) {
      Floating::Operation::Type typeOperation = ((const Floating::Operation&) operation).getType();
      if (typeOperation >= Floating::Operation::StartOfTernary && typeOperation < Floating::Operation::EndOfNumeric)
        (*_functions->constraint_assign_float_ternary)(&_implementation,
            (TernaryFloatOperationType) (typeOperation-Floating::Operation::StartOfTernary),
            &second._implementation, &third._implementation, result._implementation, &parameters, modification_date, constraintEnvironment);
      else
        assert(false);
    }
    else
      assert(false);
  };
  IntersectResult returnResult;
  returnResult._isEmpty = flags.isEmpty();
  returnResult._isStable = flags.isStable();
  return returnResult;
}

ScalarElement::CompareSpecialIntegerResult&
ScalarElement::CompareSpecialIntegerResult::opposite()
{ unsigned int domain = _result;
  unsigned int inverseDomain = domain & BEqualMinint;
  unsigned int bitIndex = BEqualMinint << 1, inverseBitIndex = BEND;
  while (inverseBitIndex > BEqualMinint) {
    inverseBitIndex >>= 1;
    if (domain & bitIndex)
      inverseDomain |= inverseBitIndex;
    bitIndex <<= 1;
  };
  _result = inverseDomain;
  return *this;
}

ScalarElement::CompareSpecialIntegerResult
ScalarElement::compareSpecialInteger() const
{ ScalarElement::CompareSpecialIntegerResult result;
  result._result = (*_functions->compare_special_integer)(_implementation);
  return result;
}

ScalarElement::CompareSpecialFloatingResult
ScalarElement::compareSpecialFloating() const
{ ScalarElement::CompareSpecialFloatingResult result;
  result._result = (*_functions->compare_special_floating)(_implementation);
  return result;
}

void
MemoryFlags::retrieveParameters(struct ScalarParameters& parameters) {
  (*_functions->retrieve_scalar_parameters)(_implementation, &parameters);
}

ScalarElement
MemoryFlags::newDisjunction(int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_disjunction)(_implementation, size, &result._functions);
  result._type = TIUndefined;
  return result;
}

void
MemoryFlags::addMayDisjunction(ScalarElement& disjunction, ScalarElement& element) const
{ assert(!disjunction._type || disjunction._type == element._type);
  disjunction._implementation = (*_functions->add_may_disjunction)(_implementation,
      disjunction._implementation, element._implementation);
  disjunction._type = element._type;
  element._implementation = nullptr;
  element._functions = nullptr;
  element._type = TIUndefined;
}

void
MemoryFlags::addSureDisjunction(ScalarElement& disjunction, ScalarElement& element) const
{ assert(!disjunction._type || disjunction._type == element._type);
  disjunction._implementation = (*_functions->add_sure_disjunction)(_implementation,
      disjunction._implementation, element._implementation);
  disjunction._type = element._type;
  element._implementation = nullptr;
  element._functions = nullptr;
  element._type = TIUndefined;
}

#if 0
ScalarElement
MemoryFlags::newMultiBitConstant(unsigned low, unsigned high, int size /* <= 64 */)
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant_64)(_implementation, low, high, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
MemoryFlags::newMultiBitConstant(unsigned value, int size /* <= 32 */) const
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant_32)(_implementation, value, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}
#endif

ScalarElement
MemoryFlags::newMultiBitConstant(uint64_t* value, int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant)(_implementation, value, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
MemoryFlags::newFloatConstant(double value, int sizeExponent, int sizeMantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_constant)(_implementation, value, sizeExponent, sizeMantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
MemoryFlags::newBitConstant(bool value) const
{ ScalarElement result;
  result._implementation = (*_functions->create_bit_constant)(_implementation, value, &result._functions);
  result._type = TIBit;
  return result;
}

ScalarElement
MemoryFlags::newZeroFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_zero)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
MemoryFlags::newOneFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_one)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
MemoryFlags::newTwoFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_two)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
MemoryFlags::newByte(char value) const
{ ScalarElement result;
  result._implementation = (*_functions->create_byte)(_implementation, value, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
MemoryFlags::newTop(int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_top)(_implementation, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
MemoryFlags::newTopFloat(int sizeExponent, int sizeMantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_top_float)(_implementation, sizeExponent, sizeMantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
MemoryFlags::newTopBit() const
{ ScalarElement result;
  result._implementation = (*_functions->create_top_bit)(_implementation, &result._functions);
  result._type = TIBit;
  return result;
}

ScalarElement
MemoryFlags::newInterval(ScalarElement& min, ScalarElement& max) const
{ ScalarElement result;
  assert(min._type == max._type);
  result._implementation = (*_functions->create_interval)(_implementation, min._implementation,
      max._implementation, &result._functions);
  min._implementation = nullptr;
  min._functions = nullptr;
  min._type = TIUndefined;
  max._implementation = nullptr;
  max._functions = nullptr;
  max._type = TIUndefined;
  result._type = min._type;
  return result;
}

ScalarElement
MemoryFlags::newByteInterval(char min, char max) const
{ ScalarElement minValue, maxValue;
  minValue._implementation = (*_functions->create_byte)(_implementation, min, &minValue._functions);
  maxValue._implementation = (*_functions->create_byte)(_implementation, max, &maxValue._functions);
  ScalarElement result;
  result._implementation = (*_functions->create_interval)(_implementation, minValue._implementation,
      maxValue._implementation, &result._functions);
  minValue._implementation = nullptr;
  minValue._functions = nullptr;
  maxValue._implementation = nullptr;
  maxValue._functions = nullptr;
  result._type = TIMultiBit;
  return result;
}

bool
MemoryFlags::isConstant(const ScalarElement& element, int& size) const {
  return (*_functions->is_constant)(_implementation, element._implementation, &size);
}

bool
MemoryFlags::isBounded(const ScalarElement& element, int& size) const {
  return (*_functions->is_bounded)(_implementation, element._implementation, &size);
}

bool
MemoryFlags::isConstantDisjunction(const ScalarElement& element, int& numberOrElements, int& size) const {
  return (*_functions->is_constant_disjunction)(_implementation,
        &const_cast<ScalarElement&>(element)._implementation, &numberOrElements, &size);
}

void
MemoryFlags::retrieveConstant(const ScalarElement& element, uint64_t* result, int size) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_constant)(_implementation, element._implementation, result, size);
}

void
MemoryFlags::retrieveBounds(const ScalarElement& element, uint64_t* min, uint64_t* max, int size) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_bounds)(_implementation, element._implementation, min, max, size);
}

void
MemoryFlags::retrieveConstantDisjunction(const ScalarElement& element, uint64_t** result, int size, int numberOfElements) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_constant_disjunction)(_implementation, element._implementation, result, size, numberOfElements);
}

// void
// MemoryFlags::retrieveAddress(const VirtualInstruction& instruction, unsigned* address, int sizeAddress)
// { (*_functions->retrieve_address)(_implementation,
//     reinterpret_cast<struct _Instruction*>((VirtualInstruction*) &instruction), address, sizeAddress);
// }

void
MemoryState::setNumberOfRegisters(int number)
{ (*_functions->set_number_of_registers)(_implementation, number); }

void
MemoryState::setRegisterValue(int registerIndex, ScalarElement& value, MemoryFlags& flags)
{ (*_functions->set_register_value)(_implementation, registerIndex, value._implementation,
    flags._implementation, &flags._error);
  value._implementation = nullptr;
  value._functions = nullptr;
  value._type = TIUndefined;
}

ScalarElement
MemoryState::getRegisterValue(int registerIndex, MemoryFlags& flags, TypeInfo type) const
{ ScalarElement result;
  result._implementation = (*_functions->get_register_value)(_implementation, registerIndex,
      flags._implementation, &flags._error, &result._functions);
  result._type = type;
  return result;
}

ScalarElement
MemoryState::loadValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_value)(_implementation, indirectAddress._implementation,
      size, flags._implementation, &flags._error, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
MemoryState::loadDisjunctiveValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_disjunctive_value)(_implementation, indirectAddress._implementation,
      size, flags._implementation, &flags._error, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
MemoryState::loadMultiBitValue(ScalarElement& indirectAddress, int size, MemoryFlags& flags) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_value)(_implementation, indirectAddress._implementation,
      size, flags._implementation, &flags._error, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
MemoryState::loadFloatingValue(ScalarElement& indirectAddress, int size_exponent, int size_mantissa, MemoryFlags& flags) const
{ ScalarElement result;
  result._implementation = (*_functions->load_floating_value)(_implementation, indirectAddress._implementation,
      size_exponent, size_mantissa, flags._implementation, &flags._error, &result._functions);
  result._type = TIFloat;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

void
MemoryState::storeValue(ScalarElement& indirectAddress, ScalarElement& value, MemoryFlags& flags)
{ assert(indirectAddress._type == TIMultiBit);
  (*_functions->store_value)(_implementation, indirectAddress._implementation,
      value._implementation, flags._implementation, &flags._error);
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  value._implementation = nullptr;
  value._functions = nullptr;
  value._type = TIUndefined;
}

ScalarElement::IntersectResult
MemoryState::constraintStoreValue(ScalarElement& indirectAddress, ScalarElement& value,
    unsigned indirectRegister, MemoryFlags& flags)
{ assert(indirectAddress._type == TIMultiBit);
  (*_functions->constraint_store_value)(_implementation, &indirectAddress._implementation,
      value._implementation, indirectRegister, flags._implementation, &flags._error);
  value._implementation = nullptr;
  value._functions = nullptr;
  value._type = TIUndefined;
  ScalarElement::IntersectResult returnResult;
  returnResult._isEmpty = flags.scalarPart().isEmpty();
  returnResult._isStable = flags.scalarPart().isStable();
  return returnResult;
}

ScalarElement::IntersectResult
MemoryState::constraintAddress(ScalarElement& indirectAddress, const ScalarElement& value,
    MemoryFlags& flags)
{ assert(indirectAddress._type == TIMultiBit);
  (*_functions->constraint_address)(_implementation, &indirectAddress._implementation,
      value._implementation, flags._implementation, &flags._error);
  ScalarElement::IntersectResult returnResult;
  returnResult._isEmpty = flags.scalarPart().isEmpty();
  returnResult._isStable = flags.scalarPart().isStable();
  return returnResult;
}

ScalarElement
MemoryState::createMallocZone(ScalarElement& size, MemoryFlags& flags, int sizeAddress)
{ assert(size._type == TIMultiBit);
  ScalarElement result;
  result._implementation = (*_functions->create_malloc_zone)(_implementation, &size._implementation,
      flags._implementation, &flags._error, &result._functions, sizeAddress);
  result._type = TIMultiBit;
  size._implementation = nullptr;
  size._functions = nullptr;
  size._type = TIUndefined;
  return result;
}

void
MemoryState::freeMallocZone(ScalarElement& address, MemoryFlags& flags)
{ (*_functions->free_malloc_zone)(_implementation, &address._implementation,
      flags._implementation, &flags._error);
  address._implementation = nullptr;
  address._functions = nullptr;
  address._type = TIUndefined;
}

/* start -- only for DBA */

ScalarElement
Iteration::newDisjunction(int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_disjunction)(_implementation, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

void
Iteration::addMayDisjunction(ScalarElement& disjunction, ScalarElement& element) const
{ assert(!disjunction._type || disjunction._type == element._type);
  disjunction._implementation = (*_functions->add_may_disjunction)(_implementation,
      disjunction._implementation, element._implementation);
  disjunction._type = element._type;
  element._implementation = nullptr;
  element._functions = nullptr;
  element._type = TIUndefined;
}

void
Iteration::addSureDisjunction(ScalarElement& disjunction, ScalarElement& element) const
{ assert(!disjunction._type || disjunction._type == element._type);
  disjunction._implementation = (*_functions->add_sure_disjunction)(_implementation,
      disjunction._implementation, element._implementation);
  disjunction._type = element._type;
  element._implementation = nullptr;
  element._functions = nullptr;
  element._type = TIUndefined;
}

ScalarElement
Iteration::newZeroFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_zero)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
Iteration::newOneFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_one)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
Iteration::newTwoFloat(int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_two)(_implementation, size_exponent, size_mantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
Iteration::newByte(char value) const
{ ScalarElement result;
  result._implementation = (*_functions->create_byte)(_implementation, value, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
Iteration::newTop(int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_top)(_implementation, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
Iteration::newTopFloat(int sizeExponent, int sizeMantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_top_float)(_implementation, sizeExponent, sizeMantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
Iteration::newTopBit() const
{ ScalarElement result;
  result._implementation = (*_functions->create_top_bit)(_implementation, &result._functions);
  result._type = TIBit;
  return result;
}

ScalarElement
Iteration::newInterval(ScalarElement& min, ScalarElement& max) const
{ ScalarElement result;
  assert(min._type == max._type);
  result._implementation = (*_functions->create_interval)(_implementation, min._implementation,
      max._implementation, &result._functions);
  min._implementation = nullptr;
  min._functions = nullptr;
  min._type = TIUndefined;
  max._implementation = nullptr;
  max._functions = nullptr;
  max._type = TIUndefined;
  result._type = min._type;
  return result;
}

ScalarElement
Iteration::newByteInterval(char min, char max) const
{ ScalarElement minValue, maxValue;
  minValue._implementation = (*_functions->create_byte)(_implementation, min, &minValue._functions);
  maxValue._implementation = (*_functions->create_byte)(_implementation, max, &maxValue._functions);
  ScalarElement result;
  result._implementation = (*_functions->create_interval)(_implementation, minValue._implementation,
      maxValue._implementation, &result._functions);
  minValue._implementation = nullptr;
  minValue._functions = nullptr;
  maxValue._implementation = nullptr;
  maxValue._functions = nullptr;
  result._type = TIMultiBit;
  return result;
}

/* end -- only for DBA */

#if 0
ScalarElement
Iteration::newMultiBitConstant(unsigned low, unsigned high, int size /* <= 64 */) const
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant_64)(_implementation, low, high, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
Iteration::newMultiBitConstant(unsigned value, int size /* <= 32 */) const
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant_32)(_implementation, value, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}
#endif

ScalarElement
Iteration::newMultiBitConstant(uint64_t* value, int size) const
{ ScalarElement result;
  result._implementation = (*_functions->create_multibit_constant)(_implementation, value, size, &result._functions);
  result._type = TIMultiBit;
  return result;
}

ScalarElement
Iteration::newBitConstant(bool value) const
{ ScalarElement result;
  result._implementation = (*_functions->create_bit_constant)(_implementation, value, &result._functions);
  result._type = TIBit;
  return result;
}

ScalarElement
Iteration::newFloatConstant(double value, int sizeExponent, int sizeMantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->create_float_constant)(_implementation, value, sizeExponent, sizeMantissa, &result._functions);
  result._type = TIFloat;
  return result;
}

ScalarElement
Iteration::getRegisterValue(int registerIndex, TypeInfo type) const
{ ScalarElement result;
  result._implementation = (*_functions->get_register_value)(_implementation, registerIndex, &result._functions);
  result._type = type;
  return result;
}

ScalarElement
Iteration::loadValue(ScalarElement& indirectAddress, int size) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_value)(_implementation, indirectAddress._implementation,
      size, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
Iteration::loadDisjunctiveValue(ScalarElement& indirectAddress, int size) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_disjunctive_value)(_implementation, indirectAddress._implementation,
      size, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
Iteration::loadMultiBitValue(ScalarElement& indirectAddress, int size) const
{ ScalarElement result;
  result._implementation = (*_functions->load_multibit_value)(_implementation, indirectAddress._implementation,
      size, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

ScalarElement
Iteration::loadFloatingValue(ScalarElement& indirectAddress, int size_exponent, int size_mantissa) const
{ ScalarElement result;
  result._implementation = (*_functions->load_floating_value)(_implementation, indirectAddress._implementation,
      size_exponent, size_mantissa, &result._functions);
  result._type = TIMultiBit;
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  return result;
}

bool
Iteration::isConstant(const ScalarElement& element, int& size) const {
  return (*_functions->is_constant)(_implementation, element._implementation, &size);
}

bool
Iteration::isBounded(const ScalarElement& element, int& size) const {
  return (*_functions->is_bounded)(_implementation, element._implementation, &size);
}

bool
Iteration::isConstantDisjunction(const ScalarElement& element, int& numberOrElements, int& size) const {
  return (*_functions->is_constant_disjunction)(_implementation,
        &const_cast<ScalarElement&>(element)._implementation, &numberOrElements, &size);
}

void
Iteration::retrieveConstant(const ScalarElement& element, uint64_t* result, int size) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_constant)(_implementation, element._implementation, result, size);
}

void
Iteration::retrieveBounds(const ScalarElement& element, uint64_t* min, uint64_t* max, int size) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_bounds)(_implementation, element._implementation, min, max, size);
}

void
Iteration::retrieveConstantDisjunction(const ScalarElement& element, uint64_t** result, int size, int numberOfElements) {
  assert(element._type == TIMultiBit);
  (*_functions->retrieve_constant_disjunction)(_implementation, element._implementation, result, size, numberOfElements);
}

void
Iteration::setRegisterValue(int registerIndex, ScalarElement& value)
{ (*_functions->set_register_value)(_implementation, registerIndex, value._implementation);
  value._implementation = nullptr;
  value._functions = nullptr;
  value._type = TIUndefined;
}

void
Iteration::storeValue(ScalarElement& indirectAddress, ScalarElement& value)
{ assert(indirectAddress._type == TIMultiBit);
  (*_functions->store_value)(_implementation, indirectAddress._implementation, value._implementation);
  indirectAddress._implementation = nullptr;
  indirectAddress._functions = nullptr;
  indirectAddress._type = TIUndefined;
  value._implementation = nullptr;
  value._functions = nullptr;
  value._type = TIUndefined;
}

VirtualInstruction&
Iteration::getCurrentInstruction(uint64_t* address, int sizeAddress) const
{ struct _Instruction* result = (*_functions->get_current_instruction)(_implementation, address, sizeAddress);
  assert(result);
  return *reinterpret_cast<VirtualInstruction*>(result);
}

VirtualInstruction*
Iteration::getPreviousInstruction(uint64_t* address, int sizeAddress) const
{ struct _Instruction* result = (*_functions->get_previous_instruction)(_implementation, address, sizeAddress);
  return reinterpret_cast<VirtualInstruction*>(result);
}

} /* namespace debug */
} /* namespace forbint */
} /* namespace util */
} /* namespace unisim */

