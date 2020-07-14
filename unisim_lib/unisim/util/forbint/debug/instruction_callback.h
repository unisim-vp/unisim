/////////////////////////////////
//
// Library   : Analyzer/Assembly
// Unit      : Instructions
// File      : InstructionCallBack.h
// Copyright : CEA LIST - 2013 - 2020
//
// Description :
//   Definition of a generic C Interface to make the connection
//   between the binary static analysis and the interpretation
//   of the instructions.
//

#pragma once

#include <unisim/util/forbint/forbint.h>

#include <stddef.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct _Element;
struct _InterpretParameters;
struct _MemoryState;
struct _Iteration;
struct _ConstraintEnvironment;
struct _ModificationDate;

enum UnaryBitOperationType {
  UBOTPrev, UBOTNext, UBOTNegate
};

enum BinaryBitOperationType {
  BBOTPlus, BBOTMinus, BBOTCompareLess, BBOTCompareLessOrEqual, BBOTCompareEqual,
  BBOTCompareDifferent, BBOTCompareGreaterOrEqual, BBOTCompareGreater, BBOTMin,
  BBOTMax, BBOTOr, BBOTAnd, BBOTExclusiveOr
};

enum UnaryMultiBitOperationType {
  UMBOTPrevSigned, UMBOTPrevUnsigned, UMBOTNextSigned, UMBOTNextUnsigned,
  UMBOTOppositeSigned, UMBOTBitNegate
};

enum UnaryMultiBitFloatOperationType {
  UMBOTOppositeFloat, UMBOTAcos, UMBOTAsin, UMBOTAtan, UMBOTAtan2, UMBOTCeil,
  UMBOTCos, UMBOTCosh, UMBOTExp, UMBOTFabs, UMBOTFloor, UMBOTFmod, UMBOTFrexp,
  UMBOTLdexp, UMBOTLog, UMBOTLog10, UMBOTModf, UMBOTPow, UMBOTSin, UMBOTSinh,
  UMBOTSqrt, UMBOTTan, UMBOTTanh
};

enum BinaryMultiBitOperationType {
  BMBOTConcat, BMBOTPlusSigned, BMBOTPlusUnsigned, BMBOTPlusUnsignedWithSigned,
  BMBOTMinusSigned, BMBOTMinusUnsigned,
  BMBOTCompareLessSigned, BMBOTCompareLessOrEqualSigned, BMBOTCompareLessUnsigned,
  BMBOTCompareLessOrEqualUnsigned, BMBOTCompareEqual, BMBOTCompareDifferent,
  BMBOTCompareGreaterOrEqualUnsigned, BMBOTCompareGreaterUnsigned,
  BMBOTCompareGreaterOrEqualSigned, BMBOTCompareGreaterSigned,
  BMBOTMinSigned, BMBOTMinUnsigned, BMBOTMaxSigned, BMBOTMaxUnsigned, BMBOTTimesSigned,
  BMBOTTimesUnsigned, BMBOTDivideSigned, BMBOTDivideUnsigned, BMBOTModuloSigned,
  BMBOTModuloUnsigned, BMBOTBitOr, BMBOTBitAnd, BMBOTBitExclusiveOr, BMBOTBitNegate,
  BMBOTLeftShift, BMBOTLogicalRightShift, BMBOTArithmeticRightShift, BMBOTLeftRotate,
  BMBOTRightRotate
};

enum BinaryMultiBitFloatOperationType {
  BMBOTPlusFloat, BMBOTMinusFloat,
  BMBOTCompareLessFloat, BMBOTCompareLessOrEqualFloat, BMBOTCompareEqualFloat,
  BMBOTCompareDifferentFloat, BMBOTCompareGreaterOrEqualFloat, BMBOTCompareGreaterFloat,
  BMBOTMinFloat, BMBOTMaxFloat, BMBOTTimesFloat, BMBOTDivideFloat
};

enum UnaryFloatOperationType {
  UFOTIsInftyExponent, UFOTIsNaN, UFOTIsQNaN, UFOTIsSNaN,
  UFOTIsPositive, UFOTIsZeroExponent, UFOTIsNegative, UFOTCastMantissa,
  UFOTOpposite, UFOTAbs, UFOTAcos, UFOTAsin, UFOTAtan, UFOTAtan2, UFOTCeil,
  UFOTCos, UFOTCosh, UFOTExp, UFOTFabs, UFOTFloor, UFOTFmod, UFOTFrexp,
  UFOTLdexp, UFOTLog, UFOTLog10, UFOTModf, UFOTPow, UFOTSin, UFOTSinh,
  UFOTSqrt, UFOTTan, UFOTTanh
};

