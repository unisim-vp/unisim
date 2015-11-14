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

#ifndef __IEEE1666_BASE_SIGNAL_H__
#define __IEEE1666_BASE_SIGNAL_H__

#include <ieee1666/kernel/prim_channel.h>
#include <ieee1666/kernel/port.h>
#include <ieee1666/kernel/event.h>
#include <ieee1666/kernel/kernel.h>
#include <ieee1666/base/fwd.h>
#include <ieee1666/base/signal_if.h>
#include <ieee1666/base/signal.h>
#include <iostream>

namespace sc_core {

template <class T, sc_writer_policy WRITER_POLICY = SC_ONE_WRITER>
class sc_signal : public sc_signal_inout_if<T>, public sc_prim_channel
{
public:
	sc_signal();
	explicit sc_signal(const char *);
	virtual ~sc_signal();
	virtual void register_port(sc_port_base&, const char *);
	virtual const T& read() const;
	operator const T& () const;
	virtual sc_writer_policy get_writer_policy() const;
	virtual void write(const T&);
	sc_signal<T,WRITER_POLICY>& operator = (const T&);
	sc_signal<T,WRITER_POLICY>& operator = (const sc_signal<T,WRITER_POLICY>&);
	virtual const sc_event& default_event() const;
	virtual const sc_event& value_changed_event() const;
	virtual bool event() const;
	virtual void print(std::ostream& = std::cout) const;
	virtual void dump(std::ostream& = std::cout) const;
	virtual const char *kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_signal( const sc_signal<T,WRITER_POLICY>& );
	
	sc_port_base *registered_port;
	sc_event signal_value_changed_event;
	sc_dt::uint64 value_changed_delta_cycle;
	unsigned int value_index;
	T value[2];

