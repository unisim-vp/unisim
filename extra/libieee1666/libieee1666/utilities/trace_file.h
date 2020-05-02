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

#ifndef __LIBIEEE1666_UTILITIES_TRACE_FILE_H__
#define __LIBIEEE1666_UTILITIES_TRACE_FILE_H__

#include "core/time.h"
#include "channels/signal_if.h"
#include "utilities/fwd.h"
#include <string>
#include <fstream>

namespace sc_core {

class sc_trace_file
{
public:
	virtual void set_time_unit(double d, sc_time_unit tu) = 0;
	//implementation-defined
protected:
	friend class sc_kernel;
	friend sc_trace_file *sc_create_vcd_trace_file(const char *name);
	friend void sc_close_vcd_trace_file(sc_trace_file *tf);
	friend void sc_write_comment(sc_trace_file *tf, const std::string& comment);
	friend void sc_trace(sc_trace_file *tf, const bool& value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const bool *value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const float& value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const float *value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const double& value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const double *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum *value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast& value, const std::string& name);
//	friend void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast *value, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const unsigned char& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned char *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned short& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned short *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned int& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned int *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned long& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned long *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const char& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const char *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const short& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const short *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const int& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const int *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const long& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const long *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const long long& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const long long *value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned long long& value, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const unsigned long long *value, const std::string& name, int width);
	template <class T> friend void sc_trace(sc_trace_file *tf, const sc_signal_in_if<T>& signal_in_if, const std::string& name);
	friend void sc_trace(sc_trace_file *tf, const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width);
	friend void sc_trace(sc_trace_file *tf, const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width);

	sc_trace_file();
	virtual ~sc_trace_file();
	virtual void close() = 0;
	virtual void write_comment(const std::string& comment) = 0;
	virtual void trace(const bool& value, const std::string& name) = 0;
	virtual void trace(const bool* value, const std::string& name) = 0;
	virtual void trace(const float& value, const std::string& name) = 0;
	virtual void trace(const float* value, const std::string& name) = 0;
	virtual void trace(const double& value, const std::string& name) = 0;
	virtual void trace(const double* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_logic& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_logic* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_int_base& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_int_base* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_uint_base& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_uint_base* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_signed& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_signed* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_unsigned& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_unsigned* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_bv_base& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_bv_base* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_lv_base& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_lv_base* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxval& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxval* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxval_fast& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxval_fast* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxnum& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxnum* value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxnum_fast& value, const std::string& name) = 0;
//	virtual void trace(const sc_dt::sc_fxnum_fast* value, const std::string& name) = 0;
	virtual void trace(const unsigned char& value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned char *value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned short& value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned short *value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned int& value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned int *value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned long& value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned long *value, const std::string& name, int width) = 0;
	virtual void trace(const char& value, const std::string& name, int width) = 0;
	virtual void trace(const char *value, const std::string& name, int width) = 0;
	virtual void trace(const short& value, const std::string& name, int width) = 0;
	virtual void trace(const short *value, const std::string& name, int width) = 0;
	virtual void trace(const int& value, const std::string& name, int width) = 0;
	virtual void trace(const int *value, const std::string& name, int width) = 0;
	virtual void trace(const long& value, const std::string& name, int width) = 0;
	virtual void trace(const long *value, const std::string& name, int width) = 0;
	virtual void trace(const long long& value, const std::string& name, int width) = 0;
	virtual void trace(const long long *value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned long long& value, const std::string& name, int width) = 0;
	virtual void trace(const unsigned long long *value, const std::string& name, int width) = 0;
	virtual void trace(const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width) = 0;
	virtual void trace(const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width) = 0;
	virtual void trace(const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width) = 0;
	virtual void trace(const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width) = 0;
	
