/*
 *  Copyright (c) 2014,
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

#include "utilities/trace_file.h"
#include "core/kernel.h"
#include <math.h>

namespace sc_core {

static const char *time_unit_strings[SC_SEC + 1] = { "fs", "ps", "ns", "us", "ms", "s" };

///////////////////////////////// definition //////////////////////////////////

////////////////////////////// global functions ///////////////////////////////

sc_trace_file* sc_create_vcd_trace_file(const char *name)
{
	return sc_kernel::get_kernel()->create_vcd_trace_file(name);
}

void sc_close_vcd_trace_file(sc_trace_file *tf)
{
	tf->close();
}

void sc_write_comment(sc_trace_file *tf , const std::string& comment)
{
	tf->write_comment(comment);
}

void sc_trace(sc_trace_file *tf, const bool& value, const std::string& name)
{
	tf->trace(value, name);
}

void sc_trace(sc_trace_file *tf, const bool *value, const std::string& name)
{
	tf->trace(value, name);
}

void sc_trace(sc_trace_file *tf, const float& value, const std::string& name)
{
	tf->trace(value, name);
}

void sc_trace(sc_trace_file *tf, const float *value, const std::string& name)
{
	tf->trace(value, name);
}

void sc_trace(sc_trace_file *tf, const double& value, const std::string& name)
{
	tf->trace(value, name);
}

void sc_trace(sc_trace_file *tf, const double *value, const std::string& name)
{
	tf->trace(value, name);
}

// void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast& value, const std::string& name)
// {
// 	tf->trace(value, name);
// }
// 
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast *value, const std::string& name)
// {
// 	tf->trace(value, name);
// }

void sc_trace(sc_trace_file *tf, const unsigned char& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned char *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned short& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned short *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned int& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned int *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned long& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned long *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const char& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const char *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const short& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const short *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const int& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const int *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const long& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const long *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const long long& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const long long *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned long long& value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const unsigned long long *value, const std::string& name, int width)
{
	tf->trace(value, name, width);
}

void sc_trace(sc_trace_file *tf, const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width)
{
	tf->trace(signal_in_if, name, width);
}

void sc_trace(sc_trace_file *tf, const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width)
{
	tf->trace(signal_in_if, name, width);
}

void sc_trace(sc_trace_file *tf, const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width)
{
	tf->trace(signal_in_if, name, width);
}

void sc_trace(sc_trace_file *tf, const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width)
{
	tf->trace(signal_in_if, name, width);
}

//////////////////////////////// sc_trace_file ////////////////////////////////

sc_trace_file::sc_trace_file()
{
}

sc_trace_file::~sc_trace_file()
{
}

///////////////////////////// sc_trace_file_base //////////////////////////////

sc_trace_file_base::sc_trace_file_base(const char *name, const char *ext)
	: sc_trace_file()
	, comment()
	, file((std::string(name) + '.' + ext).c_str())
	, time_unit_value(1.0)
	, time_unit_unit(SC_PS)
	, scaling_up(true)
	, scaling(1)
	, curr_time_stamp(0)
	, initialized(false)
{
}

void sc_trace_file_base::set_time_unit(double d, sc_time_unit tu)
{
	double log10_d = log10(d);
	double log10_d_int_part;
    if(modf(log10_d, &log10_d_int_part) != 0.0 )
	{
		throw std::runtime_error("sc_trace_file time unit shall be a power of ten");
	}
	time_unit_value = d;
	time_unit_unit = tu;
}

void sc_trace_file_base::close()
{
	file.close();
}

void sc_trace_file_base::write_comment(const std::string& _comment)
{
	comment = _comment;
}

void sc_trace_file_base::initialize()
{
	double time_unit = 1e-12;
	switch(time_unit_unit)
	{
		case SC_FS: time_unit = time_unit_value * 1e-15; break;
		case SC_PS: time_unit = time_unit_value * 1e-12; break;
		case SC_NS: time_unit = time_unit_value * 1e-9; break;
		case SC_US: time_unit = time_unit_value * 1e-6; break;
		case SC_MS: time_unit = time_unit_value * 1e-3; break;
		case SC_SEC: time_unit = time_unit_value; break;
	}
	
	double time_resolution = sc_get_time_resolution().to_seconds();
	scaling_up = (time_resolution >= time_unit);
	scaling = (scaling_up ? (time_resolution / time_unit) : (time_unit / time_resolution)) + 0.5;
	
	sc_dt::uint64 time_stamp = sc_time_stamp().value();
	curr_time_stamp = scaling_up ? (time_stamp * scaling) : (time_stamp / scaling);
	
	do_initialize();
	initialized = true;
}

bool sc_trace_file_base::advance()
{
	if(!initialized)
	{
		initialize();
	}
	
	sc_dt::uint64 time_stamp = sc_time_stamp().value();
	time_stamp = scaling_up ? (time_stamp * scaling) : (time_stamp / scaling);
	if(time_stamp != curr_time_stamp)
	{
		curr_time_stamp = time_stamp;
		return true;
	}
	return false;
}

sc_dt::uint64 sc_trace_file_base::time_stamp() const
{
	return curr_time_stamp;
}

//////////////////////////////// sc_vcd_module ////////////////////////////////

sc_vcd_module::sc_vcd_module()
	: module_name()
	, children()
	, variables()
{
}

sc_vcd_module::sc_vcd_module(const std::string& _name, sc_vcd_module& parent)
	: module_name(_name)
	, children()
	, variables()
{
	parent.add(*this);
}

sc_vcd_module::~sc_vcd_module()
{
	for(variables_t::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		sc_vcd_variable_base *variable = *it;
		delete variable;
	}
	for(children_t::iterator it = children.begin(); it != children.end(); ++it)
	{
		sc_vcd_module *child = *it;
		delete child;
	}
}

const std::string& sc_vcd_module::name() const
{
	return module_name;
}

void sc_vcd_module::add(sc_vcd_module& child)
{
	children.push_back(&child);
}

void sc_vcd_module::add(sc_vcd_variable_base& variable)
{
	variables.push_back(&variable);
}

sc_vcd_module *sc_vcd_module::find_child(const std::string& child_name) const
{
	for(children_t::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		sc_vcd_module *child = *it;
		if(child->name() == child_name) return child;
	}
	return 0;
}

//////////////////////////// sc_vcd_variable_base /////////////////////////////

sc_vcd_variable_base::sc_vcd_variable_base(const std::string& _name, const std::string& _identifier, const std::string& _type, int _width)
	: var_name(_name)
	, var_identifier(_identifier)
	, var_type(_type)
	, var_width(_width)
{
}

sc_vcd_variable_base::~sc_vcd_variable_base()
{
}

const std::string& sc_vcd_variable_base::name() const
{
	return var_name;
}

const std::string& sc_vcd_variable_base::identifier() const
{
	return var_identifier;
}

const std::string& sc_vcd_variable_base::type() const
{
	return var_type;
}

int sc_vcd_variable_base::width() const
{
	return var_width;
}

void sc_vcd_variable_base::print_decl(std::ostream& stream)
{
	stream << "$var " << var_type << " " << var_width << " " << var_identifier << " " << var_name << " $end" << '\n';
}

/////////////////////////////// sc_vcd_variable<> /////////////////////////////

template <class T>
sc_vcd_variable<T>::sc_vcd_variable(const std::string& _name, const std::string& _identifier, const T& _value)
	: sc_vcd_variable_base(_name, _identifier, sc_vcd_type_trait<T>::type(), sc_vcd_type_trait<T>::width())
	, value(_value)
	, prev_value(_value)
{
}

template <class T>
sc_vcd_variable<T>::sc_vcd_variable(const std::string& _name, const std::string& _identifier, const T& _value, int _width)
	: sc_vcd_variable_base(_name, _identifier, sc_vcd_type_trait<T>::type(), _width)
	, value(_value)
	, prev_value(_value)
{
}

template <class T>
sc_vcd_variable<T>::~sc_vcd_variable()
{
}

template <class T>
bool sc_vcd_variable<T>::value_changed() const
{
	return !(value == prev_value);
}

template <class T>
void sc_vcd_variable<T>::dump(std::ostream& stream)
{
	sc_vcd_type_trait<T>::print(stream, value, identifier());
	prev_value = value;
}

template <class T>
void sc_vcd_variable<T>::print(std::ostream& stream)
{
	sc_vcd_type_trait<T>::print(stream, value, identifier());
	prev_value = value;
}

/////////////////////////////// sc_vcd_trace_file /////////////////////////////

sc_vcd_trace_file::sc_vcd_trace_file(const char *name)
	: sc_trace_file_base(name, "vcd")
	, top()
	, variables()
	, next_variable_identifier()
{
}

sc_vcd_trace_file::~sc_vcd_trace_file()
{
}

sc_vcd_variable_base *sc_vcd_trace_file::find_variable(const std::string& name) const
{
	for(variables_t::const_iterator it = variables.begin(); it != variables.end(); ++it)
	{
		sc_vcd_variable_base *variable = *it;
		if(variable->name() == name) return variable;
	}
	return 0;
}

const std::string& sc_vcd_trace_file::gen_variable_identifier()
{
	const char first = '!';
	const char last = '~';
	std::size_t len = next_variable_identifier.length();
	if(len)
	{
		std::size_t pos = len - 1;
		do
		{
			char c = next_variable_identifier[pos];
			bool extend = (c == last);
			next_variable_identifier[pos] = extend ? first : (c + 1);
			if(!extend) return next_variable_identifier;
			if(!pos)
			{
				next_variable_identifier.insert(0, 1, first);
				return next_variable_identifier;
			}
			--pos;
		}
		while(1);
	}

	return next_variable_identifier = first;
}

void sc_vcd_trace_file::trace(const bool& value, const std::string& name)
{
	trace<bool>(value, name);
}

void sc_vcd_trace_file::trace(const bool* value, const std::string& name)
{
	trace<bool>(*value, name);
}

void sc_vcd_trace_file::trace(const float& value, const std::string& name)
{
	trace<float>(value, name);
}

void sc_vcd_trace_file::trace(const float* value, const std::string& name)
{
	trace<float>(*value, name);
}

void sc_vcd_trace_file::trace(const double& value, const std::string& name)
{
	trace<double>(value, name);
}

void sc_vcd_trace_file::trace(const double* value, const std::string& name)
{
	trace<double>(*value, name);
}

// void sc_vcd_trace_file::trace(const sc_dt::sc_logic& value, const std::string& name)
// {
//	trace<sc_dt::sc_logic>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_logic* value, const std::string& name)
// {
//	trace<sc_dt::sc_logic>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_int_base& value, const std::string& name)
// {
//	trace<sc_dt::sc_int_base>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_int_base* value, const std::string& name)
// {
//	trace<sc_dt::sc_int_base>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_uint_base& value, const std::string& name)
// {
//	trace<sc_dt::sc_uint_base>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_uint_base* value, const std::string& name)
// {
//	trace<sc_dt::sc_uint_base>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_signed& value, const std::string& name)
// {
//	trace<sc_dt::sc_signed>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_signed* value, const std::string& name)
// {
//	trace<sc_dt::sc_signed>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_unsigned& value, const std::string& name)
// {
//	trace<sc_dt::sc_unsigned>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_unsigned* value, const std::string& name)
// {
//	trace<sc_dt::sc_unsigned>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_bv_base& value, const std::string& name)
// {
//	trace<sc_dt::sc_bv_base>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_bv_base* value, const std::string& name)
// {
//	trace<sc_dt::sc_bv_base>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_lv_base& value, const std::string& name)
// {
//	trace<sc_dt::sc_lv_base>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_lv_base* value, const std::string& name)
// {
//	trace<sc_dt::sc_lv_base>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxval& value, const std::string& name)
// {
//	trace<sc_dt::sc_fxval>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxval* value, const std::string& name)
// {
//	trace<sc_dt::sc_fxval>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxval_fast& value, const std::string& name)
// {
//	trace<sc_dt::sc_fxval_fast>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxval_fast* value, const std::string& name)
// {
//	trace<sc_dt::sc_fxval_fast>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxnum& value, const std::string& name)
// {
//	trace<sc_dt::sc_fxnum>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxnum* value, const std::string& name)
// {
//	trace<sc_dt::sc_fxnum>(*value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxnum_fast& value, const std::string& name)
// {
//	trace<sc_dt::sc_fxnum_fast>(value, name);
// }
// 
// void sc_vcd_trace_file::trace(const sc_dt::sc_fxnum_fast* value, const std::string& name)
// {
//	trace<sc_dt::sc_fxnum_fast>(*value, name);
// }

void sc_vcd_trace_file::trace(const unsigned char& value, const std::string& name, int width)
{
	trace<unsigned char>(value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned char *value, const std::string& name, int width)
{
	trace<unsigned char>(*value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned short& value, const std::string& name, int width)
{
	trace<unsigned short>(value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned short *value, const std::string& name, int width)
{
	trace<unsigned short>(*value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned int& value, const std::string& name, int width)
{
	trace<unsigned int>(value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned int *value, const std::string& name, int width)
{
	trace<unsigned int>(*value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned long& value, const std::string& name, int width)
{
	trace<unsigned long>(value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned long *value, const std::string& name, int width)
{
	trace<unsigned long>(*value, name, width);
}

void sc_vcd_trace_file::trace(const char& value, const std::string& name, int width)
{
	trace<char>(value, name, width);
}

void sc_vcd_trace_file::trace(const char *value, const std::string& name, int width)
{
	trace<char>(*value, name, width);
}

void sc_vcd_trace_file::trace(const short& value, const std::string& name, int width)
{
	trace<short>(value, name, width);
}

void sc_vcd_trace_file::trace(const short *value, const std::string& name, int width)
{
	trace<short>(*value, name, width);
}

void sc_vcd_trace_file::trace(const int& value, const std::string& name, int width)
{
	trace<int>(value, name, width);
}

void sc_vcd_trace_file::trace(const int *value, const std::string& name, int width)
{
	trace<int>(*value, name, width);
}

void sc_vcd_trace_file::trace(const long& value, const std::string& name, int width)
{
	trace<long>(value, name, width);
}

void sc_vcd_trace_file::trace(const long *value, const std::string& name, int width)
{
	trace<long>(*value, name, width);
}

void sc_vcd_trace_file::trace(const long long& value, const std::string& name, int width)
{
	trace<long long>(value, name, width);
}

void sc_vcd_trace_file::trace(const long long *value, const std::string& name, int width)
{
	trace<long long>(*value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned long long& value, const std::string& name, int width)
{
	trace<unsigned long long>(value, name, width);
}

void sc_vcd_trace_file::trace(const unsigned long long *value, const std::string& name, int width)
{
	trace<unsigned long long>(*value, name, width);
}

void sc_vcd_trace_file::trace(const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width)
{
	trace<char>(signal_in_if.read(), name, width);
}

void sc_vcd_trace_file::trace(const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width)
{
	trace<short>(signal_in_if.read(), name, width);
}

void sc_vcd_trace_file::trace(const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width)
{
	trace<int>(signal_in_if.read(), name, width);
}

void sc_vcd_trace_file::trace(const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width)
{
	trace<long>(signal_in_if.read(), name, width);
}

void sc_vcd_trace_file::do_initialize()
{
	file << "$version Generated by libieee1666 $end" << '\n';
	if(comment.length())
	{
		file << "$comment " << comment << " $end" << '\n';
	}
	file << "$timescale " << time_unit_value << " " << time_unit_strings[time_unit_unit] << " $end" << '\n';
	
	file << "$scope module TOP $end" << '\n';
	sc_vcd_var_decl_printer vcd_var_decl_printer;
	top.scan_variables<sc_vcd_var_decl_printer, std::ostream, bool>(vcd_var_decl_printer, file);
	top.scan_children<sc_vcd_var_decl_printer, std::ostream, bool>(vcd_var_decl_printer, file);
	file << "$upscope $end" << '\n';
	
	file << "$enddefinitions $end" << '\n';

	file << "$dumpvars" << '\n';
	for(variables_t::iterator it = variables.begin(); it != variables.end(); ++it)
	{
		sc_vcd_variable_base *variable = *it;
		variable->dump(file);
		file << '\n';
	}
	file << "$end" << std::endl;
}

sc_vcd_module& sc_vcd_trace_file::make_module(sc_vcd_module& module, const std::string& leaf_hierarchical_name)
{
	std::size_t hierarchical_delimiter_pos = leaf_hierarchical_name.find_first_of('.');
	std::string child_name = leaf_hierarchical_name.substr(0, hierarchical_delimiter_pos);
	sc_vcd_module *child = module.find_child(child_name);
	if(!child) child = new sc_vcd_module(child_name, module);
	return (hierarchical_delimiter_pos == std::string::npos) ? (*child) : make_module(*child, leaf_hierarchical_name.substr(hierarchical_delimiter_pos + 1));
}

sc_vcd_module& sc_vcd_trace_file::make_module(const std::string& variable_name)
{
	std::size_t hierarchical_delimiter_pos = variable_name.find_last_of('.');
	return (hierarchical_delimiter_pos == std::string::npos) ? top : make_module(top, variable_name.substr(0, hierarchical_delimiter_pos));
}

bool sc_vcd_trace_file::sc_vcd_var_decl_printer::visit_child(sc_vcd_module& child, std::ostream& stream)
{
	stream << "$scope module " << child.name() << " $end" << '\n';
	child.scan_variables<sc_vcd_var_decl_printer, std::ostream, bool>(*this, stream);
	child.scan_children<sc_vcd_var_decl_printer, std::ostream, bool>(*this, stream);
	stream << "$upscope $end" << '\n';
	return false;
}

bool sc_vcd_trace_file::sc_vcd_var_decl_printer::visit_variable(sc_vcd_variable_base& variable, std::ostream& stream)
{
	stream << "$var " << variable.type() << ' ' << variable.width() << ' ' << variable.identifier() << ' ' << variable.name() << " $end" << '\n';
	return false;
}

bool sc_vcd_trace_file::value_changed() const
{
	for(variables_t::const_iterator it = variables.begin(); it != variables.end(); ++it)
	{
		sc_vcd_variable_base *variable = *it;
		if(variable->value_changed()) return true;
	}
	return false;
}

void sc_vcd_trace_file::do_tracing()
{
	if(advance())
	{
		if(value_changed())
		{
			file << '#' << time_stamp() << '\n';
			for(variables_t::iterator it = variables.begin(); it != variables.end(); ++it)
			{
				sc_vcd_variable_base *variable = *it;
				if(variable->value_changed())
				{
					variable->print(file);
				}
				file << '\n';
			}
		}
	}
}

} // end of namespace sc_core
