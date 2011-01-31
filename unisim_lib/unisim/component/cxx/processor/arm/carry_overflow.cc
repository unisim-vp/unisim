/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/component/cxx/processor/arm/carry_overflow.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** Compute if a carry is produced from an operation.
 *
 * @param res the result of the operation
 * @param s1 the first operand
 * @param s2 the second operand
 * @param carry_in the input carry if any
 * @return true if a carry is produced, false otherwise
 */
bool CarryFrom(const uint32_t res, const uint32_t s1, const uint32_t s2,
		const uint32_t carry_in)
{
	if ((res < s1) || (res < s2)) return true;
	return false;
}

/** Compute if a borrow is produced from an operation.
 *
 * @param res the result of the operation
 * @param s1 the first operand
 * @param s2 the second operand
 * @param carry_in the input carry if any
 * @return true if a borrow is produced, false otherwise
 */
bool BorrowFrom(const uint32_t res, const uint32_t s1, const uint32_t s2,
		const uint32_t carry_in)
{
	if (s1 > s2)
		return false;
	else 
	{
		if (s1 == s2)
			if (carry_in) return true;
			else return false;
			else return true;
	}
}

/** Compute if an overflow is produced from an addition operation.
 *
 * @param res the result of the operation
 * @param s1 the first operand
 * @param s2 the second operand
 * @param carry_in the input carry if any
 * @result true if an overflow is produced, false otherwise
 */
bool AdditionOverflowFrom(const uint32_t res, const uint32_t s1,
		const uint32_t s2,	const uint32_t carry_in)
{
	if (((((int32_t)s1 >= 0) &&
					((int32_t)s2 >= 0)) ||
				(((int32_t)s1 < 0) &&
				 ((int32_t)s2 < 0))) &&
			(((((int32_t)res >= 0) &&
			   ((int32_t)s1 < 0)) ||
			  (((int32_t)res < 0) &&
			   ((int32_t)s1 >= 0))))) 
		return true;
	return false;
}

/** Compute if an overflow is produced from a subtraction operation.
 *
 * @param res the result of the operation
 * @param s1 the first operand
 * @param s2 the second operand
 * @param carry_in the input carry if any
 * @result true if an overflow is produced, false otherwise
 */
bool SubtractionOverflowFrom(const uint32_t res, const uint32_t s1,
		const uint32_t s2, const uint32_t carry_in)
{
	if ((((((int32_t)s1) < 0) &&
					(((int32_t)s2) >= 0)) ||
				((((int32_t)s1) >= 0) &&
				 (((int32_t)s2) < 0))) &&
			((((((int32_t)res) >= 0) &&
			   (((int32_t)s1) < 0)) ||
			  ((((int32_t)res) < 0) &&
			   (((int32_t)s1) >= 0))))) 
		return true;
	return false;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
