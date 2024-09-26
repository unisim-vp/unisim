/*
 *  Copyright (c) 2012,
 *  Commissariat a l'Energie Atomique (CEA)
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/data_object.hh>
#include <unisim/util/debug/dwarf/c_loc_expr_parser.hh>
#include <inttypes.h>
#include <vector>
#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

class DWARF_BitVector;

std::ostream& operator << (std::ostream& os, const DWARF_BitVector& bv);

class DWARF_BitVector
{
public:
	DWARF_BitVector(unisim::util::endian::endian_type target_endian);
	~DWARF_BitVector();
	
	void Clear();
	
	// Notes:
	//   - for buffer flavor of Append/Write/Read, bit offset passed as argument is offset of high order bit for big-endian target and low order bit for little-endian target (same convention as DWARF bit offsets), so that bit offset keeps increasing with byte address
	//   - for scalar value flavor of Append/Write/Read, bit offset passed as argument is always low order bit (a DWARF offset has to be converted before)
	void Append(uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size); // scalar
	void Append(const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size); // buffer
	bool Write(unsigned int dest_bit_offset, uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size); // scalar
	bool Write(unsigned int dest_bit_offset, const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size); // buffer
	bool Read(unsigned int source_bit_offset, uint64_t& dest_value, unsigned int dest_bit_offset, unsigned int dest_bit_size) const; // scalar
	bool Read(unsigned int source_bit_offset, uint8_t *dest_buffer, unsigned int dest_bit_offset, unsigned int dest_bit_size) const; // buffer
private:
	unisim::util::endian::endian_type target_endian;
	unsigned int bit_size;
	std::vector<uint8_t> storage;
	friend std::ostream& operator << (std::ostream& os, const DWARF_BitVector& bv);
};

template <class MEMORY_ADDR>
class DWARF_DataObject
{
public:
	DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *dw_handler, const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const char *data_object_name, const CLocOperationStream *c_loc_operation_stream, const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc, const DWARF_DIE<MEMORY_ADDR> *dw_die_type, unsigned int dim, const unisim::util::debug::Type *type);
	virtual ~DWARF_DataObject();
	const char *GetName() const;
	unsigned int GetProcessorNumber() const;
	MEMORY_ADDR GetBitSize() const;
	unisim::util::endian::endian_type GetEndian() const;
	const unisim::util::debug::Type *GetType() const;
	bool IsOptimizedOut() const;
	bool GetAddress(MEMORY_ADDR& addr) const;
	bool Read(MEMORY_ADDR obj_bit_offset, uint64_t& value, MEMORY_ADDR bit_size) const;
	bool Write(MEMORY_ADDR obj_bit_offset, uint64_t value, MEMORY_ADDR bit_size);
	bool Read(MEMORY_ADDR obj_bit_offset, void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size) const;
	bool Write(MEMORY_ADDR obj_bit_offset, const void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size);
	bool Seek() const;
	bool Fetch() const;
	bool Commit();

	template <typename T> T ToBaseType() const;
	std::string ToString() const;
	template <typename T> DWARF_DataObject<MEMORY_ADDR>& AssignBaseType(T value);
	DWARF_DataObject<MEMORY_ADDR>& AssignString(const std::string& value);
	DWARF_DataObject<MEMORY_ADDR> *Dereference();
	DWARF_DataObject<MEMORY_ADDR> *GetProperty(const char *property_name);
	DWARF_DataObject<MEMORY_ADDR> *GetItem(int64_t subscript);
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state;
	unsigned int prc_num;
	std::string data_object_name;
	const CLocOperationStream *c_loc_operation_stream;
	mutable const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc;
	mutable const DWARF_DIE<MEMORY_ADDR> *dw_die_type;
	mutable unsigned int dim;
	mutable const unisim::util::debug::Type *dw_data_object_type;
	unisim::util::endian::endian_type arch_endianness;
	unsigned int arch_address_size;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	mutable DWARF_BitVector bv;
	bool hold;
	const bool& debug;
};

template <class ADDRESS>
class DWARF_DataObjectProxy : public unisim::util::debug::DataObject<ADDRESS>
{
public:
	DWARF_DataObjectProxy(DWARF_DataObject<ADDRESS> *dw_data_object);
	virtual ~DWARF_DataObjectProxy();
	virtual const char *GetName() const;
	virtual unsigned int GetProcessorNumber() const;
	virtual ADDRESS GetBitSize() const;
	virtual const Type *GetType() const;
	virtual unisim::util::endian::endian_type GetEndian() const;
	virtual bool Exists() const;
	virtual bool IsOptimizedOut() const;
	virtual bool GetAddress(ADDRESS& addr) const;
	virtual bool Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const;
	virtual bool Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size);
	virtual bool Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const;
	virtual bool Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size);
	
	template <typename T> T ToBaseType() const;
	std::string ToString() const;
	template <typename T> DWARF_DataObjectProxy<ADDRESS>& AssignBaseType(T value);
	DWARF_DataObjectProxy<ADDRESS>& AssignString(const std::string& value);
	
	// conversion operators
	virtual operator bool() const;
	virtual operator char() const;
	virtual operator signed char() const;
	virtual operator short() const;
	virtual operator int() const;
	virtual operator long() const;
	virtual operator long long() const;
	virtual operator unsigned char() const;
	virtual operator unsigned short() const;
	virtual operator unsigned int() const;
	virtual operator unsigned long() const;
	virtual operator unsigned long long() const;
	virtual operator float() const;
	virtual operator double() const;
	virtual operator std::string() const;
	
	// assignment operators
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (bool value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (signed char value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (short value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (int value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (long value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (long long value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (unsigned char value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (unsigned short value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (unsigned int value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (unsigned long value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (unsigned long long value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (float value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (double value);
	virtual unisim::util::debug::DataObject<ADDRESS>& operator = (const std::string& value);

	// dereferencing operator
	virtual unisim::util::debug::DataObjectRef<ADDRESS> operator * ();
	virtual const unisim::util::debug::DataObjectRef<ADDRESS> operator * () const;
	
	// object property accessors
	virtual unisim::util::debug::DataObjectRef<ADDRESS> operator [] (const char *property_name);
	virtual const unisim::util::debug::DataObjectRef<ADDRESS> operator [] (const char *property_name) const;

	// array item accessors
	virtual unisim::util::debug::DataObjectRef<ADDRESS> operator [] (int64_t subscript);
	virtual const unisim::util::debug::DataObjectRef<ADDRESS> operator [] (int64_t subscript) const;
	
private:
	DWARF_DataObject<ADDRESS> *dw_data_object;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__
