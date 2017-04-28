/*
 *  Copyright (c) 2017,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

/* Example of use:
 * 
 * struct MR : Register<MR, 32, SW_RW>   // 32-bit register, hardware read-write/software read-write access
 * {
 *     typedef Register<MR, 32> Super;
 * 
 *     struct A : Field<31, 2, SW_W> {}; // field [31..2], hardware write-only/software write-only access
 *     struct B : Field<26> {};          // field [26], implicitely one bit length field, implicitely hardware read-write/software read-write access
 *     struct C : Field<11, 3> {};       // field [11..3], implicitely hardware read-write/software read-write
 *     struct E : Field<7, 2, HW_R> {};  // field [7..2], hardware read-only/no software access 
 *     struct D : Field<0, 5, SW_R> {};  // field [5..0], hardware read-only/software read-only access
 * 
 *     typedef FieldSet<A, B, C, D> ALL; // field order is not important but determine pretty print order
 * 
 *     MR(uint32_t _value) : Super(_value) {}
 *     using Super::operator =;
 * };
 * 
 * // construction
 * MR mr(0x12345678);
 * 
 * // initialization
 * mr.Initialize(0x12345678);
 * 
 * // hardware write of a register field
 * mr.Set<MR::B>(1);
 * 
 * // hardware read of a register field
 * unsigned int a_value = mr.Get<MR::A>();
 * 
 * // hardware read of register value
 * MR::TYPE mr_val = mr;
 * 
 * // hardware write register value
 * mr = 0x0;
 * 
 * // software write of register value
 * WarningStatus ws_write = mr.Write(0xffffffff);
 * if(ws_write)
 * {
 *     std::cerr << ws_write << std::endl;
 * }
 * 
 * // software read of register value
 * uint32_t v;
 * WarningStatus ws_read = mr.Read(v);
 * if(ws_read)
 * {
 *     std::cerr << ws_read << std::endl;
 * }
 * 
 * // print register value
 * std::cout << mr << std::endl;
 * 
 * // short pretty print of register content
 * mr.ShortPrettyPrint(std::cout); std::cout << std::endl;
 * 
 * // long pretty print of register content
 * mr.LongPrettyPrint(std::cout); std::cout << std::endl;
 */

#ifndef __UNISIM_UTIL_REG_CORE_REGISTER_HH__
#define __UNISIM_UTIL_REG_CORE_REGISTER_HH__

#include <unisim/util/inlining/inlining.hh>
#include <unisim/service/interfaces/register.hh>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <typeinfo>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// Access ////////////////////////////////////

enum Access
{
	HW_RO = 0,                    // hardware read-only/no software access
	HW_RW = 1,                    // hardware read-write/no software access
	SW_R = 2 | HW_RW,             // hardware read-write/software read access
	SW_W = 4 | HW_RW,             // hardware read-write/software write access
	SW_RW = SW_R | SW_W,          // hardware read-write/software read-write access (default)
	SW_W1 = 8 | SW_W,             // hardware write/software writeOnce access
	SW_RW1 = SW_RW | SW_W1,       // hardware read-write/software read-writeOnce access
};

std::ostream& operator << (std::ostream& os, const Access& access);

//////////////////////////////// WarningStatus ////////////////////////////////

enum WarningStatus
{
	WS_OK        = 0,                 // OK
	WS_WOORV     = 1,                 // writing out-of-range value
	WS_WROR      = 2,                 // writing read-only register
	WS_WROB      = 4,                 // writing read-only bits
	WS_RWOR      = 8,                 // reading write-only register
	WS_RWOB      = 16,                // reading write-only bits
	WS_WOORV_WROR = WS_WOORV | WS_WROR, // writing out-of-range value and writing read-only register
	WS_WOORV_WROB = WS_WOORV | WS_WROB, // writing out-of-range value and writing read-only bits
};

