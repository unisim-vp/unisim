/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_UTIL_GARBAGE_COLLECTOR_GARBAGE_COLLECTOR_HH__
#define __UNISIM_UTIL_GARBAGE_COLLECTOR_GARBAGE_COLLECTOR_HH__

#include <iostream>
#include <stdlib.h>

namespace unisim {
namespace util {
namespace garbage_collector {

class PointerBase { };

} // end of namespace garbage_collector
} // end of namespace util
} // end of namespace unisim

void *operator new(std::size_t size, unisim::util::garbage_collector::PointerBase);
void *operator new[](std::size_t size, unisim::util::garbage_collector::PointerBase);

namespace unisim {
namespace util {
namespace garbage_collector {

using std::cerr;
using std::endl;	
	
struct MemoryBlock
{
	unsigned int magic;
	unsigned int refcount;
	unsigned int size;
	MemoryBlock *next;
};

class GarbageCollector
{
public:
	static void Reset();

	static /* inline */ bool Collect(void *p)
#if 0 // defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	static /* inline */ void *Allocate(unsigned int size)
#if 0 // defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	static /* inline */ void Catch(void *p)
#if 0 // defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;
	
private:
	static bool Setup();
	static void AtExit();
	static const unsigned int MIN_MEMORY_BLOCK_SIZE = 32;
	static const unsigned int MAX_MEMORY_BLOCK_SIZE = 32 * MIN_MEMORY_BLOCK_SIZE;
	static const unsigned int MAGIC = 0x5f47435f; // '_GC_';
	static bool done_setup;
	
	static MemoryBlock *free_blocks[MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE];
};

template <class T>
class Pointer : public PointerBase
{
public:
	inline Pointer()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline Pointer(const Pointer<T>& ptr)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline Pointer(T *_obj)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline ~Pointer()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline bool operator == (const Pointer<T>& p) const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

// 	inline operator void * () const
// #if defined(__GNUC__) && (__GNUC__ >= 3)
// __attribute__((always_inline))
// #endif
// 	;

// 	inline T* operator * () const
// #if defined(__GNUC__) && (__GNUC__ >= 3)
// __attribute__((always_inline))
// #endif
// 	;

	inline operator T * () const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline T *operator -> () const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline Pointer<T>& operator = (T *_obj)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline Pointer<T>& operator = (const Pointer<T>& ptr)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;
	
	inline T& operator [](unsigned int index)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

private:
	T *obj;
};

template <class T>
inline Pointer<T>::Pointer() : obj(0)
{
}

template <class T>
inline Pointer<T>::Pointer(const Pointer<T>& ptr) : obj(ptr.obj)
{
	if(obj)
		GarbageCollector::Catch(obj);
}

template <class T>
inline Pointer<T>::Pointer(T *_obj) : obj(_obj)
{
	if(obj)
		GarbageCollector::Catch(obj);
}

template <class T>
inline Pointer<T>::~Pointer()
{
	if(obj)
	{
		if(GarbageCollector::Collect(obj))
			obj->~T();
	}
}

template <class T>
inline bool Pointer<T>::operator == (const Pointer<T>& p) const
{
	return obj == p.obj;
}

// template <class T>
// inline Pointer<T>::operator void * () const
// {
// 	return obj;
// }

// template <class T>
// inline T* Pointer<T>::operator * () const
// {
// 	return obj;
// }

template <class T>
inline Pointer<T>::operator T * () const
{
	return obj;
}

template <class T>
inline T *Pointer<T>::operator -> () const
{
	return obj;
}

template <class T>
inline Pointer<T>& Pointer<T>::operator = (T *_obj)
{
	if(obj)
	{
		if(GarbageCollector::Collect(obj))
			obj->~T();
	}

	obj = _obj;
	
	if(obj)
		GarbageCollector::Catch(obj);
}

template <class T>
inline Pointer<T>& Pointer<T>::operator = (const Pointer<T>& ptr)
{
	if(obj)
	{
		if(GarbageCollector::Collect(obj))
			obj->~T();
	}
	
	obj = ptr.obj;
	
	if(obj)
		GarbageCollector::Catch(obj);
}

template <class T>
inline T& Pointer<T>::operator [](unsigned int index) {
	return obj[index];
} 

} // end of namespace garbage_collector
} // end of namespace util
} // end of namespace unisim

#endif