	virtual void initialize() = 0;
	virtual void do_tracing() = 0;
};

class sc_vcd_variable_base
{
public:
	sc_vcd_variable_base(const std::string& name, const std::string& identifier, const std::string& type, int width);
	virtual ~sc_vcd_variable_base();
	const std::string& name() const;
	const std::string& identifier() const;
	const std::string& type() const;
	int width() const;
	void print_decl(std::ostream& stream);
	virtual bool value_changed() const = 0;
	virtual void dump(std::ostream& stream) = 0;
	virtual void print(std::ostream& stream) = 0;
private:
	std::string var_name;
	std::string var_identifier;
	std::string var_type;
	int var_width;
};

template <class T>
class sc_vcd_variable : public sc_vcd_variable_base
{
public:
	sc_vcd_variable(const std::string& name, const std::string& identifier, const T& value);
	sc_vcd_variable(const std::string& name, const std::string& identifier, const T& value, int width);
	virtual ~sc_vcd_variable();
	virtual bool value_changed() const;
	virtual void dump(std::ostream& stream);
	virtual void print(std::ostream& stream);
private:
	const T& value;
	T prev_value;
};

class sc_vcd_trace_file : public sc_trace_file
{
public:
	virtual void set_time_unit(double d, sc_time_unit tu);
protected:
	friend class sc_kernel;
	
	double timescale_value;
	sc_time_unit timescale_unit;
	bool scaling_up;
	sc_dt::uint64 scaling;
	sc_dt::uint64 curr_time_stamp;
	bool initialized;
	
	typedef std::vector<sc_vcd_variable_base *> variables_t;
	variables_t variables;
	std::string next_variable_identifier;
	std::string comment;
	std::ofstream file;
	
	sc_vcd_trace_file(const char *name);
	virtual ~sc_vcd_trace_file();
	sc_vcd_variable_base *find_variable(const std::string& name) const;
	const std::string& gen_variable_identifier();
	bool value_changed() const;
	template <class T> void trace(const T& value, const std::string& name);
	template <class T> void trace(const T& value, const std::string& name, int width);
	virtual void close();
	virtual void write_comment(const std::string& comment);
	virtual void trace(const bool& value, const std::string& name);
	virtual void trace(const bool* value, const std::string& name);
	virtual void trace(const float& value, const std::string& name);
	virtual void trace(const float* value, const std::string& name);
	virtual void trace(const double& value, const std::string& name);
	virtual void trace(const double* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_logic& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_logic* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_int_base& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_int_base* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_uint_base& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_uint_base* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_signed& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_signed* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_unsigned& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_unsigned* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_bv_base& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_bv_base* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_lv_base& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_lv_base* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxval& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxval* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxval_fast& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxval_fast* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxnum& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxnum* value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxnum_fast& value, const std::string& name);
//	virtual void trace(const sc_dt::sc_fxnum_fast* value, const std::string& name);
	virtual void trace(const unsigned char& value, const std::string& name, int width);
	virtual void trace(const unsigned char *value, const std::string& name, int width);
	virtual void trace(const unsigned short& value, const std::string& name, int width);
	virtual void trace(const unsigned short *value, const std::string& name, int width);
	virtual void trace(const unsigned int& value, const std::string& name, int width);
	virtual void trace(const unsigned int *value, const std::string& name, int width);
	virtual void trace(const unsigned long& value, const std::string& name, int width);
	virtual void trace(const unsigned long *value, const std::string& name, int width);
	virtual void trace(const char& value, const std::string& name, int width);
	virtual void trace(const char *value, const std::string& name, int width);
	virtual void trace(const short& value, const std::string& name, int width);
	virtual void trace(const short *value, const std::string& name, int width);
	virtual void trace(const int& value, const std::string& name, int width);
	virtual void trace(const int *value, const std::string& name, int width);
	virtual void trace(const long& value, const std::string& name, int width);
	virtual void trace(const long *value, const std::string& name, int width);
	virtual void trace(const long long& value, const std::string& name, int width);
	virtual void trace(const long long *value, const std::string& name, int width);
	virtual void trace(const unsigned long long& value, const std::string& name, int width);
	virtual void trace(const unsigned long long *value, const std::string& name, int width);
	virtual void trace(const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width);
	virtual void trace(const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width);
	virtual void trace(const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width);
	virtual void trace(const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width);