std::ostream& operator << (std::ostream& os, const WarningStatus& ws);

////////////////////////// Forward declarations ///////////////////////////////

class PropertyRegistry;

class LongPrettyPrinter;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS> class Field;

struct NullField;

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63> struct FieldSet;

template <typename ADDRESS> class RegisterAddressMap;

template <unsigned int BYTE_SIZE> struct TypeForByteSize;

template <unsigned int SIZE> struct TypeForBitSize;

struct NullRegisterBase;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE> class Register;

class AddressableRegisterBase;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS> class AddressableRegister;

template <typename ADDRESS> class AddressableRegisterHandle;

template <typename ADDRESS> class RegisterAddressMap;

///////////////////////////// PropertyRegistry ////////////////////////////////

class PropertyRegistry
{
public:
	enum ElementType
	{
		EL_REGISTER = 0,
		EL_FIELD    = 1
	};
	
	enum StringPropertyType
	{
		STR_PROP_NAME      = 0,
		STR_PROP_DISP_NAME = 1,
		STR_PROP_DESC      = 2
	};

	static void SetStringProperty(ElementType el_type, StringPropertyType prop_type, intptr_t key, const std::string& value);
	static const std::string& GetStringProperty(ElementType el_type, StringPropertyType prop_type, intptr_t key);
	
private:
	static std::map<intptr_t, std::string> string_prop_registry[2][3];
	
};

///////////////////////////// LongPrettyPrinter ///////////////////////////////

class LongPrettyPrinter
{
public:
	static const unsigned int COLUMN_SPACING_PRINT_WIDTH = 2;
	static const unsigned int KIND_PRINT_WIDTH           = 8;
	static const unsigned int RANGE_PRINT_WIDTH          = 8;
	static const unsigned int NAME_PRINT_WIDTH           = 16;
	static const unsigned int VALUE_PRINT_WIDTH          = 23; // for at most 64-bit numbers in octal format
	static const unsigned int ACCESS_PRINT_WIDTH         = 46;
	
	static void Print(std::ostream& os, const std::string& s, unsigned int max_length);
	static void PrintHeader(std::ostream& os);
	static void PrintKind(std::ostream& os, const std::string& kind);
	static void PrintRegisterSize(std::ostream& os, unsigned int register_size);
	static void PrintFieldRange(std::ostream& os, unsigned int bitoffset, unsigned int bitwidth);
	static void PrintDisplayName(std::ostream& os, const std::string& display_name);
	template <typename T> static void PrintValue(std::ostream& os, const T& value);
	static void PrintAccess(std::ostream& os, const Access& access);
	static void PrintDescription(std::ostream& os, const std::string& s);
};

//////////////////////////////// FieldID ///////////////////////////////////

unsigned int FieldID();

////////////////////////////////// Field<> ////////////////////////////////////


template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH = 1, Access _ACCESS = SW_RW>
class Field
{
public:
	static const unsigned int BITOFFSET = _BITOFFSET;
	static const unsigned int BITWIDTH = _BITWIDTH;
	static const Access ACCESS = _ACCESS;
	static const unsigned int BITWIDTH_MINUS_1 = _BITWIDTH - 1;
	
