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

#ifndef __LIBIEEE1666_UTILITIES_VECTOR_H__
#define __LIBIEEE1666_UTILITIES_VECTOR_H__

#include "core/fwd.h"
#include "utilities/fwd.h"
#include <stdexcept>

namespace sc_core {

class sc_vector_base : public sc_object
{
public:
	typedef int size_type; // implementation-defined

	virtual const char* kind() const;
	size_type size() const;
	const std::vector<sc_object*>& get_elements() const;
};

template< typename T >
class sc_vector_iter : public std::iterator< std::random_access_iterator_tag, T >
{
// Conforms to Random Access Iterator category.
// See ISO/IEC 14882:2003(E), 24.1 [lib.iterator.requirements]
// implementation-defined
};

template< typename T >
class sc_vector : public sc_vector_base
{
public:
	using sc_vector_base::size_type;
	typedef sc_vector_iter<T> iterator;
	typedef sc_vector_iter<const T> const_iterator;
	sc_vector();
	explicit sc_vector( const char* );
	sc_vector( const char* , size_type );
	template< typename Creator >
	sc_vector( const char* , size_type , Creator );
	virtual ~sc_vector();
	void init( size_type );
	static T* create_element( const char* , size_type );
	template< typename Creator >
	void init( size_type , Creator );
	T& operator[] ( size_type );
	const T& operator[] ( size_type ) const;
	T& at( size_type );
	const T& at( size_type ) const;
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	template< typename ContainerType, typename ArgumentType >
	iterator bind( sc_vector_assembly<ContainerType,ArgumentType> );
	template< typename BindableContainer >
	iterator bind( BindableContainer& );
	template< typename BindableIterator >
	iterator bind( BindableIterator , BindableIterator );
	template< typename BindableIterator >
	iterator bind( BindableIterator , BindableIterator , iterator );
	template< typename ContainerType, typename ArgumentType >
	iterator operator() ( sc_vector_assembly<ContainerType,ArgumentType>template< typename ArgumentContainer >
	iterator operator() ( ArgumentContainer& );
	template< typename ArgumentIterator >
	iterator operator() ( ArgumentIterator , ArgumentIterator );
	template< typename ArgumentIterator >
	iterator operator() ( ArgumentIterator , ArgumentIterator , iterator );
private:
	// Disabled
	sc_vector( const sc_vector& );
	sc_vector& operator= ( const sc_vector& );
};

template< typename T, typename MT >
class sc_vector_assembly
{
public:
	typedef int size_type; // implementation-defined
	typedef int iterator;  // implementation-defined
	typedef int const_iterator implementation-defined;
	typedef MT (T::*member_type);
	sc_vector_assembly( const sc_vector_assembly& );
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	size_type size() const;
	std::vector< sc_object* > get_elements() const;
	iterator::reference operator[] ( size_type );
	const_iterator::reference operator[] ( size_type ) const;
	iterator::reference at( size_type );
	const_iterator::reference at( size_type ) const;

	template< typename ContainerType, typename ArgumentType >
	iterator bind( sc_vector_assembly<ContainerType, ArgumentType> );

	template< typename BindableContainer >
	iterator bind( BindableContainer& );

	template< typename BindableIterator >
	iterator bind( BindableIterator , BindableIterator );

	template< typename BindableIterator >
	iterator bind( BindableIterator , BindableIterator , iterator );

	template< typename BindableIterator >
	iterator bind( BindableIterator , BindableIterator , sc_vector<T>::iterator );

	template< typename ContainerType, typename ArgumentType >
	iterator operator() ( sc_vector_assembly<ContainerType, ArgumentType> );

	template< typename ArgumentContainer >
	iterator operator() ( ArgumentContainer& );

	template< typename ArgumentIterator >
	iterator operator() ( ArgumentIterator , ArgumentIterator );

	template< typename ArgumentIterator >
	iterator operator() ( ArgumentIterator , ArgumentIterator , iterator );

	template< typename ArgumentIterator >
	iterator operator()( ArgumentIterator , ArgumentIterator , sc_vector<T>::iterator );
private:
	// Disabled
	sc_vector_assembly& operator=( const sc_vector_assembly& );
};

template< typename T, typename MT >
sc_vector_assembly<T,MT> sc_assemble_vector( sc_vector<T> & , MT (T::*member_ptr ) );


} // end of namespace sc_core

#endif