	T& current_value();
	T& new_value();
	void toggle_value();
};

template <class T, sc_writer_policy WRITER_POLICY>
inline std::ostream& operator<< ( std::ostream&, const sc_signal<T,WRITER_POLICY>& );

template <sc_writer_policy WRITER_POLICY>
class sc_signal<bool,WRITER_POLICY> : public sc_signal_inout_if<bool>, public sc_prim_channel
{
public:
	sc_signal();
	explicit sc_signal( const char* );
	virtual ~sc_signal();
	virtual void register_port( sc_port_base&, const char* );
	virtual const bool& read() const;
	operator const bool& () const;
	virtual sc_writer_policy get_writer_policy() const;
	virtual void write( const bool& );
	sc_signal<bool,WRITER_POLICY>& operator= ( const bool& );
	sc_signal<bool,WRITER_POLICY>& operator= ( const sc_signal<bool,WRITER_POLICY>& );
	virtual const sc_event& default_event() const;
	virtual const sc_event& value_changed_event() const;
	virtual const sc_event& posedge_event() const;
	virtual const sc_event& negedge_event() const;
	virtual bool event() const;
	virtual bool posedge() const;
	virtual bool negedge() const;
	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;
	virtual const char* kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_signal( const sc_signal<bool,WRITER_POLICY>& );
	
};

// template <sc_writer_policy WRITER_POLICY>
// class sc_signal<sc_dt::sc_logic,WRITER_POLICY>
// : public sc_signal_inout_if<sc_dt::sc_logic,WRITER_POLICY>, public sc_prim_channel
// {
// public:
// 	sc_signal();
// 	explicit sc_signal( const char* );
// 	virtual ~sc_signal();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual const sc_dt::sc_logic& read() const;
// 	operator const sc_dt::sc_logic& () const;
// 	virtual void write( const sc_dt::sc_logic& );
// 	sc_signal<sc_dt::sc_logic,WRITER_POLICY>& operator= ( const sc_dt::sc_logic& );
// 	sc_signal<sc_dt::sc_logic,WRITER_POLICY>&
// 	operator= ( const sc_signal<sc_dt::sc_logic,WRITER_POLICY>& );
// 	virtual const sc_event& default_event() const;
// 	virtual const sc_event& value_changed_event() const;
// 	virtual const sc_event& posedge_event() const;
// 	virtual const sc_event& negedge_event() const;
// 	virtual bool event() const;
// 	virtual bool posedge() const;
// 	virtual bool negedge() const;
// 	virtual void print( std::ostream& = std::cout ) const;
// 	virtual void dump( std::ostream& = std::cout ) const;
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal( const sc_signal<sc_dt::sc_logic,WRITER_POLICY>& );
// };
// 
// class sc_signal_resolved : public sc_signal<sc_dt::sc_logic,SC_MANY_WRITERS>
// {
// public:
// 	sc_signal_resolved();
// 	explicit sc_signal_resolved( const char* );
// 	virtual ~sc_signal_resolved();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual void write( const sc_dt::sc_logic& );
// 	sc_signal_resolved& operator= ( const sc_dt::sc_logic& );
// 	sc_signal_resolved& operator= ( const sc_signal_resolved& );
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal_resolved( const sc_signal_resolved& );
// };
// 
// template <int W>
// class sc_signal_rv : public sc_signal<sc_dt::sc_lv<W>,SC_MANY_WRITERS>
// {
// public:
// 	sc_signal_rv();
// 	explicit sc_signal_rv( const char* );
// 	virtual ~sc_signal_rv();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual void write( const sc_dt::sc_lv<W>& );
// 	sc_signal_rv<W>& operator= ( const sc_dt::sc_lv<W>& );
// 	sc_signal_rv<W>& operator= ( const sc_signal_rv<W>& );
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal_rv( const sc_signal_rv<W>& );
// };

/////////////////////////////////// sc_signal<> /////////////////////////////////////////////


template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal()
	: sc_prim_channel(IEEE1666_KERNEL_PREFIX "_signal")
	, signal_value_changed_event(IEEE1666_KERNEL_PREFIX "_value_changed_event")
	, value_changed_delta_cycle(0x7fffffffffffffffULL)
	, value_index(0)
	, value()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal(const char* _name)
	: sc_prim_channel(_name)
	, signal_value_changed_event(IEEE1666_KERNEL_PREFIX "_signal_value_changed_event")
	, value_index(0)
	, value()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::~sc_signal()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::register_port(sc_port_base& port, const char *if_typename)
{
	if(std::string(typeid(sc_signal_inout_if<T>).name()).compare(if_typename) != 0)
	{
		throw std::runtime_error("sc_signal is not bound to sc_signal_inout_if<T>");
	}
	
	if((WRITER_POLICY == SC_ONE_WRITER) && (registered_port != &port))
	{
		throw std::runtime_error("sc_signal can't be bound to more than one port");
	}
	
	registered_port = &port;
}

template <class T, sc_writer_policy WRITER_POLICY>
const T& sc_signal<T, WRITER_POLICY>::read() const
{
	return current_value();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::operator const T& () const
{
	return read();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_writer_policy sc_signal<T, WRITER_POLICY>::get_writer_policy() const
{
	return WRITER_POLICY;
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::write(const T& v)
{
	bool value_changed = !(v == current_value());
	
	if(value_changed)
	{
		new_value() = v;
		request_update();
	}
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T,WRITER_POLICY>& sc_signal<T, WRITER_POLICY>::operator = (const T& v)
{
	write(v);
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T,WRITER_POLICY>& sc_signal<T, WRITER_POLICY>::operator = (const sc_signal<T,WRITER_POLICY>& s)
{
	write(s.read());
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<T, WRITER_POLICY>::default_event() const
{
	return signal_value_changed_event;
}

template <class T, sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<T, WRITER_POLICY>::value_changed_event() const
{
	return signal_value_changed_event;
}

template <class T, sc_writer_policy WRITER_POLICY>
bool sc_signal<T, WRITER_POLICY>::event() const
{
	return value_changed_delta_cycle == kernel->get_delta_count();
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::print(std::ostream& os) const
{
	os << *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::dump(std::ostream& os) const
{
	os << name() << ": kind=\"" << kind() << "\", current_value=" << current_value() << ", new_value=" << new_value();
}

template <class T, sc_writer_policy WRITER_POLICY>
const char* sc_signal<T, WRITER_POLICY>::kind() const
{
	return "sc_signal";
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::update()
{
	toggle_value();
	value_changed_delta_cycle = kernel->get_delta_count();
	value_changed_event.notify(SC_ZERO_TIME);
}

// disabled
template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal( const sc_signal<T,WRITER_POLICY>& )
{
}

template <class T, sc_writer_policy WRITER_POLICY>
T& sc_signal<T, WRITER_POLICY>::current_value()
{
	return value[value_index];
}

template <class T, sc_writer_policy WRITER_POLICY>
T& sc_signal<T, WRITER_POLICY>::new_value()
{
	return value[value_index ^ 1];
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::toggle_value()
{
	value_index ^= 1;
}

template <class T, sc_writer_policy WRITER_POLICY>
inline std::ostream& operator << (std::ostream& os, const sc_signal<T,WRITER_POLICY>& s)
{
	return os << s.current_value();
}

///////////////////////////// sc_signal<bool,WRITER_POLICY> ///////////////////////////////////

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal()
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal( const char* )
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::~sc_signal()
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::register_port( sc_port_base&, const char* )
{
}

template <sc_writer_policy WRITER_POLICY>
const bool& sc_signal<bool, WRITER_POLICY>::read() const
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::operator const bool& () const
{
}

template <sc_writer_policy WRITER_POLICY>
sc_writer_policy sc_signal<bool, WRITER_POLICY>::get_writer_policy() const
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::write( const bool& )
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool,WRITER_POLICY>& sc_signal<bool, WRITER_POLICY>::operator= ( const bool& )
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool,WRITER_POLICY>& sc_signal<bool, WRITER_POLICY>::operator= ( const sc_signal<bool,WRITER_POLICY>& )
{
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::default_event() const
{
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::value_changed_event() const
{
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::posedge_event() const
{
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::negedge_event() const
{
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::event() const
{
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::posedge() const
{
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::negedge() const
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::print( std::ostream& ) const
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::dump( std::ostream& ) const
{
}

template <sc_writer_policy WRITER_POLICY>
const char* sc_signal<bool, WRITER_POLICY>::kind() const
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::update()
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal( const sc_signal<bool,WRITER_POLICY>& )
{
}

} // end of namespace sc_core

#endif