	static const unsigned int ID;

	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	template <typename T> static inline T Mask(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline T Get(const T& storage) ALWAYS_INLINE;
	template <typename T> static void inline Set(T& storage, const T& field_value) ALWAYS_INLINE;
	static inline void SetName(const std::string& name);
	static inline void SetDisplayName(const std::string& disp_name);
	static inline void SetDescription(const std::string& desc);
	static inline unsigned int GetBitOffset();
	static inline unsigned int GetBitWidth();
	static inline Access GetAccess();
	static inline bool IsReadable();
	static inline bool IsWritable();
	static inline bool IsWriteOnce();
	static inline bool IsReadOnly();
	static inline bool IsWriteOnly();
	static inline bool IsWriteOnlyAndOnce();
	static inline const std::string& GetName();
	static inline const std::string& GetDisplayName();
	static inline const std::string& GetDescription();
	template <typename T> static inline void ShortPrettyPrint(std::ostream& os, const T& storage);
	template <typename T> static inline void LongPrettyPrint(std::ostream& os, const T& storage);
private:
	static inline const std::string& GetNameKey();
	static inline const std::string& GetDisplayNameKey();
	static inline const std::string& GetDescriptionKey();
};

/////////////////////////////// NullField //////////////////////////////////

struct NullField
{
	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	static void SetName(const std::string& name);
	static void SetDisplayName(const std::string& disp_name);
	static void SetDescription(const std::string& desc);
	static const std::string& GetName();
	template <typename T> static void ShortPrettyPrint(std::ostream& os, const T& storage);
	template <typename T> static void LongPrettyPrint(std::ostream& os, const T& storage);
};

/////////////////////////////// FieldSet<> /////////////////////////////////

template < typename  BF0               , typename  BF1 = NullField, typename  BF2 = NullField, typename  BF3 = NullField
         , typename  BF4 = NullField, typename  BF5 = NullField, typename  BF6 = NullField, typename  BF7 = NullField
         , typename  BF8 = NullField, typename  BF9 = NullField, typename BF10 = NullField, typename BF11 = NullField
         , typename BF12 = NullField, typename BF13 = NullField, typename BF14 = NullField, typename BF15 = NullField
         , typename BF16 = NullField, typename BF17 = NullField, typename BF18 = NullField, typename BF19 = NullField
         , typename BF20 = NullField, typename BF21 = NullField, typename BF22 = NullField, typename BF23 = NullField
         , typename BF24 = NullField, typename BF25 = NullField, typename BF26 = NullField, typename BF27 = NullField
         , typename BF28 = NullField, typename BF29 = NullField, typename BF30 = NullField, typename BF31 = NullField
         , typename BF32 = NullField, typename BF33 = NullField, typename BF34 = NullField, typename BF35 = NullField
         , typename BF36 = NullField, typename BF37 = NullField, typename BF38 = NullField, typename BF39 = NullField
         , typename BF40 = NullField, typename BF41 = NullField, typename BF42 = NullField, typename BF43 = NullField
         , typename BF44 = NullField, typename BF45 = NullField, typename BF46 = NullField, typename BF47 = NullField
         , typename BF48 = NullField, typename BF49 = NullField, typename BF50 = NullField, typename BF51 = NullField
         , typename BF52 = NullField, typename BF53 = NullField, typename BF54 = NullField, typename BF55 = NullField
         , typename BF56 = NullField, typename BF57 = NullField, typename BF58 = NullField, typename BF59 = NullField
         , typename BF60 = NullField, typename BF61 = NullField, typename BF62 = NullField, typename BF63 = NullField>
struct FieldSet
{
	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	template <typename T> static inline T Get(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline void Set(T& storage, const T& bitfied_value) ALWAYS_INLINE;
	static inline const std::string& GetName();
	template <typename T> static inline void ShortPrettyPrint(std::ostream& os, const T& storage);
	template <typename T> static inline void LongPrettyPrint(std::ostream& os, const T& storage);
};

///////////////////////////// TypeForByteSize<> ///////////////////////////////

template <unsigned int BYTE_SIZE> struct TypeForByteSize {};
template <> struct TypeForByteSize<1> { typedef uint8_t TYPE; };
template <> struct TypeForByteSize<2> { typedef uint16_t TYPE; };
template <> struct TypeForByteSize<3> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<4> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<5> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<6> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<7> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<8> { typedef uint64_t TYPE; };

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> struct TypeForBitSize
{
	static const unsigned int BYTE_SIZE = (SIZE + 7) / 8;
	typedef typename TypeForByteSize<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (SIZE == (8 * BYTE_SIZE)) ? (~TYPE(0)) : ((TYPE(1) << SIZE) - 1);
};

////////////////////////////// NullRegisterBase ///////////////////////////////

struct NullRegisterBase {};

////////////////////////////////// Register<> /////////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
std::ostream& operator << (std::ostream& os, const Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>& reg);

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS = SW_RW, typename REGISTER_BASE = NullRegisterBase>
class Register : public REGISTER_BASE
{
public:
	typedef typename TypeForBitSize<_SIZE>::TYPE TYPE;
	static const TYPE TYPE_MASK = TypeForBitSize<_SIZE>::MASK;
	static const unsigned int SIZE = _SIZE;
	static const Access ACCESS = _ACCESS;
	
