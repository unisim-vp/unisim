/*
 * data_processing.hh
 *
 *  Created on: Sep 7, 2010
 *      Author: gracia
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DATA_PROCESSING_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DATA_PROCESSING_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

// static inline uint32_t ShiftOperand32imm(const uint32_t rotate_imm,
//								  const uint32_t imm) GCC_INLINE;
uint32_t ShiftOperand32imm(const uint32_t rotate_imm,
		const uint32_t imm);


// static inline uint32_t ShiftOperand32imm(const uint32_t rotate_imm,
//							   const uint32_t imm,
//							   bool *shift_carry_out) GCC_INLINE;
uint32_t ShiftOperand32imm(const uint32_t rotate_imm,
		const uint32_t imm,
		const bool carry_in,
		bool *shift_carry_out);


// inline reg_t ShiftOperandImmShift(const uint32_t shift_imm,
// 								  const uint32_t shift,
// 								  const reg_t val_reg) GCC_INLINE;
uint32_t ShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t val_reg,
		const bool carry_in);

//inline reg_t ShiftOperandImmShift(const uint32_t shift_imm,
//								  const uint32_t shift,
//								  const reg_t val_reg,
//								  bool *shift_carry_out) GCC_INLINE;
uint32_t ShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t val_reg,
		const bool carry_in,
		bool *shift_carry_out);

//static inline reg_t ShiftOperandRegShift(const uint32_t shift_reg,
//										 const uint32_t shift,
//										 const reg_t val_reg) GCC_INLINE;
uint32_t ShiftOperandRegShift(const uint32_t shift_reg,
		const uint32_t shift,
		const uint32_t val_reg);

//inline reg_t ShiftOperandRegShift(const uint32_t shift_reg,
//								  const uint32_t shift,
//								  const reg_t val_reg,
//								  bool *shift_carry_out) GCC_INLINE;
uint32_t ShiftOperandRegShift(const uint32_t shift_reg,
		const uint32_t shift,
		const uint32_t val_reg,
		const bool carry_in,
		bool *shift_carry_out);

}
}
}
}
}

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DATA_PROCESSING_HH__ */