	virtual void initialize();
	virtual void do_tracing();
};

sc_trace_file *sc_create_vcd_trace_file(const char *name);
void sc_close_vcd_trace_file(sc_trace_file *tf);
void sc_write_comment(sc_trace_file *tf, const std::string& comment);
void sc_trace(sc_trace_file *tf, const bool& value, const std::string& name);
void sc_trace(sc_trace_file *tf, const bool *value, const std::string& name);
void sc_trace(sc_trace_file *tf, const float& value, const std::string& name);
void sc_trace(sc_trace_file *tf, const float *value, const std::string& name);
void sc_trace(sc_trace_file *tf, const double& value, const std::string& name);
void sc_trace(sc_trace_file *tf, const double *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_logic *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_int_base *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_uint_base *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_signed *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_unsigned *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_bv_base *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_lv_base *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxval_fast *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum *value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast& value, const std::string& name);
// void sc_trace(sc_trace_file *tf, const sc_dt::sc_fxnum_fast *value, const std::string& name);
void sc_trace(sc_trace_file *tf, const unsigned char& value, const std::string& name, int width = 8 * sizeof(unsigned char));
void sc_trace(sc_trace_file *tf, const unsigned char *value, const std::string& name, int width = 8 * sizeof(unsigned char));
void sc_trace(sc_trace_file *tf, const unsigned short& value, const std::string& name, int width = 8 * sizeof(unsigned short));
void sc_trace(sc_trace_file *tf, const unsigned short *value, const std::string& name, int width = 8 * sizeof(unsigned short));
void sc_trace(sc_trace_file *tf, const unsigned int& value, const std::string& name, int width = 8 * sizeof(unsigned int));
void sc_trace(sc_trace_file *tf, const unsigned int *value, const std::string& name, int width = 8 * sizeof(unsigned int));
void sc_trace(sc_trace_file *tf, const unsigned long& value, const std::string& name, int width = 8 * sizeof(unsigned long));
void sc_trace(sc_trace_file *tf, const unsigned long *value, const std::string& name, int width = 8 * sizeof(unsigned long));
void sc_trace(sc_trace_file *tf, const char& value, const std::string& name, int width = 8 * sizeof(char));
void sc_trace(sc_trace_file *tf, const char *value, const std::string& name, int width = 8 * sizeof(char));
void sc_trace(sc_trace_file *tf, const short& value, const std::string& name, int width = 8 * sizeof(short));
void sc_trace(sc_trace_file *tf, const short *value, const std::string& name, int width = 8 * sizeof(short));
void sc_trace(sc_trace_file *tf, const int& value, const std::string& name, int width = 8 * sizeof(int));
void sc_trace(sc_trace_file *tf, const int *value, const std::string& name, int width = 8 * sizeof(int));
void sc_trace(sc_trace_file *tf, const long& value, const std::string& name, int width = 8 * sizeof(long));
void sc_trace(sc_trace_file *tf, const long *value, const std::string& name, int width = 8 * sizeof(long));
void sc_trace(sc_trace_file *tf, const long long& value, const std::string& name, int width = 8 * sizeof(long long));
void sc_trace(sc_trace_file *tf, const long long *value, const std::string& name, int width = 8 * sizeof(long long));
void sc_trace(sc_trace_file *tf, const unsigned long long& value, const std::string& name, int width = 8 * sizeof(unsigned long long));
void sc_trace(sc_trace_file *tf, const unsigned long long *value, const std::string& name, int width = 8 * sizeof(unsigned long long));
template <class T>
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<T>& signal_in_if, const std::string& name);
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<char>& signal_in_if, const std::string& name, int width);
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<short>& signal_in_if, const std::string& name, int width);
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<int>& signal_in_if, const std::string& name, int width);
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<long>& signal_in_if, const std::string& name, int width);

template <class T>
void sc_trace(sc_trace_file *tf, const sc_signal_in_if<T>& signal_in_if, const std::string& name)
{
	sc_trace(tf, signal_in_if.read(), name);
}

template <class T>
void sc_vcd_trace_file::trace(const T& value, const std::string& name)
{
	if(!find_variable(name))
	{
		variables.push_back(new sc_vcd_variable<T>(name, gen_variable_identifier(), value));
	}
}

template <class T>
void sc_vcd_trace_file::trace(const T& value, const std::string& name, int width)
{
	if(!find_variable(name))
	{
		variables.push_back(new sc_vcd_variable<T>(name, gen_variable_identifier(), value, width));
	}
}

} // end of namespace sc_core

#endif