	inline TYPE Get(unsigned int bit_offset) const ALWAYS_INLINE;
	inline void Set(unsigned int bit_offset, TYPE bit_value) ALWAYS_INLINE;
	template <typename FIELD> inline TYPE Get() const ALWAYS_INLINE;
	template <typename FIELD> inline void Set(TYPE field_value) ALWAYS_INLINE;
	
	inline TYPE GetMask() const ALWAYS_INLINE;
	inline TYPE GetAssignMask() const ALWAYS_INLINE;
	inline TYPE GetWriteMask() const ALWAYS_INLINE;
	inline TYPE GetReadMask() const ALWAYS_INLINE;
	
	Register();
	Register(TYPE value);
	void Initialize(TYPE value);
	inline Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>& operator = (const TYPE& value) ALWAYS_INLINE;
	inline operator TYPE () const ALWAYS_INLINE;
	inline TYPE operator [] (unsigned int bit_offset) const ALWAYS_INLINE;
	WarningStatus Write(const TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	WarningStatus Read(TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	void DebugWrite(const TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	void DebugRead(TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	WarningStatus Write(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0);
	WarningStatus Read(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0);
	void DebugWrite(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0);
	void DebugRead(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0);
	void ShortPrettyPrint(std::ostream& os) const;
	void LongPrettyPrint(std::ostream& os) const;
	
	friend std::ostream& operator << <REGISTER, _SIZE>(std::ostream& os, const Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>& reg);
	
	void SetName(const std::string& name);
	void SetDisplayName(const std::string& disp_name);
	void SetDescription(const std::string& desc);
	unsigned int GetSize() const;
	Access GetAccess() const;
	bool IsReadable() const;
	bool IsWritable() const;
	bool IsWriteOnce() const;
	bool IsReadOnly() const;
	bool IsWriteOnly() const;
	bool IsWriteOnlyAndOnce() const;
	const std::string& GetName() const;
	const std::string& GetDisplayName() const;
	const std::string& GetDescription() const;
	
	unisim::service::interfaces::Register *CreateRegisterInterface();
private:
	TYPE value;
	const std::string GetNameKey() const;
	const std::string GetDisplayNameKey() const;
	const std::string GetDescriptionKey() const;
};

/////////////////////////// NullRegisterFileBase //////////////////////////////

struct NullRegisterFileBase {};

/////////////////////////////// RegisterFile<> ////////////////////////////////

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE = NullRegisterFileBase>
class RegisterFile
{
public:
	RegisterFile();
	RegisterFile(typename REGISTER::TYPE value);
	inline REGISTER& operator [] (int index) ALWAYS_INLINE;
	inline const REGISTER& operator [] (int index) const ALWAYS_INLINE;
	
	inline unsigned int GetDim() const ALWAYS_INLINE;
	
	void SetName(const std::string& name);
	void SetDisplayName(const std::string& disp_name);
	void SetDescription(const std::string& desc);
	
	const std::string& GetName() const;
	const std::string& GetDisplayName() const;
	const std::string& GetDescription() const;
private:
	REGISTER regs[_DIM];
	
	const std::string& GetNameKey() const;
	const std::string& GetDisplayNameKey() const;
	const std::string& GetDescriptionKey() const;
};

////////////////////////// AddressableRegisterBase ////////////////////////////

// Note: this class is internal, it is not intended to be used directly by the user
//       use AddressableRegister instead
class AddressableRegisterBase
{
protected:
	virtual unsigned int __ARB_GetSize__() const = 0;
	virtual WarningStatus __ARB_Write__(unsigned char *data_ptr, unsigned char *bit_enable_ptr) = 0;
	virtual WarningStatus __ARB_Read__(unsigned char *data_ptr, unsigned char *bit_enable_ptr) = 0;
	virtual void __ARB_DebugWrite__(unsigned char *data_ptr, unsigned char *bit_enable_ptr) = 0;
	virtual void __ARB_DebugRead__(unsigned char *data_ptr, unsigned char *bit_enable_ptr) = 0;
	
	template <typename ADDRESS> friend class AddressableRegisterHandle;
	template <typename ADDRESS> friend class RegisterAddressMap;
};

/////////////////////////// AddressableRegister<> /////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS = SW_RW>
class AddressableRegister : public Register<REGISTER, _SIZE, _ACCESS, AddressableRegisterBase>
{
public:
	typedef Register<REGISTER, _SIZE, _ACCESS, AddressableRegisterBase> Super;
	
	AddressableRegister();
	AddressableRegister(typename Super::TYPE value);

	using Super::operator =;
	
private:
	virtual unsigned int __ARB_GetSize__() const;
	virtual WarningStatus __ARB_Write__(unsigned char *data_ptr, unsigned char *bit_enable_ptr);
	virtual WarningStatus __ARB_Read__(unsigned char *data_ptr, unsigned char *bit_enable_ptr);
	virtual void __ARB_DebugWrite__(unsigned char *data_ptr, unsigned char *bit_enable_ptr);
	virtual void __ARB_DebugRead__(unsigned char *data_ptr, unsigned char *bit_enable_ptr);
};

////////////////////////// AddressableRegisterHandle<> ////////////////////////

template <typename ADDRESS>
class AddressableRegisterHandle
{
public:
	AddressableRegisterHandle(ADDRESS addr, unsigned int size, AddressableRegisterBase *arb);
	
	ADDRESS GetAddress() const { return addr; }
	unsigned int GetSize() const { return size; }
	bool Write(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0) { return arb->__ARB_Write__(data_ptr, bit_enable_ptr); }
	bool Read(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_Read__(data_ptr, bit_enable_ptr); }
	void DebugWrite(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0) { return arb->__ARB_DebugWrite__(data_ptr, bit_enable_ptr); }
	void DebugRead(unsigned char *data_ptr, unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_DebugRead__(data_ptr, bit_enable_ptr); }
private:
	friend class RegisterAddressMap<ADDRESS>;
	
	ADDRESS addr;
	unsigned int size; // in bytes (with padding)
	AddressableRegisterBase *arb;
	unsigned int ref_count;
	
	void Acquire();
	void Release();
};

///////////////////////////// RegisterAddressMap<> ////////////////////////////

template <typename ADDRESS>
class RegisterAddressMap
{
public:
	RegisterAddressMap();
	virtual ~RegisterAddressMap();
	void MapRegister(ADDRESS addr, AddressableRegisterBase *r, unsigned int size = 0 /* in bytes (with padding) */);
	AddressableRegisterHandle<ADDRESS> *FindAddressableRegister(ADDRESS addr) const;
private:
	mutable bool optimized;
	mutable bool optimizable;
	mutable std::pair<ADDRESS, ADDRESS> addr_range;
	mutable std::vector<AddressableRegisterHandle<ADDRESS> *> opt_reg_addr_map;
	std::map<ADDRESS, AddressableRegisterHandle<ADDRESS> *> reg_addr_map;

	void Optimize() const;
};

} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_CORE_REGISTER_HH__
