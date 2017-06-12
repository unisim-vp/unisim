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

#ifndef __UNISIM_UTIL_REG_CORE_REGISTER_TCC__
#define __UNISIM_UTIL_REG_CORE_REGISTER_TCC__

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> const unsigned int TypeForBitSize<SIZE>::BYTE_SIZE;
template <unsigned int SIZE> const typename TypeForBitSize<SIZE>::TYPE TypeForBitSize<SIZE>::MASK;

///////////////////////////// LongPrettyPrinter ///////////////////////////////

template <typename T>
void LongPrettyPrinter::PrintValue(std::ostream& os, const T& value)
{
	std::ios_base::fmtflags ff = os.flags();
	std::stringstream sstr_value;

	if((ff & os.hex) && (value >= 16)) sstr_value << "0x";
	else if((ff & os.oct) && (value >= 8)) sstr_value << "0";
	sstr_value.flags(ff);
	sstr_value << value;

	std::string str_value(sstr_value.str());

	Print(os, str_value, VALUE_PRINT_WIDTH);
}

/////////////////////////////////// Field /////////////////////////////////////

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITOFFSET;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITWIDTH;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const Access Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ACCESS;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITWIDTH_MINUS_1;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ID = FieldID();

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetMask()
{
	return ((T(2) << (BITWIDTH_MINUS_1)) - T(1)) << _BITOFFSET;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetAssignMask()
{
	return (_ACCESS & HW_RW) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetWriteMask()
{
	return (_ACCESS & SW_W) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetReadMask()
{
	return (_ACCESS & SW_R) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Mask(const T& storage)
{
	return storage & GetMask<T>();
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Get(const T& storage)
{
	return (storage & GetMask<T>()) >> _BITOFFSET;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Set(T& storage, T bitfied_value)
{
	storage = (storage & ~GetAssignMask<T>()) | ((bitfied_value << _BITOFFSET) & GetAssignMask<T>());
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_NAME, ID, name);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DISP_NAME, ID, disp_name);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DESC, ID, desc);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetBitOffset()
{
	return _BITOFFSET;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetBitWidth()
{
	return _BITWIDTH;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline Access Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetAccess()
{
	return _ACCESS;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsReadable()
{
	return _ACCESS & SW_R;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWritable()
{
	return _ACCESS & SW_W;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnce()
{
	return _ACCESS & SW_W1;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsReadOnly()
{
	return (_ACCESS & SW_RW1) == SW_R;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnly()
{
	return (_ACCESS & SW_RW1) == SW_W;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnlyAndOnce()
{
	return (_ACCESS & SW_RW1) == SW_W1;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetName()
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_NAME, ID);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetDisplayName()
{
	const std::string& display_name = PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DISP_NAME, ID);
	if(!display_name.empty()) return display_name;
	return GetName();
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetDescription()
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DESC, ID);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ShortPrettyPrint(std::ostream& os, const T& storage)
{
	std::ios_base::fmtflags ff = os.flags();
	os << GetDisplayName();
	os << '[';
	os << std::dec;
	if(_BITWIDTH != 1)
	{
		os << (_BITOFFSET + BITWIDTH_MINUS_1);
		os << "..";
	}
	os << _BITOFFSET;
	os << ']';
	os << '=';
	if((ff & os.hex) && (_BITWIDTH >= 16)) os << "0x";
	else if((ff & os.oct) && (_BITWIDTH >= 8)) os << "0";
	os.flags(ff);
	os << Get<T>(storage);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::LongPrettyPrint(std::ostream& os, const T& storage)
{
	LongPrettyPrinter::PrintKind(os, "field");
	LongPrettyPrinter::PrintFieldRange(os, _BITOFFSET, _BITWIDTH);
	LongPrettyPrinter::PrintDisplayName(os, GetDisplayName());
	LongPrettyPrinter::PrintValue(os, Get<T>(storage));
	LongPrettyPrinter::PrintAccess(os, _ACCESS);
	LongPrettyPrinter::PrintDescription(os, GetDescription());
}

/////////////////////////////// NullField //////////////////////////////////

template <typename T>
inline T NullField::GetMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetAssignMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetWriteMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetReadMask()
{
	return 0;
}

template <typename T>
inline void NullField::ShortPrettyPrint(std::ostream& os, const T& storage)
{
}

template <typename T>
inline void NullField::LongPrettyPrint(std::ostream& os, const T& storage)
{
}

//////////////////////////////// FieldSet //////////////////////////////////

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetMask()
{
	return  BF0::template GetMask<T>() |  BF1::template GetMask<T>() |  BF2::template GetMask<T>() |  BF3::template GetMask<T>() |  BF4::template GetMask<T>() |  BF5::template GetMask<T>() |  BF6::template GetMask<T>() |  BF7::template GetMask<T>()
	     |  BF8::template GetMask<T>() |  BF9::template GetMask<T>() | BF10::template GetMask<T>() | BF11::template GetMask<T>() | BF12::template GetMask<T>() | BF13::template GetMask<T>() | BF14::template GetMask<T>() | BF15::template GetMask<T>()
	     | BF16::template GetMask<T>() | BF17::template GetMask<T>() | BF18::template GetMask<T>() | BF19::template GetMask<T>() | BF20::template GetMask<T>() | BF21::template GetMask<T>() | BF22::template GetMask<T>() | BF23::template GetMask<T>()
	     | BF24::template GetMask<T>() | BF25::template GetMask<T>() | BF26::template GetMask<T>() | BF27::template GetMask<T>() | BF28::template GetMask<T>() | BF29::template GetMask<T>() | BF30::template GetMask<T>() | BF31::template GetMask<T>()
	     | BF32::template GetMask<T>() | BF33::template GetMask<T>() | BF34::template GetMask<T>() | BF35::template GetMask<T>() | BF36::template GetMask<T>() | BF37::template GetMask<T>() | BF38::template GetMask<T>() | BF39::template GetMask<T>()
	     | BF40::template GetMask<T>() | BF41::template GetMask<T>() | BF42::template GetMask<T>() | BF43::template GetMask<T>() | BF44::template GetMask<T>() | BF45::template GetMask<T>() | BF46::template GetMask<T>() | BF47::template GetMask<T>()
	     | BF48::template GetMask<T>() | BF49::template GetMask<T>() | BF50::template GetMask<T>() | BF51::template GetMask<T>() | BF52::template GetMask<T>() | BF53::template GetMask<T>() | BF54::template GetMask<T>() | BF55::template GetMask<T>()
	     | BF56::template GetMask<T>() | BF57::template GetMask<T>() | BF58::template GetMask<T>() | BF59::template GetMask<T>() | BF60::template GetMask<T>() | BF61::template GetMask<T>() | BF62::template GetMask<T>() | BF63::template GetMask<T>();
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetAssignMask()
{
	return  BF0::template GetAssignMask<T>() |  BF1::template GetAssignMask<T>() |  BF2::template GetAssignMask<T>() |  BF3::template GetAssignMask<T>() |  BF4::template GetAssignMask<T>() |  BF5::template GetAssignMask<T>() |  BF6::template GetAssignMask<T>() |  BF7::template GetAssignMask<T>()
	     |  BF8::template GetAssignMask<T>() |  BF9::template GetAssignMask<T>() | BF10::template GetAssignMask<T>() | BF11::template GetAssignMask<T>() | BF12::template GetAssignMask<T>() | BF13::template GetAssignMask<T>() | BF14::template GetAssignMask<T>() | BF15::template GetAssignMask<T>()
	     | BF16::template GetAssignMask<T>() | BF17::template GetAssignMask<T>() | BF18::template GetAssignMask<T>() | BF19::template GetAssignMask<T>() | BF20::template GetAssignMask<T>() | BF21::template GetAssignMask<T>() | BF22::template GetAssignMask<T>() | BF23::template GetAssignMask<T>()
	     | BF24::template GetAssignMask<T>() | BF25::template GetAssignMask<T>() | BF26::template GetAssignMask<T>() | BF27::template GetAssignMask<T>() | BF28::template GetAssignMask<T>() | BF29::template GetAssignMask<T>() | BF30::template GetAssignMask<T>() | BF31::template GetAssignMask<T>()
	     | BF32::template GetAssignMask<T>() | BF33::template GetAssignMask<T>() | BF34::template GetAssignMask<T>() | BF35::template GetAssignMask<T>() | BF36::template GetAssignMask<T>() | BF37::template GetAssignMask<T>() | BF38::template GetAssignMask<T>() | BF39::template GetAssignMask<T>()
	     | BF40::template GetAssignMask<T>() | BF41::template GetAssignMask<T>() | BF42::template GetAssignMask<T>() | BF43::template GetAssignMask<T>() | BF44::template GetAssignMask<T>() | BF45::template GetAssignMask<T>() | BF46::template GetAssignMask<T>() | BF47::template GetAssignMask<T>()
	     | BF48::template GetAssignMask<T>() | BF49::template GetAssignMask<T>() | BF50::template GetAssignMask<T>() | BF51::template GetAssignMask<T>() | BF52::template GetAssignMask<T>() | BF53::template GetAssignMask<T>() | BF54::template GetAssignMask<T>() | BF55::template GetAssignMask<T>()
	     | BF56::template GetAssignMask<T>() | BF57::template GetAssignMask<T>() | BF58::template GetAssignMask<T>() | BF59::template GetAssignMask<T>() | BF60::template GetAssignMask<T>() | BF61::template GetAssignMask<T>() | BF62::template GetAssignMask<T>() | BF63::template GetAssignMask<T>();
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetWriteMask()
{
	return  BF0::template GetWriteMask<T>() |  BF1::template GetWriteMask<T>() |  BF2::template GetWriteMask<T>() |  BF3::template GetWriteMask<T>() |  BF4::template GetWriteMask<T>() |  BF5::template GetWriteMask<T>() |  BF6::template GetWriteMask<T>() |  BF7::template GetWriteMask<T>()
	     |  BF8::template GetWriteMask<T>() |  BF9::template GetWriteMask<T>() | BF10::template GetWriteMask<T>() | BF11::template GetWriteMask<T>() | BF12::template GetWriteMask<T>() | BF13::template GetWriteMask<T>() | BF14::template GetWriteMask<T>() | BF15::template GetWriteMask<T>()
	     | BF16::template GetWriteMask<T>() | BF17::template GetWriteMask<T>() | BF18::template GetWriteMask<T>() | BF19::template GetWriteMask<T>() | BF20::template GetWriteMask<T>() | BF21::template GetWriteMask<T>() | BF22::template GetWriteMask<T>() | BF23::template GetWriteMask<T>()
	     | BF24::template GetWriteMask<T>() | BF25::template GetWriteMask<T>() | BF26::template GetWriteMask<T>() | BF27::template GetWriteMask<T>() | BF28::template GetWriteMask<T>() | BF29::template GetWriteMask<T>() | BF30::template GetWriteMask<T>() | BF31::template GetWriteMask<T>()
	     | BF32::template GetWriteMask<T>() | BF33::template GetWriteMask<T>() | BF34::template GetWriteMask<T>() | BF35::template GetWriteMask<T>() | BF36::template GetWriteMask<T>() | BF37::template GetWriteMask<T>() | BF38::template GetWriteMask<T>() | BF39::template GetWriteMask<T>()
	     | BF40::template GetWriteMask<T>() | BF41::template GetWriteMask<T>() | BF42::template GetWriteMask<T>() | BF43::template GetWriteMask<T>() | BF44::template GetWriteMask<T>() | BF45::template GetWriteMask<T>() | BF46::template GetWriteMask<T>() | BF47::template GetWriteMask<T>()
	     | BF48::template GetWriteMask<T>() | BF49::template GetWriteMask<T>() | BF50::template GetWriteMask<T>() | BF51::template GetWriteMask<T>() | BF52::template GetWriteMask<T>() | BF53::template GetWriteMask<T>() | BF54::template GetWriteMask<T>() | BF55::template GetWriteMask<T>()
	     | BF56::template GetWriteMask<T>() | BF57::template GetWriteMask<T>() | BF58::template GetWriteMask<T>() | BF59::template GetWriteMask<T>() | BF60::template GetWriteMask<T>() | BF61::template GetWriteMask<T>() | BF62::template GetWriteMask<T>() | BF63::template GetWriteMask<T>();
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetReadMask()
{
	return  BF0::template GetReadMask<T>() |  BF1::template GetReadMask<T>() |  BF2::template GetReadMask<T>() |  BF3::template GetReadMask<T>() |  BF4::template GetReadMask<T>() |  BF5::template GetReadMask<T>() |  BF6::template GetReadMask<T>() |  BF7::template GetReadMask<T>()
	     |  BF8::template GetReadMask<T>() |  BF9::template GetReadMask<T>() | BF10::template GetReadMask<T>() | BF11::template GetReadMask<T>() | BF12::template GetReadMask<T>() | BF13::template GetReadMask<T>() | BF14::template GetReadMask<T>() | BF15::template GetReadMask<T>()
	     | BF16::template GetReadMask<T>() | BF17::template GetReadMask<T>() | BF18::template GetReadMask<T>() | BF19::template GetReadMask<T>() | BF20::template GetReadMask<T>() | BF21::template GetReadMask<T>() | BF22::template GetReadMask<T>() | BF23::template GetReadMask<T>()
	     | BF24::template GetReadMask<T>() | BF25::template GetReadMask<T>() | BF26::template GetReadMask<T>() | BF27::template GetReadMask<T>() | BF28::template GetReadMask<T>() | BF29::template GetReadMask<T>() | BF30::template GetReadMask<T>() | BF31::template GetReadMask<T>()
	     | BF32::template GetReadMask<T>() | BF33::template GetReadMask<T>() | BF34::template GetReadMask<T>() | BF35::template GetReadMask<T>() | BF36::template GetReadMask<T>() | BF37::template GetReadMask<T>() | BF38::template GetReadMask<T>() | BF39::template GetReadMask<T>()
	     | BF40::template GetReadMask<T>() | BF41::template GetReadMask<T>() | BF42::template GetReadMask<T>() | BF43::template GetReadMask<T>() | BF44::template GetReadMask<T>() | BF45::template GetReadMask<T>() | BF46::template GetReadMask<T>() | BF47::template GetReadMask<T>()
	     | BF48::template GetReadMask<T>() | BF49::template GetReadMask<T>() | BF50::template GetReadMask<T>() | BF51::template GetReadMask<T>() | BF52::template GetReadMask<T>() | BF53::template GetReadMask<T>() | BF54::template GetReadMask<T>() | BF55::template GetReadMask<T>()
	     | BF56::template GetReadMask<T>() | BF57::template GetReadMask<T>() | BF58::template GetReadMask<T>() | BF59::template GetReadMask<T>() | BF60::template GetReadMask<T>() | BF61::template GetReadMask<T>() | BF62::template GetReadMask<T>() | BF63::template GetReadMask<T>();
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Get(const T& storage)
{
	return storage & GetMask<T>();
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline void FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                          ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                          , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                          , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                          , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                          , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                          , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                          , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Set(T& storage, T fieldset_value)
{
	storage = (storage & ~GetMask<T>()) | (fieldset_value & GetMask<T>());
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
inline const std::string& FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                  ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                  , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                  , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                  , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                  , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                  , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                  , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetName()
{
	static std::string name;
	if(!name.empty()) return name;
	
	name += BF0::GetName();
	if(typeid(BF1) == typeid(NullField)) return name; name += ':'; name += BF1::GetName();
	if(typeid(BF2) == typeid(NullField)) return name; name += ':'; name += BF2::GetName();
	if(typeid(BF3) == typeid(NullField)) return name; name += ':'; name += BF3::GetName();
	if(typeid(BF4) == typeid(NullField)) return name; name += ':'; name += BF4::GetName();
	if(typeid(BF5) == typeid(NullField)) return name; name += ':'; name += BF5::GetName();
	if(typeid(BF6) == typeid(NullField)) return name; name += ':'; name += BF6::GetName();
	if(typeid(BF7) == typeid(NullField)) return name; name += ':'; name += BF7::GetName();
	if(typeid(BF8) == typeid(NullField)) return name; name += ':'; name += BF8::GetName();
	if(typeid(BF9) == typeid(NullField)) return name; name += ':'; name += BF9::GetName();
	if(typeid(BF10) == typeid(NullField)) return name; name += ':'; name += BF10::GetName();
	if(typeid(BF11) == typeid(NullField)) return name; name += ':'; name += BF11::GetName();
	if(typeid(BF12) == typeid(NullField)) return name; name += ':'; name += BF12::GetName();
	if(typeid(BF13) == typeid(NullField)) return name; name += ':'; name += BF13::GetName();
	if(typeid(BF14) == typeid(NullField)) return name; name += ':'; name += BF14::GetName();
	if(typeid(BF15) == typeid(NullField)) return name; name += ':'; name += BF15::GetName();
	if(typeid(BF16) == typeid(NullField)) return name; name += ':'; name += BF16::GetName();
	if(typeid(BF17) == typeid(NullField)) return name; name += ':'; name += BF17::GetName();
	if(typeid(BF18) == typeid(NullField)) return name; name += ':'; name += BF18::GetName();
	if(typeid(BF19) == typeid(NullField)) return name; name += ':'; name += BF19::GetName();
	if(typeid(BF20) == typeid(NullField)) return name; name += ':'; name += BF20::GetName();
	if(typeid(BF21) == typeid(NullField)) return name; name += ':'; name += BF21::GetName();
	if(typeid(BF22) == typeid(NullField)) return name; name += ':'; name += BF22::GetName();
	if(typeid(BF23) == typeid(NullField)) return name; name += ':'; name += BF23::GetName();
	if(typeid(BF24) == typeid(NullField)) return name; name += ':'; name += BF24::GetName();
	if(typeid(BF25) == typeid(NullField)) return name; name += ':'; name += BF25::GetName();
	if(typeid(BF26) == typeid(NullField)) return name; name += ':'; name += BF26::GetName();
	if(typeid(BF27) == typeid(NullField)) return name; name += ':'; name += BF27::GetName();
	if(typeid(BF28) == typeid(NullField)) return name; name += ':'; name += BF28::GetName();
	if(typeid(BF29) == typeid(NullField)) return name; name += ':'; name += BF29::GetName();
	if(typeid(BF30) == typeid(NullField)) return name; name += ':'; name += BF30::GetName();
	if(typeid(BF31) == typeid(NullField)) return name; name += ':'; name += BF31::GetName();
	if(typeid(BF32) == typeid(NullField)) return name; name += ':'; name += BF32::GetName();
	if(typeid(BF33) == typeid(NullField)) return name; name += ':'; name += BF33::GetName();
	if(typeid(BF34) == typeid(NullField)) return name; name += ':'; name += BF34::GetName();
	if(typeid(BF35) == typeid(NullField)) return name; name += ':'; name += BF35::GetName();
	if(typeid(BF36) == typeid(NullField)) return name; name += ':'; name += BF36::GetName();
	if(typeid(BF37) == typeid(NullField)) return name; name += ':'; name += BF37::GetName();
	if(typeid(BF38) == typeid(NullField)) return name; name += ':'; name += BF38::GetName();
	if(typeid(BF39) == typeid(NullField)) return name; name += ':'; name += BF39::GetName();
	if(typeid(BF40) == typeid(NullField)) return name; name += ':'; name += BF40::GetName();
	if(typeid(BF41) == typeid(NullField)) return name; name += ':'; name += BF41::GetName();
	if(typeid(BF42) == typeid(NullField)) return name; name += ':'; name += BF42::GetName();
	if(typeid(BF43) == typeid(NullField)) return name; name += ':'; name += BF43::GetName();
	if(typeid(BF44) == typeid(NullField)) return name; name += ':'; name += BF44::GetName();
	if(typeid(BF45) == typeid(NullField)) return name; name += ':'; name += BF45::GetName();
	if(typeid(BF46) == typeid(NullField)) return name; name += ':'; name += BF46::GetName();
	if(typeid(BF47) == typeid(NullField)) return name; name += ':'; name += BF47::GetName();
	if(typeid(BF48) == typeid(NullField)) return name; name += ':'; name += BF48::GetName();
	if(typeid(BF49) == typeid(NullField)) return name; name += ':'; name += BF49::GetName();
	if(typeid(BF50) == typeid(NullField)) return name; name += ':'; name += BF50::GetName();
	if(typeid(BF51) == typeid(NullField)) return name; name += ':'; name += BF51::GetName();
	if(typeid(BF52) == typeid(NullField)) return name; name += ':'; name += BF52::GetName();
	if(typeid(BF53) == typeid(NullField)) return name; name += ':'; name += BF53::GetName();
	if(typeid(BF54) == typeid(NullField)) return name; name += ':'; name += BF54::GetName();
	if(typeid(BF55) == typeid(NullField)) return name; name += ':'; name += BF55::GetName();
	if(typeid(BF56) == typeid(NullField)) return name; name += ':'; name += BF56::GetName();
	if(typeid(BF57) == typeid(NullField)) return name; name += ':'; name += BF57::GetName();
	if(typeid(BF58) == typeid(NullField)) return name; name += ':'; name += BF58::GetName();
	if(typeid(BF59) == typeid(NullField)) return name; name += ':'; name += BF59::GetName();
	if(typeid(BF60) == typeid(NullField)) return name; name += ':'; name += BF60::GetName();
	if(typeid(BF61) == typeid(NullField)) return name; name += ':'; name += BF61::GetName();
	if(typeid(BF62) == typeid(NullField)) return name; name += ':'; name += BF62::GetName();
	if(typeid(BF63) == typeid(NullField)) return name; name += ':'; name += BF63::GetName();
	return name;
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T>
inline void FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                    ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                    , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                    , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                    , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                    , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                    , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                    , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::ShortPrettyPrint(std::ostream& os, const T& storage)
{
	BF0::ShortPrettyPrint(os, storage);
	if(typeid(BF1) == typeid(NullField)) return; os << ':'; BF1::ShortPrettyPrint(os, storage);
	if(typeid(BF2) == typeid(NullField)) return; os << ':'; BF2::ShortPrettyPrint(os, storage);
	if(typeid(BF3) == typeid(NullField)) return; os << ':'; BF3::ShortPrettyPrint(os, storage);
	if(typeid(BF4) == typeid(NullField)) return; os << ':'; BF4::ShortPrettyPrint(os, storage);
	if(typeid(BF5) == typeid(NullField)) return; os << ':'; BF5::ShortPrettyPrint(os, storage);
	if(typeid(BF6) == typeid(NullField)) return; os << ':'; BF6::ShortPrettyPrint(os, storage);
	if(typeid(BF7) == typeid(NullField)) return; os << ':'; BF7::ShortPrettyPrint(os, storage);
	if(typeid(BF8) == typeid(NullField)) return; os << ':'; BF8::ShortPrettyPrint(os, storage);
	if(typeid(BF9) == typeid(NullField)) return; os << ':'; BF9::ShortPrettyPrint(os, storage);
	if(typeid(BF10) == typeid(NullField)) return; os << ':'; BF10::ShortPrettyPrint(os, storage);
	if(typeid(BF11) == typeid(NullField)) return; os << ':'; BF11::ShortPrettyPrint(os, storage);
	if(typeid(BF12) == typeid(NullField)) return; os << ':'; BF12::ShortPrettyPrint(os, storage);
	if(typeid(BF13) == typeid(NullField)) return; os << ':'; BF13::ShortPrettyPrint(os, storage);
	if(typeid(BF14) == typeid(NullField)) return; os << ':'; BF14::ShortPrettyPrint(os, storage);
	if(typeid(BF15) == typeid(NullField)) return; os << ':'; BF15::ShortPrettyPrint(os, storage);
	if(typeid(BF16) == typeid(NullField)) return; os << ':'; BF16::ShortPrettyPrint(os, storage);
	if(typeid(BF17) == typeid(NullField)) return; os << ':'; BF17::ShortPrettyPrint(os, storage);
	if(typeid(BF18) == typeid(NullField)) return; os << ':'; BF18::ShortPrettyPrint(os, storage);
	if(typeid(BF19) == typeid(NullField)) return; os << ':'; BF19::ShortPrettyPrint(os, storage);
	if(typeid(BF20) == typeid(NullField)) return; os << ':'; BF20::ShortPrettyPrint(os, storage);
	if(typeid(BF21) == typeid(NullField)) return; os << ':'; BF21::ShortPrettyPrint(os, storage);
	if(typeid(BF22) == typeid(NullField)) return; os << ':'; BF22::ShortPrettyPrint(os, storage);
	if(typeid(BF23) == typeid(NullField)) return; os << ':'; BF23::ShortPrettyPrint(os, storage);
	if(typeid(BF24) == typeid(NullField)) return; os << ':'; BF24::ShortPrettyPrint(os, storage);
	if(typeid(BF25) == typeid(NullField)) return; os << ':'; BF25::ShortPrettyPrint(os, storage);
	if(typeid(BF26) == typeid(NullField)) return; os << ':'; BF26::ShortPrettyPrint(os, storage);
	if(typeid(BF27) == typeid(NullField)) return; os << ':'; BF27::ShortPrettyPrint(os, storage);
	if(typeid(BF28) == typeid(NullField)) return; os << ':'; BF28::ShortPrettyPrint(os, storage);
	if(typeid(BF29) == typeid(NullField)) return; os << ':'; BF29::ShortPrettyPrint(os, storage);
	if(typeid(BF30) == typeid(NullField)) return; os << ':'; BF30::ShortPrettyPrint(os, storage);
	if(typeid(BF31) == typeid(NullField)) return; os << ':'; BF31::ShortPrettyPrint(os, storage);
	if(typeid(BF32) == typeid(NullField)) return; os << ':'; BF32::ShortPrettyPrint(os, storage);
	if(typeid(BF33) == typeid(NullField)) return; os << ':'; BF33::ShortPrettyPrint(os, storage);
	if(typeid(BF34) == typeid(NullField)) return; os << ':'; BF34::ShortPrettyPrint(os, storage);
	if(typeid(BF35) == typeid(NullField)) return; os << ':'; BF35::ShortPrettyPrint(os, storage);
	if(typeid(BF36) == typeid(NullField)) return; os << ':'; BF36::ShortPrettyPrint(os, storage);
	if(typeid(BF37) == typeid(NullField)) return; os << ':'; BF37::ShortPrettyPrint(os, storage);
	if(typeid(BF38) == typeid(NullField)) return; os << ':'; BF38::ShortPrettyPrint(os, storage);
	if(typeid(BF39) == typeid(NullField)) return; os << ':'; BF39::ShortPrettyPrint(os, storage);
	if(typeid(BF40) == typeid(NullField)) return; os << ':'; BF40::ShortPrettyPrint(os, storage);
	if(typeid(BF41) == typeid(NullField)) return; os << ':'; BF41::ShortPrettyPrint(os, storage);
	if(typeid(BF42) == typeid(NullField)) return; os << ':'; BF42::ShortPrettyPrint(os, storage);
	if(typeid(BF43) == typeid(NullField)) return; os << ':'; BF43::ShortPrettyPrint(os, storage);
	if(typeid(BF44) == typeid(NullField)) return; os << ':'; BF44::ShortPrettyPrint(os, storage);
	if(typeid(BF45) == typeid(NullField)) return; os << ':'; BF45::ShortPrettyPrint(os, storage);
	if(typeid(BF46) == typeid(NullField)) return; os << ':'; BF46::ShortPrettyPrint(os, storage);
	if(typeid(BF47) == typeid(NullField)) return; os << ':'; BF47::ShortPrettyPrint(os, storage);
	if(typeid(BF48) == typeid(NullField)) return; os << ':'; BF48::ShortPrettyPrint(os, storage);
	if(typeid(BF49) == typeid(NullField)) return; os << ':'; BF49::ShortPrettyPrint(os, storage);
	if(typeid(BF50) == typeid(NullField)) return; os << ':'; BF50::ShortPrettyPrint(os, storage);
	if(typeid(BF51) == typeid(NullField)) return; os << ':'; BF51::ShortPrettyPrint(os, storage);
	if(typeid(BF52) == typeid(NullField)) return; os << ':'; BF52::ShortPrettyPrint(os, storage);
	if(typeid(BF53) == typeid(NullField)) return; os << ':'; BF53::ShortPrettyPrint(os, storage);
	if(typeid(BF54) == typeid(NullField)) return; os << ':'; BF54::ShortPrettyPrint(os, storage);
	if(typeid(BF55) == typeid(NullField)) return; os << ':'; BF55::ShortPrettyPrint(os, storage);
	if(typeid(BF56) == typeid(NullField)) return; os << ':'; BF56::ShortPrettyPrint(os, storage);
	if(typeid(BF57) == typeid(NullField)) return; os << ':'; BF57::ShortPrettyPrint(os, storage);
	if(typeid(BF58) == typeid(NullField)) return; os << ':'; BF58::ShortPrettyPrint(os, storage);
	if(typeid(BF59) == typeid(NullField)) return; os << ':'; BF59::ShortPrettyPrint(os, storage);
	if(typeid(BF60) == typeid(NullField)) return; os << ':'; BF60::ShortPrettyPrint(os, storage);
	if(typeid(BF61) == typeid(NullField)) return; os << ':'; BF61::ShortPrettyPrint(os, storage);
	if(typeid(BF62) == typeid(NullField)) return; os << ':'; BF62::ShortPrettyPrint(os, storage);
	if(typeid(BF63) == typeid(NullField)) return; os << ':'; BF63::ShortPrettyPrint(os, storage);
}

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
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T>
inline void FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                    ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                    , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                    , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                    , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                    , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                    , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                    , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::LongPrettyPrint(std::ostream& os, const T& storage)
{
	BF0::LongPrettyPrint(os, storage);
	if(typeid(BF1) == typeid(NullField)) return; os << std::endl; BF1::LongPrettyPrint(os, storage);
	if(typeid(BF2) == typeid(NullField)) return; os << std::endl; BF2::LongPrettyPrint(os, storage);
	if(typeid(BF3) == typeid(NullField)) return; os << std::endl; BF3::LongPrettyPrint(os, storage);
	if(typeid(BF4) == typeid(NullField)) return; os << std::endl; BF4::LongPrettyPrint(os, storage);
	if(typeid(BF5) == typeid(NullField)) return; os << std::endl; BF5::LongPrettyPrint(os, storage);
	if(typeid(BF6) == typeid(NullField)) return; os << std::endl; BF6::LongPrettyPrint(os, storage);
	if(typeid(BF7) == typeid(NullField)) return; os << std::endl; BF7::LongPrettyPrint(os, storage);
	if(typeid(BF8) == typeid(NullField)) return; os << std::endl; BF8::LongPrettyPrint(os, storage);
	if(typeid(BF9) == typeid(NullField)) return; os << std::endl; BF9::LongPrettyPrint(os, storage);
	if(typeid(BF10) == typeid(NullField)) return; os << std::endl; BF10::LongPrettyPrint(os, storage);
	if(typeid(BF11) == typeid(NullField)) return; os << std::endl; BF11::LongPrettyPrint(os, storage);
	if(typeid(BF12) == typeid(NullField)) return; os << std::endl; BF12::LongPrettyPrint(os, storage);
	if(typeid(BF13) == typeid(NullField)) return; os << std::endl; BF13::LongPrettyPrint(os, storage);
	if(typeid(BF14) == typeid(NullField)) return; os << std::endl; BF14::LongPrettyPrint(os, storage);
	if(typeid(BF15) == typeid(NullField)) return; os << std::endl; BF15::LongPrettyPrint(os, storage);
	if(typeid(BF16) == typeid(NullField)) return; os << std::endl; BF16::LongPrettyPrint(os, storage);
	if(typeid(BF17) == typeid(NullField)) return; os << std::endl; BF17::LongPrettyPrint(os, storage);
	if(typeid(BF18) == typeid(NullField)) return; os << std::endl; BF18::LongPrettyPrint(os, storage);
	if(typeid(BF19) == typeid(NullField)) return; os << std::endl; BF19::LongPrettyPrint(os, storage);
	if(typeid(BF20) == typeid(NullField)) return; os << std::endl; BF20::LongPrettyPrint(os, storage);
	if(typeid(BF21) == typeid(NullField)) return; os << std::endl; BF21::LongPrettyPrint(os, storage);
	if(typeid(BF22) == typeid(NullField)) return; os << std::endl; BF22::LongPrettyPrint(os, storage);
	if(typeid(BF23) == typeid(NullField)) return; os << std::endl; BF23::LongPrettyPrint(os, storage);
	if(typeid(BF24) == typeid(NullField)) return; os << std::endl; BF24::LongPrettyPrint(os, storage);
	if(typeid(BF25) == typeid(NullField)) return; os << std::endl; BF25::LongPrettyPrint(os, storage);
	if(typeid(BF26) == typeid(NullField)) return; os << std::endl; BF26::LongPrettyPrint(os, storage);
	if(typeid(BF27) == typeid(NullField)) return; os << std::endl; BF27::LongPrettyPrint(os, storage);
	if(typeid(BF28) == typeid(NullField)) return; os << std::endl; BF28::LongPrettyPrint(os, storage);
	if(typeid(BF29) == typeid(NullField)) return; os << std::endl; BF29::LongPrettyPrint(os, storage);
	if(typeid(BF30) == typeid(NullField)) return; os << std::endl; BF30::LongPrettyPrint(os, storage);
	if(typeid(BF31) == typeid(NullField)) return; os << std::endl; BF31::LongPrettyPrint(os, storage);
	if(typeid(BF32) == typeid(NullField)) return; os << std::endl; BF32::LongPrettyPrint(os, storage);
	if(typeid(BF33) == typeid(NullField)) return; os << std::endl; BF33::LongPrettyPrint(os, storage);
	if(typeid(BF34) == typeid(NullField)) return; os << std::endl; BF34::LongPrettyPrint(os, storage);
	if(typeid(BF35) == typeid(NullField)) return; os << std::endl; BF35::LongPrettyPrint(os, storage);
	if(typeid(BF36) == typeid(NullField)) return; os << std::endl; BF36::LongPrettyPrint(os, storage);
	if(typeid(BF37) == typeid(NullField)) return; os << std::endl; BF37::LongPrettyPrint(os, storage);
	if(typeid(BF38) == typeid(NullField)) return; os << std::endl; BF38::LongPrettyPrint(os, storage);
	if(typeid(BF39) == typeid(NullField)) return; os << std::endl; BF39::LongPrettyPrint(os, storage);
	if(typeid(BF40) == typeid(NullField)) return; os << std::endl; BF40::LongPrettyPrint(os, storage);
	if(typeid(BF41) == typeid(NullField)) return; os << std::endl; BF41::LongPrettyPrint(os, storage);
	if(typeid(BF42) == typeid(NullField)) return; os << std::endl; BF42::LongPrettyPrint(os, storage);
	if(typeid(BF43) == typeid(NullField)) return; os << std::endl; BF43::LongPrettyPrint(os, storage);
	if(typeid(BF44) == typeid(NullField)) return; os << std::endl; BF44::LongPrettyPrint(os, storage);
	if(typeid(BF45) == typeid(NullField)) return; os << std::endl; BF45::LongPrettyPrint(os, storage);
	if(typeid(BF46) == typeid(NullField)) return; os << std::endl; BF46::LongPrettyPrint(os, storage);
	if(typeid(BF47) == typeid(NullField)) return; os << std::endl; BF47::LongPrettyPrint(os, storage);
	if(typeid(BF48) == typeid(NullField)) return; os << std::endl; BF48::LongPrettyPrint(os, storage);
	if(typeid(BF49) == typeid(NullField)) return; os << std::endl; BF49::LongPrettyPrint(os, storage);
	if(typeid(BF50) == typeid(NullField)) return; os << std::endl; BF50::LongPrettyPrint(os, storage);
	if(typeid(BF51) == typeid(NullField)) return; os << std::endl; BF51::LongPrettyPrint(os, storage);
	if(typeid(BF52) == typeid(NullField)) return; os << std::endl; BF52::LongPrettyPrint(os, storage);
	if(typeid(BF53) == typeid(NullField)) return; os << std::endl; BF53::LongPrettyPrint(os, storage);
	if(typeid(BF54) == typeid(NullField)) return; os << std::endl; BF54::LongPrettyPrint(os, storage);
	if(typeid(BF55) == typeid(NullField)) return; os << std::endl; BF55::LongPrettyPrint(os, storage);
	if(typeid(BF56) == typeid(NullField)) return; os << std::endl; BF56::LongPrettyPrint(os, storage);
	if(typeid(BF57) == typeid(NullField)) return; os << std::endl; BF57::LongPrettyPrint(os, storage);
	if(typeid(BF58) == typeid(NullField)) return; os << std::endl; BF58::LongPrettyPrint(os, storage);
	if(typeid(BF59) == typeid(NullField)) return; os << std::endl; BF59::LongPrettyPrint(os, storage);
	if(typeid(BF60) == typeid(NullField)) return; os << std::endl; BF60::LongPrettyPrint(os, storage);
	if(typeid(BF61) == typeid(NullField)) return; os << std::endl; BF61::LongPrettyPrint(os, storage);
	if(typeid(BF62) == typeid(NullField)) return; os << std::endl; BF62::LongPrettyPrint(os, storage);
	if(typeid(BF63) == typeid(NullField)) return; os << std::endl; BF63::LongPrettyPrint(os, storage);
}

////////////////////////////////// Register<> /////////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE_MASK;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const unsigned int Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::SIZE;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const Access Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::ACCESS;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetMask() const
{
	return (REGISTER::ALL::template GetMask<TYPE>() & TYPE_MASK);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetAssignMask() const
{
	return (_ACCESS & HW_RW) ? (REGISTER::ALL::template GetAssignMask<TYPE>() & TYPE_MASK) : 0;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetWriteMask() const
{
	return (_ACCESS & SW_W) ? (REGISTER::ALL::template GetWriteMask<TYPE>() & TYPE_MASK): 0;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetReadMask() const
{
	return (_ACCESS & SW_R) ? (REGISTER::ALL::template GetReadMask<TYPE>() & TYPE_MASK) : 0;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::HasBit(unsigned int bit_offset) const
{
	return REGISTER::ALL::template GetMask<TYPE>() & (TYPE(1) << bit_offset);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Get(unsigned int bit_offset) const
{
	return ((value & TYPE_MASK) >> bit_offset) & 1;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Set(unsigned int bit_offset, TYPE bit_value)
{
	value = (value | (TYPE(1) << bit_offset)) & GetMask();
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
template <typename FIELD> inline bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::HasField() const
{
	return REGISTER::ALL::template GetMask<TYPE>() & FIELD::template GetMask<TYPE>();
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
template <typename FIELD> inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Get() const
{
	return FIELD::Get(value);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
template <typename FIELD> inline void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Set(TYPE field_value)
{
	FIELD::Set(value, field_value);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Register()
	: value(0)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Register(TYPE _value)
	: value(_value & TYPE_MASK)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::WithinRegisterFileCtor(unsigned int index)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Initialize(TYPE _value)
{
	value = _value & TYPE_MASK;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>& Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::operator = (const TYPE& _value)
{
	value = (value & ~GetAssignMask()) | (_value & GetAssignMask());
	return *this;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::operator TYPE () const
{
	return value & TYPE_MASK;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
inline typename Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::operator [] (unsigned int bit_offset) const
{
	return Get(bit_offset);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
WarningStatus Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Write(const TYPE& _value, const TYPE& bit_enable)
{
	WarningStatus ws(WS_OK);
	const TYPE write_mask = bit_enable & GetWriteMask();
	const TYPE unwritable_mask = bit_enable & ~GetWriteMask();
	
	TYPE old_value = value & TYPE_MASK;
	
	if((_value & bit_enable & TYPE_MASK) != (_value & bit_enable))
	{
		// writing out-of-range register value
		ws = WarningStatus(ws | WS_WOORV);
	}
	
	if(!(_ACCESS & SW_R))
	{
		// writing read-only register
		ws = WarningStatus(ws | WS_WROR);
	}
	else if((_value & unwritable_mask) != (old_value & unwritable_mask))
	{
		// writing read-only bits
		ws = WarningStatus(ws | WS_WROB);
	}
	
	value = (old_value & ~write_mask) | (_value & write_mask);
	
	return ws;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
WarningStatus Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Read(TYPE& _value, const TYPE& bit_enable)
{
	WarningStatus ws(WS_OK);
	const TYPE read_mask = bit_enable & GetReadMask();
	
	if(!(_ACCESS & SW_R))
	{
		// reading write-only register
		ws = WarningStatus(ws | WS_RWOR);
	}
	
	_value = (_value & ~read_mask) | (value & read_mask);
	
	return ws;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::DebugWrite(const TYPE& _value, const TYPE& bit_enable)
{
	TYPE old_value = value & TYPE_MASK;
	TYPE debug_write_mask = bit_enable & GetAssignMask();
	value = (old_value & ~debug_write_mask) | (_value & debug_write_mask);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::DebugRead(TYPE& _value, const TYPE& bit_enable)
{
	_value = (_value & ~bit_enable & ~GetMask()) | (value & bit_enable & GetMask());
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
WarningStatus Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Write(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	TYPE *data_to_write = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(TYPE *) bit_enable_ptr : ~TYPE(0);
	return Write(*data_to_write, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
WarningStatus Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::Read(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	TYPE *read_data = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(TYPE *) bit_enable_ptr : ~TYPE(0);
	return Read(*read_data, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::DebugWrite(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	TYPE *data_to_write = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(TYPE *) bit_enable_ptr : ~TYPE(0);
	DebugWrite(*data_to_write, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::DebugRead(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	TYPE *read_data = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(TYPE *) bit_enable_ptr : ~TYPE(0);
	DebugRead(*read_data, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
std::ostream& operator << (std::ostream& os, const Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>& reg)
{
	return os << (reg.value & Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::TYPE_MASK);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::ShortPrettyPrint(std::ostream& os) const
{
	std::ios_base::fmtflags ff = os.flags();
	os << GetName() << "=";
	if((ff & os.hex) && (value >= 16)) os << "0x";
	else if((ff & os.oct) && (value >= 8)) os << "0";
	os << (value & TYPE_MASK);
	os << "<";
	REGISTER::ALL::ShortPrettyPrint(os, value & TYPE_MASK);
	os << ">";
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::LongPrettyPrint(std::ostream& os) const
{
	LongPrettyPrinter::PrintHeader(os);
	LongPrettyPrinter::PrintKind(os, "register");
	LongPrettyPrinter::PrintRegisterSize(os, _SIZE);
	LongPrettyPrinter::PrintDisplayName(os, GetDisplayName());
	LongPrettyPrinter::PrintValue(os, value & TYPE_MASK);
	LongPrettyPrinter::PrintAccess(os, _ACCESS);
	LongPrettyPrinter::PrintDescription(os, GetDescription());
	os << std::endl;
	return REGISTER::ALL::LongPrettyPrint(os, value & TYPE_MASK);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_NAME, (intptr_t) this, name);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this, disp_name);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
void Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DESC, (intptr_t) this, desc);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
unsigned int Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetSize() const
{
	return _SIZE;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
Access Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetAccess() const
{
	return _ACCESS;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsReadable() const
{
	return _ACCESS & SW_R;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsWritable() const
{
	return _ACCESS & SW_W;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsWriteOnce() const
{
	return _ACCESS & SW_W1;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsReadOnly() const
{
	return (_ACCESS & SW_RW1) == SW_R;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsWriteOnly() const
{
	return (_ACCESS & SW_RW1) == SW_W;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
bool Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::IsWriteOnlyAndOnce() const
{
	return (_ACCESS & SW_RW1) == SW_W1;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const std::string& Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetName() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_NAME, (intptr_t) this);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const std::string& Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetDisplayName() const
{
	const std::string& display_name = PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this);
	if(!display_name.empty()) return display_name;
	return GetName();
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
const std::string& Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::GetDescription() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DESC, (intptr_t) this);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE>
unisim::service::interfaces::Register *Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE>::CreateRegisterInterface()
{
	typedef Register<REGISTER, _SIZE, _ACCESS, REGISTER_BASE> RegType;
	
	struct RegisterInterface : public unisim::service::interfaces::Register
	{
		RegisterInterface(RegType *_reg) : reg(_reg) {}
		virtual const char *GetName() const { return reg->GetName().c_str(); }
		virtual void GetValue(void *buffer) const { reg->DebugRead((unsigned char *) buffer); }
		virtual void SetValue(const void *buffer) { reg->DebugWrite((unsigned char *) buffer); }
		virtual int GetSize() const { return (reg->GetSize() + 7) / 8; }
		
		RegType *reg;
	};
	
	return new RegisterInterface(this);
}

/////////////////////////////// RegisterFile<> ////////////////////////////////

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::RegisterFile()
	: regs()
{
	unsigned int index;
	
	for(index = 0; index < _DIM; index++)
	{
		regs[index].WithinRegisterFileCtor(index);
	}
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::RegisterFile(typename REGISTER::TYPE value)
	: regs()
{
	unsigned int index;
	
	for(index = 0; index < _DIM; index++)
	{
		regs[index].WithinRegisterFileCtor(index);
		regs[index].Initialize(value);
	}
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
inline REGISTER& RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::operator [] (int index)
{
	return regs[index];
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
inline const REGISTER& RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::operator [] (int index) const
{
	return regs[index];
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
inline unsigned int RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::GetDim() const
{
	return _DIM;
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(GetNameKey(), name);
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(GetDisplayNameKey(), disp_name);
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(GetDescriptionKey(), desc);
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::GetName() const
{
	return PropertyRegistry::GetStringProperty(GetNameKey());
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::GetDisplayName() const
{
	return PropertyRegistry::GetStringProperty(GetDisplayNameKey());
}

template <typename REGISTER, unsigned int _DIM, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, REGISTER_FILE_BASE>::GetDescription() const
{
	return PropertyRegistry::GetStringProperty(GetDescriptionKey());
}

/////////////////////////// AddressableRegister<> /////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
AddressableRegister<REGISTER, _SIZE, _ACCESS>::AddressableRegister()
	: Super()
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
AddressableRegister<REGISTER, _SIZE, _ACCESS>::AddressableRegister(typename Super::TYPE value)
	: Super(value)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
unsigned int AddressableRegister<REGISTER, _SIZE, _ACCESS>::__ARB_GetSize__() const
{
	return _SIZE;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
WarningStatus AddressableRegister<REGISTER, _SIZE, _ACCESS>::__ARB_Write__(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	return Super::Write(data_ptr, bit_enable_ptr);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
WarningStatus AddressableRegister<REGISTER, _SIZE, _ACCESS>::__ARB_Read__(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	return Super::Read(data_ptr, bit_enable_ptr);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
void AddressableRegister<REGISTER, _SIZE, _ACCESS>::__ARB_DebugWrite__(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	Super::DebugWrite(data_ptr, bit_enable_ptr);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
void AddressableRegister<REGISTER, _SIZE, _ACCESS>::__ARB_DebugRead__(unsigned char *data_ptr, unsigned char *bit_enable_ptr)
{
	Super::DebugRead(data_ptr, bit_enable_ptr);
}

///////////////////////////// AddressableRegisterHandle<> ///////////////////////////

template <typename ADDRESS>
AddressableRegisterHandle<ADDRESS>::AddressableRegisterHandle(ADDRESS _addr, unsigned int _size, AddressableRegisterBase *_arb)
	: addr(_addr)
	, size(_size)
	, arb(_arb)
	, ref_count(0)
{
}

template <typename ADDRESS>
void AddressableRegisterHandle<ADDRESS>::Acquire()
{
	ref_count++;
}

template <typename ADDRESS>
void AddressableRegisterHandle<ADDRESS>::Release()
{
	if(ref_count && (--ref_count == 0))
	{
		delete this;
	}
}

///////////////////////////// RegisterAddressMap<> ////////////////////////////

template <typename ADDRESS>
RegisterAddressMap<ADDRESS>::RegisterAddressMap()
	: optimized(true)
	, optimizable(true)
	, addr_range(ADDRESS(-1), ADDRESS(0))
	, opt_reg_addr_map()
	, reg_addr_map()
{
}

template <typename ADDRESS>
RegisterAddressMap<ADDRESS>::~RegisterAddressMap()
{
	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS> *>::const_iterator it;

	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		AddressableRegisterHandle<ADDRESS> *arh = (*it).second;
		
		if(arh)
		{
			arh->Release();
		}
	}
}

template <typename ADDRESS>
void RegisterAddressMap<ADDRESS>::MapRegister(ADDRESS addr, AddressableRegisterBase *arb, unsigned int size)
{
	unsigned int reg_byte_size = size ? size : (arb->__ARB_GetSize__() + 7) / 8;

	unsigned int byte_idx;

	if(reg_byte_size)
	{
		optimized = false;
		optimizable = true;
		
		AddressableRegisterHandle<ADDRESS> *arh = new AddressableRegisterHandle<ADDRESS>(addr, reg_byte_size, arb);
		
		for(byte_idx = 0; byte_idx < reg_byte_size; byte_idx++)
		{
			ADDRESS byte_addr = addr + byte_idx;
			
			// check that byte is not already mapped
			typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS> *>::const_iterator it = reg_addr_map.find(byte_addr);
			
			if(it == reg_addr_map.end())
			{
				arh->Acquire();
				reg_addr_map.insert(std::pair<ADDRESS, AddressableRegisterHandle<ADDRESS> *>(byte_addr, arh));
			}
		}
	}
}

template <typename ADDRESS>
void RegisterAddressMap<ADDRESS>::Optimize() const
{
	opt_reg_addr_map.clear();
	
	if(reg_addr_map.empty())
	{
		optimizable = true;
		optimized = true;
		return;
	}
	
	ADDRESS num_addr_holes = 0;
	addr_range.first = ADDRESS(-1);
	addr_range.second = 0;

	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS> *>::const_iterator it;
	ADDRESS next_addr = 1;

	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		ADDRESS addr = (*it).first;
		if(addr < addr_range.first) addr_range.first = addr;
		if(addr > addr_range.second) addr_range.second = addr;
		if(addr != addr_range.first)
		{
			if(addr > next_addr)
			{
				num_addr_holes += addr - next_addr;
			}
		}
		next_addr = addr + 1;
	}

	if((addr_range.first == 0) && (addr_range.second == ADDRESS(-1)))
	{
		optimizable = false;
		optimized = false;
		return; // whole address space: unoptimizable
	}

	// "reasonably optimizable" means that at least half of the register map address range shall be filled
	ADDRESS threshold_range = (addr_range.second - addr_range.first + 1) / 2;
	
	if(num_addr_holes > threshold_range)
	{
		optimizable = false;
		optimized = false;
		return; // not reasonably optimizable
	}
	   
	opt_reg_addr_map.resize(addr_range.second - addr_range.first + 1);
	
	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		ADDRESS addr = (*it).first;
		AddressableRegisterHandle<ADDRESS> *arh = (*it).second;
		opt_reg_addr_map[addr - addr_range.first] = arh;
	}
	
	optimizable = true;
	optimized = true;
}

template <typename ADDRESS>
AddressableRegisterHandle<ADDRESS> *RegisterAddressMap<ADDRESS>::FindAddressableRegister(ADDRESS addr) const
{
	if(optimized)
	{
		return opt_reg_addr_map.size() ? opt_reg_addr_map[addr - addr_range.first] : 0;
	}
	
	if(optimizable)
	{
		Optimize();
	}
	
	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS> *>::const_iterator it = reg_addr_map.find(addr);

	return it != reg_addr_map.end() ? (*it).second : 0;
}

template <typename ADDRESS>
bool RegisterAddressMap<ADDRESS>::Write(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return false;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		unsigned char reg_be[reg_size];
		
		memset(reg_value, 0, sizeof(reg_value));
		memset(reg_be, 0, sizeof(reg_be));
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = data_ptr[byte_offset];
			if(host_endian != target_endian)
			{
				reg_value[reg_size_minus_1 - byte_pos] = byte_value;
				reg_be[reg_size_minus_1 - byte_pos] = 0xff;
			}
			else
			{
				reg_value[byte_pos] = byte_value;
				reg_be[byte_pos] = 0xff;
			}
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
		
		arh->Write(reg_value, reg_be);
	}
	
	return true;
}

template <typename ADDRESS>
bool RegisterAddressMap<ADDRESS>::Read(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return false;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		
		arh->Read(reg_value);
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = (host_endian != target_endian) ? reg_value[reg_size_minus_1 - byte_pos] : reg_value[byte_pos];
			data_ptr[byte_offset] = byte_value;
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
	}
	
	return true;
}

template <typename ADDRESS>
unsigned int RegisterAddressMap<ADDRESS>::DebugWrite(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		unsigned char reg_be[reg_size];
		
		memset(reg_value, 0, sizeof(reg_value));
		memset(reg_be, 0, sizeof(reg_be));
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = data_ptr[byte_offset];
			if(host_endian != target_endian)
			{
				reg_value[reg_size_minus_1 - byte_pos] = byte_value;
				reg_be[reg_size_minus_1 - byte_pos] = 0xff;
			}
			else
			{
				reg_value[byte_pos] = byte_value;
				reg_be[byte_pos] = 0xff;
			}
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
		
		arh->DebugWrite(reg_value, reg_be);
	}
	
	return true;
}

template <typename ADDRESS>
unsigned int RegisterAddressMap<ADDRESS>::DebugRead(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		
		arh->DebugRead(reg_value);
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = (host_endian != target_endian) ? reg_value[reg_size_minus_1 - byte_pos] : reg_value[byte_pos];
			data_ptr[byte_offset] = byte_value;
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
	}
	
	return true;
}

#if 0
template <typename ADDRESS>
unsigned int RegisterAddressMap<ADDRESS>::DebugWrite(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length; byte_offset++)
	{
		uint8_t byte_value = data_ptr[byte_offset]; 
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		unsigned int reg_size = arh->GetSize();
		unsigned char reg_value[reg_size];
		arh->DebugRead(reg_value);
		if(host_endian != target_endian)
		{
			reg_value[reg_size - 1 - (byte_addr - arh->GetAddress())] = byte_value;
		}
		else
		{
			reg_value[byte_addr - arh->GetAddress()] = byte_value;
		}
		
		arh->DebugWrite(reg_value);
	}
	
	return data_length;
}

template <typename ADDRESS>
unsigned int RegisterAddressMap<ADDRESS>::DebugRead(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length, unisim::util::endian::endian_type target_endian)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length; byte_offset++)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		unsigned int reg_size = arh->GetSize();
		unsigned char reg_value[reg_size];
		arh->DebugRead(reg_value);
		
		data_ptr[byte_offset] = (host_endian != target_endian) ? reg_value[reg_size - 1 - (byte_addr - arh->GetAddress())] : reg_value[byte_addr - arh->GetAddress()];
	}
	
	return data_length;
}
#endif

} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_CORE_REGISTER_TCC__
