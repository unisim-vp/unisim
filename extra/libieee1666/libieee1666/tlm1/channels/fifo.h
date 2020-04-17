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

#ifndef __LIBIEEE1666_TLM1_CHANNELS_FIFO_H__
#define __LIBIEEE1666_TLM1_CHANNELS_FIFO_H__

#include <tlm1/fwd.h>
#include <systemc>

namespace tlm {

//////////////////////////////// declaration //////////////////////////////////

template <typename T>
class tlm_fifo
	: public virtual tlm_fifo_get_if<T>
	, public virtual tlm_fifo_put_if<T>
	, public sc_core::sc_prim_channel
{
public:
	explicit tlm_fifo(int size_ = 1);
	explicit tlm_fifo(const char *name_, int size_ = 1);
	virtual ~tlm_fifo();
	
	T get(tlm_tag<T> *t = 0);
	bool nb_get(T&);
	bool nb_can_get(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event &ok_to_get(tlm_tag<T> *t = 0) const;
	T peek(tlm_tag<T> *t = 0) const;
	bool nb_peek(T&) const;
	bool nb_can_peek(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event& ok_to_peek(tlm_tag<T> *t = 0) const;
	void put(const T&);
	bool nb_put(const T&);
	bool nb_can_put(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event& ok_to_put(tlm_tag<T> *t = 0) const;
	void nb_expand(unsigned int n = 1);
	void nb_unbound(unsigned int n = 16);
	bool nb_reduce(unsigned int n = 1);
	bool nb_bound(unsigned int n);
	bool nb_peek(T &, int n) const;
	bool nb_poke(const T&, int n = 0);
	int used() const;
	int size() const;
	void debug() const;
	const char *kind() const;
protected:
	virtual void update();
private:
	enum
	{
		BOUNDED   = 0,
		UNBOUNDED = 1
	}
	type;
	
	unsigned int buffer_capacity;
	unsigned int fifo_size;
	unsigned int fifo_used;
	unsigned int free_count;
	unsigned int available_count;
	unsigned int read_count;
	unsigned int write_count;
	unsigned int write_idx;
	unsigned int read_idx;

	T *buffer;

	sc_core::sc_event fifo_data_written_event;
	sc_core::sc_event fifo_data_read_event;
	bool resized;
	
	void grow_buffer(unsigned int n);
	void expand(unsigned int n);
};

///////////////////////////////// definition //////////////////////////////////

template <typename T>
tlm_fifo<T>::tlm_fifo(int _size)
	: sc_prim_channel(sc_core::sc_gen_unique_name("fifo"))
	, type((_size < 0) ? UNBOUNDED : BOUNDED)
	, buffer_capacity(0)
	, fifo_size((type == BOUNDED) ? _size : -_size)
	, fifo_used(0)
	, free_count(fifo_size)
	, available_count(0)
	, read_count(0)
	, write_count(0)
	, write_idx(0)
	, read_idx(0)
	, buffer(0)
	, fifo_data_written_event(sc_core::sc_gen_unique_name("tlm_fifo_data_written_event"))
	, fifo_data_read_event(sc_core::sc_gen_unique_name("tlm_fifo_data_written_event"))
{
	grow_buffer(fifo_size);
}

template <typename T>
tlm_fifo<T>::tlm_fifo(const char *_name, int _size)
	: sc_prim_channel(name)
	, type((_size < 0) ? UNBOUNDED : BOUNDED)
	, buffer_capacity(0)
	, fifo_size((type == BOUNDED) ? _size : -_size)
	, fifo_used(0)
	, free_count(fifo_size)
	, available_count(0)
	, read_count(0)
	, write_count(0)
	, write_idx(0)
	, read_idx(0)
	, buffer(0)
	, fifo_data_written_event(sc_core::sc_gen_unique_name("tlm_fifo_data_written_event"))
	, fifo_data_read_event(sc_core::sc_gen_unique_name("tlm_fifo_data_written_event"))
{
	grow_buffer(fifo_size);
}

template <typename T>
tlm_fifo<T>::~tlm_fifo()
{
	if(buffer)
	{
		delete[] buffer;
	}
}

template <typename T>
T tlm_fifo<T>::get(tlm_tag<T> *t)
{
	if(available_count == 0)
	{
		do
		{
			sc_core::wait(fifo_data_written_event);
		}
		while(available_count == 0);
	}
	T v = buffer[read_idx++];
	if(read_idx >= buffer_capacity) read_idx = 0;
	read_count++;
	available_count--;

	request_update();
	
	return v;
}

template <typename T>
bool tlm_fifo<T>::nb_get(T& v)
{
	if(available_count == 0) return false;
		
	v = buffer[read_idx++];
	if(read_idx >= buffer_capacity) read_idx = 0;
	read_count++;
	available_count--;

	request_update();
	
	return true;
}

template <typename T>
bool tlm_fifo<T>::nb_can_get(tlm_tag<T> *t) const
{
	return (available_count == 0);
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_get(tlm_tag<T> *t) const
{
	return fifo_data_written_event;
}

template <typename T>
T tlm_fifo<T>::peek(tlm_tag<T> *t) const
{
	if(available_count == 0) return false;
		
	return buffer[read_idx];
}

template <typename T>
bool tlm_fifo<T>::nb_peek(T& v) const
{
	if(available_count == 0) return false;
		
	v = buffer[read_idx];
	
	return true;
}

template <typename T>
bool tlm_fifo<T>::nb_can_peek(tlm_tag<T> *t) const
{
	return (available_count == 0);
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_peek(tlm_tag<T> *t) const
{
	return fifo_data_written_event;
}

template <typename T>
void tlm_fifo<T>::grow_buffer(unsigned int n)
{
	if(n)
	{
		unsigned int old_buffer_capacity = buffer_capacity;
		T *old_buffer = buffer;
		buffer_capacity += n;
		buffer = new T[buffer_capacity];
		if(old_buffer)
		{
			for(unsigned int i = 0; i < old_buffer_capacity; i++)
			{
				buffer[i] = old_buffer[i];
			}
			delete old_buffer;
		}
	}
}

template <typename T>
void tlm_fifo<T>::expand(unsigned int n)
{
	unsigned int new_fifo_size = fifo_size + n;
	if(buffer_capacity < new_fifo_size)
	{
		unsigned int m = new_fifo_size - buffer_capacity;
		grow_buffer((m > 16) ? m : 16);
	}
	fifo_size = new_fifo_size;
	free_count += n;
}

template <typename T>
void tlm_fifo<T>::put(const T& v)
{
	if(free_count == 0)
	{
		if(type == BOUNDED)
		{
			do
			{
				sc_core::wait(fifo_data_read_event);
			}
			while(free_count == 0);
		}
		else
		{
			expand(1);
		}
	}
	buffer[write_idx++] = v;
	if(write_idx >= buffer_capacity) write_idx = 0;
	write_count++;
	free_count--;
	
	request_update();
}

template <typename T>
bool tlm_fifo<T>::nb_put(const T& v)
{
	if(free_count == 0)
	{
		if(type == BOUNDED) return false;
		
		expand(1);
	}

	buffer[write_idx++] = v;
	if(write_idx >= buffer_capacity) write_idx = 0;
	write_count++;
	free_count--;
	
	request_update();
	
	return true;
}

template <typename T>
bool tlm_fifo<T>::nb_can_put(tlm_tag<T> *t) const
{
	return (type == UNBOUNDED) || (free_count != 0);
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_put(tlm_tag<T> *t) const
{
	return fifo_data_read_event;
}

template <typename T>
void tlm_fifo<T>::nb_expand(unsigned int n)
{
	if(type == BOUNDED)
	{
		expand(n);
	}
	resized = true;
	request_update();
}

template <typename T>
void tlm_fifo<T>::nb_unbound(unsigned int n)
{
	if(type == BOUNDED && (n > fifo_size))
	{
		expand(fifo_size - n);
	}
	type = UNBOUNDED;
	resized = true;
	request_update();
}

template <typename T>
bool tlm_fifo<T>::nb_reduce(unsigned int n)
{
	if(type == UNBOUNDED) return false;
	
	bool r = (fifo_size >= n);
	fifo_size = n;
	return r;
}

template <typename T>
bool tlm_fifo<T>::nb_bound(unsigned int n)
{
	type = BOUNDED;
	return nb_reduce(n);
}

template <typename T>
bool tlm_fifo<T>::nb_peek(T& v, int n) const
{
	if(n >= fifo_used) return false;
	int s = buffer_capacity - read_idx;
	int i = (n >= s) ? (n - s) : (read_idx + n);
	v = buffer[i];
	return true;
}

template <typename T>
bool tlm_fifo<T>::nb_poke(const T& v, int n)
{
	if(n >= fifo_used) return false;
	int s = buffer_capacity - write_idx;
	int i = (n >= s) ? (n - s) : (write_idx + n);
	buffer[i] = v;
	return true;
}

template <typename T>
int tlm_fifo<T>::used() const
{
	return fifo_used;
}

template <typename T>
int tlm_fifo<T>::size() const
{
	return (type == BOUNDED) ? fifo_size : -1;
}

template <typename T>
void tlm_fifo<T>::debug() const
{
	std::cout << "- buffer_capacity:" << buffer_capacity << std::endl;
	std::cout << "- size:" << fifo_size << std::endl;
	std::cout << "- used:" << fifo_used << std::endl;
	std::cout << "- free_count:" << free_count << std::endl;
	std::cout << "- available_count:" << available_count << std::endl;
	std::cout << "- read_count:" << read_count << std::endl;
	std::cout << "- write_count:" << write_count << std::endl;
	std::cout << "- write_idx:" << write_idx << std::endl;
	std::cout << "- read_idx:" << read_idx << std::endl;
}

template <typename T>
const char *tlm_fifo<T>::kind() const
{
	return "tlm_fifo";
}

template <class T>
void tlm_fifo<T>::update()
{
	if(write_count)
	{
		fifo_data_written_event.notify(sc_core::SC_ZERO_TIME);
	}
	if(read_count || resized)
	{
		fifo_data_read_event.notify(sc_core::SC_ZERO_TIME);
	}
	fifo_used += write_count;
	fifo_used -= read_count;
	available_count = fifo_used;
	write_count = 0;
	read_count = 0;
	resized = false;
}

} // end of namespace tlm

#endif