enum BinaryFloatOperationType {
  BFOTPlus, BFOTMinus, BFOTCompareLess, BFOTCompareLessOrEqual, BFOTCompareEqual,
  BFOTCompareDifferent, BFOTCompareGreaterOrEqual, BFOTCompareGreater,
  BFOTMin, BFOTMax, BFOTTimes, BFOTDivide
};

enum TernaryFloatOperationType {
  TFOTMultAdd, TFOTMultSub, TFOTNegMultAdd, TFOTNegMultSub
};

enum ZeroComparisonResult { ZCRUndefined, ZCRZero, ZCRDifferentZero };

typedef unsigned ScalarFloatingParameters;
typedef unsigned ScalarErrorParameters;
typedef unsigned ScalarEvaluationParameters;

struct ScalarParameters {
   ScalarFloatingParameters floatingPart;
   ScalarErrorParameters errorPart;
   ScalarEvaluationParameters evaluationPart;
};

enum CompareSpecialIntegerResultFlags
{ CSIRFEqualMinint=1, CSIRFEqualMinintP1=2, CSIRFLessM1=4, CSIRFEqualM1=8, CSIRFEqual0=16,
  CSIRFEqual1=32, CSIRFGreater1=64, CSIRFEqualMaxint=128, CSIRFEND=256
};

enum CompareSpecialFloatingResultFlags
{ CSFRFMInfty = 1, CSFRFMin=2, CSFRFLessM1=4, CSFRFEqualM1=8, CSFRFBetweenM1_0=16, CSFRFEqual0=32, CSFRFBetween0_1=64,
  CSFRFEqual1=128, CSFRFGreater1=256, CSFRFMax=512, CSFRFInfty=1024, CSFRFNaN=2048
};

struct _ElementFunctions {
  struct _Element* (*copy_element)(struct _Element* source);
  void (*free_element)(struct _Element* source);
  bool (*is_atomic)(struct _Element* source);
  size_t (*get_size)(struct _Element* source);
  char* (*print_element)(struct _Element* source);

  struct _Element* (*apply_assign_bit_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_cast_bit)(struct _Element* element, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_cast_shift_bit)(struct _Element* element, int shift, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_extend)(struct _Element* element, int extension, bool is_signed_extension, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_reduce)(struct _Element* element, int low_bit, int high_bit, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_bitset)(struct _Element* element, struct _Element* source, int low_bit, int high_bit, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_cast_multifloat)(struct _Element* element, int size_exponent, int size_mantissa, bool is_unsigned, bool is_pointer, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_float_cast_multifloat)(struct _Element* element, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_float_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, bool is_pointer, struct ScalarParameters* parameters);

  struct _Element* (*apply_assign_bit_unary)(struct _Element* element, enum UnaryBitOperationType operation, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_unary)(struct _Element* element, enum UnaryMultiBitOperationType operation, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_float_unary)(struct _Element* element, enum UnaryMultiBitFloatOperationType operation, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_float_unary)(struct _Element* element, enum UnaryFloatOperationType operation, struct ScalarParameters* parameters);

  struct _Element* (*apply_assign_bit_binary)(struct _Element* element, enum BinaryBitOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_binary)(struct _Element* element, enum BinaryMultiBitOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_multibit_float_binary)(struct _Element* element, enum BinaryMultiBitFloatOperationType operation, struct _Element* source, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_float_binary)(struct _Element* element, enum BinaryFloatOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_assign_float_ternary)(struct _Element* element, enum TernaryFloatOperationType operation, struct _Element* first, struct _Element* second, struct ScalarParameters* parameters);

  struct _Element* (*apply_bit_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_cast_bit)(struct _Element* element, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_cast_shift_bit)(struct _Element* element, int shift, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_extend)(struct _Element* element, int extension, bool is_signed_extension, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_reduce)(struct _Element* element, int low_bit, int high_bit, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_bitset)(struct _Element* element, struct _Element* source, int low_bit, int high_bit, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_cast_multifloat)(struct _Element* element, int size_exponent, int size_mantissa, bool is_unsigned, bool is_pointer, struct ScalarParameters* parameters);
  struct _Element* (*apply_float_cast_multifloat)(struct _Element* element, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_float_cast_multibit)(struct _Element* element, int size_cast, bool is_signed_result, bool is_pointer, struct ScalarParameters* parameters);

  struct _Element* (*apply_bit_unary)(struct _Element* element, enum UnaryBitOperationType operation, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_unary)(struct _Element* element, enum UnaryMultiBitOperationType operation, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_float_unary)(struct _Element* element, enum UnaryMultiBitFloatOperationType operation, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_float_unary)(struct _Element* element, enum UnaryFloatOperationType operation, struct ScalarParameters* parameters);

  struct _Element* (*apply_bit_binary)(struct _Element* element, enum BinaryBitOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_binary)(struct _Element* element, enum BinaryMultiBitOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_multibit_float_binary)(struct _Element* element, enum BinaryMultiBitFloatOperationType operation, struct _Element* source, int size_exponent, int size_mantissa, bool is_unsigned, struct ScalarParameters* parameters);
  struct _Element* (*apply_float_binary)(struct _Element* element, enum BinaryFloatOperationType operation, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*apply_float_ternary)(struct _Element* element, enum TernaryFloatOperationType operation, struct _Element* second, struct _Element* third, struct ScalarParameters* parameters);

  ZeroComparisonResult (*convert_boolean)(struct _Element* element);
  struct _Element* (*guard_assign)(struct _Element* element, struct _Element* thenPart, struct _Element* elsePart, struct ScalarParameters* parameters);
  struct _Element* (*guard)(struct _Element* element, struct _Element* thenPart, struct _Element* elsePart, struct ScalarParameters* parameters);

  struct _Element* (*merge_with)(struct _Element* element, struct _Element* source, struct ScalarParameters* parameters);
  void (*contain)(struct _Element* element, struct _Element* source, struct ScalarParameters* parameters);
  struct _Element* (*intersect_with)(struct _Element* element, struct _Element* source, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  unsigned (*compare_special_integer)(struct _Element* element);  /* set of CompareSpecialIntegerResultFlags */
  unsigned (*compare_special_floating)(struct _Element* element); /* set of CompareSpecialFloatingResultFlags */

  void (*constraint_assign_bit_cast_multibit)(struct _Element** element, int size_cast, bool is_signed_result, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_cast_multibit)(struct _Element** element, int size_cast, bool is_signed_result, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_cast_bit)(struct _Element** element, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_cast_shift_bit)(struct _Element** element, int shift, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_extend)(struct _Element** element, int extension, bool is_signed_extension, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_reduce)(struct _Element** element, int low_bit, int high_bit, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_bitset)(struct _Element** element, struct _Element** source, int low_bit, int high_bit, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_cast_multifloat)(struct _Element** element, int size_exponent, int size_mantissa, bool is_unsigned, bool is_pointer, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_float_cast_multifloat)(struct _Element** element, int size_exponent, int size_mantissa, bool is_unsigned, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_float_cast_multibit)(struct _Element** element, int size_cast, bool is_signed_result, bool is_pointer, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);

  void (*constraint_assign_bit_unary)(struct _Element** element, enum UnaryBitOperationType operation, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_unary)(struct _Element** element, enum UnaryMultiBitOperationType operation, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_float_unary)(struct _Element** element, enum UnaryMultiBitFloatOperationType operation, int size_exponent, int size_mantissa, bool is_unsigned, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_float_unary)(struct _Element** element, enum UnaryFloatOperationType operation, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);

  void (*constraint_assign_bit_binary)(struct _Element** element, enum BinaryBitOperationType operation, struct _Element** source, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_binary)(struct _Element** element, enum BinaryMultiBitOperationType operation, struct _Element** source, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_multibit_float_binary)(struct _Element** element, enum BinaryMultiBitFloatOperationType operation, struct _Element** source, int size_exponent, int size_mantissa, bool is_unsigned, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_float_binary)(struct _Element** element, enum BinaryFloatOperationType operation, struct _Element** source, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
  void (*constraint_assign_float_ternary)(struct _Element** element, enum TernaryFloatOperationType operation, struct _Element** first, struct _Element** second, struct _Element* result, struct ScalarParameters* parameters, struct _ModificationDate* modification_date, struct _ConstraintEnvironment* constraint_environment);
};

struct _InterpretParameters;
struct _MemoryState;
struct _Iteration;
struct _ConstraintEnvironment;
struct _ModificationDate;

enum TargetIdentification { DINone, DIThen, DIElse, DIIdentified };

struct _Instruction;
struct _InterpretParametersFunctions {
  void (*retrieve_scalar_parameters)(struct _InterpretParameters* parameters, struct ScalarParameters* result);
  struct _Element* (*create_disjunction)(struct _InterpretParameters* parameters, int size_element_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*add_may_disjunction)(struct _InterpretParameters* parameters, struct _Element* disjunction, struct _Element* element);
  struct _Element* (*add_sure_disjunction)(struct _InterpretParameters* parameters, struct _Element* disjunction, struct _Element* element);
  struct _Element* (*create_multibit_constant_64)(struct _InterpretParameters* parameters, unsigned low, unsigned high, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_multibit_constant_32)(struct _InterpretParameters* parameters, uint64_t value, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_multibit_constant)(struct _InterpretParameters* parameters, uint64_t* value, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_bit_constant)(struct _InterpretParameters* parameters, bool value, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_zero)(struct _InterpretParameters* parameters, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_one)(struct _InterpretParameters* parameters, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_two)(struct _InterpretParameters* parameters, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_constant)(struct _InterpretParameters* parameters, double value, int size_exponent_in_bits, int size_mantissa_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_byte)(struct _InterpretParameters* parameters, char value, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top)(struct _InterpretParameters* parameters, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top_float)(struct _InterpretParameters* parameters, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top_bit)(struct _InterpretParameters* parameters, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_interval)(struct _InterpretParameters* parameters, struct _Element* min, struct _Element* max, struct _ElementFunctions** elementFunctions);
  bool (*is_constant)(struct _InterpretParameters* parameters, struct _Element* element, int* result_size_in_bits);
  bool (*is_constant_disjunction)(struct _InterpretParameters* parameters, struct _Element** element, int* number_of_elements, int* result_size_in_bits);
  bool (*is_bounded)(struct _InterpretParameters* parameters, struct _Element* element, int* result_size_in_bits);
  void (*retrieve_constant)(struct _InterpretParameters* parameters, struct _Element* element, uint64_t* result, int size_in_bits);
  void (*retrieve_bounds)(struct _InterpretParameters* parameters, struct _Element* element, uint64_t* min, uint64_t* max, int size_in_bits);
  void (*retrieve_constant_disjunction)(struct _InterpretParameters* parameters, struct _Element* element, uint64_t** result, int size_in_bits, int number_of_elements);
  
  void (*set_accept_requirements)(struct _InterpretParameters* parameters, int index);
  void (*clear_requirements)(struct _InterpretParameters* parameters);
  void (*notify_system_call)(struct _InterpretParameters* parameters, char* name);
  void (*notify_special_function_call)(struct _InterpretParameters* parameters, char* name);
  struct _ModificationDate* (*get_modification_date)(struct _InterpretParameters* parameters);
  struct _ConstraintEnvironment* (*set_modification_target)(struct _InterpretParameters* parameters, uint64_t* target_address, int size_address_in_cells, struct _ModificationDate** modification_date);
  void (*clear_modification_target)(struct _InterpretParameters* parameters);
  unsigned (*get_control_origin)(struct _InterpretParameters* parameters);
  bool (*is_then_origin)(struct _InterpretParameters* parameters);
  bool (*is_else_origin)(struct _InterpretParameters* parameters);
  struct _Instruction* (*get_current_instruction)(struct _InterpretParameters* parameters, uint64_t* address, int size_address_in_cells);
  struct _Instruction* (*query_origin_instruction)(struct _InterpretParameters* parameters, uint64_t* address, int size_address_in_cells);
};

struct _MemoryStateFunctions {
  void (*set_number_of_registers)(struct _MemoryState* memory, int numbers);
  void (*set_register_value)(struct _MemoryState* memory, int registerIndex,
      struct _Element* value, struct _InterpretParameters* parameters, unsigned* error); /* set of MemoryEvaluationErrorFlags */
  struct _Element* (*get_register_value)(struct _MemoryState* memory, int registerIndex,
      struct _InterpretParameters* parameters, unsigned* error,
      struct _ElementFunctions** elementFunctions); /* set of MemoryEvaluationErrorFlags */
  struct _Element* (*load_multibit_value)(struct _MemoryState* memory, struct _Element* indirect_address,
      int size_in_bits, struct _InterpretParameters* parameters, unsigned* error,
      struct _ElementFunctions** elementFunctions);
  struct _Element* (*load_multibit_disjunctive_value)(struct _MemoryState* memory, struct _Element* indirect_address,
      int size_in_bits, struct _InterpretParameters* parameters, unsigned* error,
      struct _ElementFunctions** elementFunctions);
  struct _Element* (*load_floating_value)(struct _MemoryState* memory, struct _Element* indirect_address,
      int size_exponent, int size_mantissa, struct _InterpretParameters* parameters,
      unsigned* error, struct _ElementFunctions** elementFunctions);
  void (*store_value)(struct _MemoryState* memory, struct _Element* indirect_address,
      struct _Element* value, struct _InterpretParameters* parameters, unsigned* error);
  void (*constraint_store_value)(struct _MemoryState* memory, struct _Element** indirect_address,
      struct _Element* value, unsigned indirect_register, struct _InterpretParameters* parameters, unsigned* error);
  void (*constraint_address)(struct _MemoryState* memory, struct _Element** indirect_address,
      struct _Element* value, struct _InterpretParameters* parameters, unsigned* error);
  struct _Element* (*create_malloc_zone)(struct _MemoryState* memory, struct _Element** size_alloc_in_bytes,
      struct _InterpretParameters* parameters, unsigned* error, struct _ElementFunctions** elementFunctions,
      int size_address_in_cells);
  void (*free_malloc_zone)(struct _MemoryState* memory, struct _Element** address,
      struct _InterpretParameters* parameters, unsigned* error);
  void (*specialize_for_share)(struct _MemoryState* memory);
  int (*get_external_state)(struct _MemoryState* memory);
};

enum RequirementLevel
{  RLNoRequirement, RLExact, RLExactConstantDisjunction, RLConstantDisjunction,
   RLStore, RLJump, RLSystemCall
};

enum FamilyKind { FKSequential, FKBranch, FKJump, FKCall, FKReturn, FKBranchAndReturn };
enum FamilyJumpKind { FJKForward, FJKBackward, FJKDisjunctive, FJKComputed };
enum FamilyCompletion { FCComplete, FCIncomplete };
typedef unsigned FamilyInstruction;

struct _IterationTarget;
struct _IterationExit;
struct _IterationTargetFunctions {
  bool (*has_address)(struct _IterationTarget* target, uint64_t* address, int size_address_in_cells);
};

struct _IterationTargetCursor;
struct _IterationTargetCursorFunctions {
  void (*free_cursor)(struct _IterationTargetCursor* cursor);
  bool (*set_to_next)(struct _IterationTargetCursor* cursor);
  struct _IterationTarget* (*element_at)(struct _IterationTargetCursor* cursor,
     struct _IterationTargetFunctions** functions, bool* isFirst, bool* isThen, bool* isElse);
  struct _IterationExit* (*exit_at)(struct _IterationTargetCursor* cursor);
};

struct _IterationCalleeCursor;
struct _IterationCalleeCursorFunctions {
  void (*free_cursor)(struct _IterationCalleeCursor* cursor);
  bool (*set_to_next)(struct _IterationCalleeCursor* cursor);
  struct _IterationTarget* (*element_at)(struct _IterationCalleeCursor* cursor,
     struct _IterationTargetFunctions** functions);
};

struct _IterationFunctions {
  void (*add_target)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);
  void (*add_then_target)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);
  void (*add_else_target)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);
  void (*add_next_target)(struct _Iteration* iteration);
  void (*add_identified_target)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells, unsigned identification);
  bool (*requires_register_value)(struct _Iteration* iteration, unsigned indirect_register,
        RequirementLevel requirement_level);
  bool (*requires_register_value_part)(struct _Iteration* iteration, unsigned indirect_register,
        int low_bit, int high_bit, RequirementLevel requirement_level);
  bool (*requires_address_value)(struct _Iteration* aiteration,
        struct _Element* indirect_address, int size_in_bits, RequirementLevel requirement_level);
  void (*add_own_target)(struct _Iteration* iteration, struct _IterationTarget* target);
  void (*add_then_own_target)(struct _Iteration* iteration, struct _IterationTarget* target);
  void (*add_else_own_target)(struct _Iteration* iteration, struct _IterationTarget* target);
  void (*add_identified_own_target)(struct _Iteration* iteration, struct _IterationTarget* target, unsigned identification);
  
  struct _IterationTarget* (*get_local_target)(struct _Iteration* iteration, struct _IterationTargetFunctions** functions);
  bool (*local_advance_on_instruction)(struct _Iteration* iteration);
  bool (*advance_on_next_graph_instruction)(struct _Iteration* iteration);
  struct _IterationExit* (*advance_on_exit)(struct _Iteration* iteration);
  struct _IterationTarget* (*advance_on_exit_return)(struct _Iteration* iteration, struct _IterationExit* exit, struct _IterationTargetFunctions** functions);
  bool (*is_call_instruction)(struct _Iteration* iteration);
  bool (*advance_on_call_instruction)(struct _Iteration* iteration);
  void (*assume_accept_empty_destination)(struct _Iteration* iteration);
  struct _IterationTargetCursor* (*new_cursor_on_existing_targets)(struct _Iteration* iteration, struct _IterationTargetCursorFunctions** functions);
  struct _IterationCalleeCursor* (*new_cursor_on_existing_callees)(struct _Iteration* iteration, struct _IterationCalleeCursorFunctions** functions);
  void (*set_family_instruction)(struct _Iteration* iteration, FamilyInstruction family);

  /* void (*retrieve_current_address)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells); */
  char* (*get_function_name_at)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);

  void (*retrieve_scalar_parameters)(struct _Iteration* iteration, struct ScalarParameters* result);
  struct _Element* (*create_disjunction)(struct _Iteration* iteration, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*add_may_disjunction)(struct _Iteration* iteration, struct _Element* disjunction, struct _Element* element);
  struct _Element* (*add_sure_disjunction)(struct _Iteration* iteration, struct _Element* disjunction, struct _Element* element);
  struct _Element* (*create_multibit_constant_64)(struct _Iteration* iteration, unsigned low, unsigned high, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_multibit_constant_32)(struct _Iteration* iteration, uint64_t value, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_multibit_constant)(struct _Iteration* iteration, uint64_t* value, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_constant)(struct _Iteration* iteration, double value, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_bit_constant)(struct _Iteration* iteration, bool value, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_zero)(struct _Iteration* iteration, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_one)(struct _Iteration* iteration, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_float_two)(struct _Iteration* iteration, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_byte)(struct _Iteration* iteration, char value, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top)(struct _Iteration* iteration, int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top_float)(struct _Iteration* iteration, int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_top_bit)(struct _Iteration* iteration, struct _ElementFunctions** elementFunctions);
  struct _Element* (*create_interval)(struct _Iteration* iteration, struct _Element* min, struct _Element* max, struct _ElementFunctions** elementFunctions);

  struct _Element* (*get_register_value)(struct _Iteration* iteration, int registerIndex,
      struct _ElementFunctions** elementFunctions);
  struct _Element* (*load_multibit_value)(struct _Iteration* iteration, struct _Element* indirect_address,
      int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*load_multibit_disjunctive_value)(struct _Iteration* iteration, struct _Element* indirect_address,
      int size_in_bits, struct _ElementFunctions** elementFunctions);
  struct _Element* (*load_floating_value)(struct _Iteration* iteration, struct _Element* indirect_address,
      int size_exponent, int size_mantissa, struct _ElementFunctions** elementFunctions);
  bool (*is_constant)(struct _Iteration* iteration, struct _Element* element, int* result_size_in_bits);
  bool (*is_constant_disjunction)(struct _Iteration* iteration, struct _Element** element, int* number_of_elements, int* result_size_in_bits);
  bool (*is_bounded)(struct _Iteration* iteration, struct _Element* element, int* result_size_in_bits);
  void (*retrieve_constant)(struct _Iteration* iteration, struct _Element* element, uint64_t* result, int size_in_bits);
  void (*retrieve_bounds)(struct _Iteration* iteration, struct _Element* element, uint64_t* min, uint64_t* max, int size_in_bits);
  void (*retrieve_constant_disjunction)(struct _Iteration* iteration, struct _Element* element, uint64_t** result, int size_in_bits, int number_of_elements);

  void (*set_register_value)(struct _Iteration* iteration, int registerIndex, struct _Element* value);
  void (*store_value)(struct _Iteration* iteration, struct _Element* indirect_address, struct _Element* value);

  struct _Instruction* (*get_current_instruction)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);
  struct _Instruction* (*get_previous_instruction)(struct _Iteration* iteration, uint64_t* address, int size_address_in_cells);
  void (*notify_dynamic_targets)(struct _Iteration* iteration, struct _Element* target_element);
  void (*notify_undefined_target)(struct _Iteration* iteration);
  int (*get_external_state)(struct _Iteration* iteration);
  void (*set_external_state_for_targets)(struct _Iteration* iteration, int new_state);
};

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

