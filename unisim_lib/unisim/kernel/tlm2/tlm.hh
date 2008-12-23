/*
 *  Copyright (c) 2008,
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
 * Author: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_TLM_HH__
#define __UNISIM_KERNEL_TLM2_TLM_HH__

#include <tlm.h>
#include <queue>

// to remove asap
#include <iostream>
#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

class ManagedPayload
{
public:
	ManagedPayload() :
		extensions(tlm::max_num_extensions()),
		memory_manager(0),
		ref_count(0)
	{
	}
	
	explicit ManagedPayload(tlm::tlm_mm_interface *mm) :
		extensions(tlm::max_num_extensions()),
		memory_manager(mm),
		ref_count(0)
	{
	}
	
	void acquire()
	{
		assert(memory_manager != 0);
		ref_count++;
	}

	void release()
	{
		assert(memory_manager != 0);
		if (--ref_count==0)
			memory_manager->free(reinterpret_cast<tlm::tlm_generic_payload *>(this));
	}
	
	int get_ref_count()
	{
		return ref_count;
	}

	void set_mm(tlm::tlm_mm_interface* mm)
	{
		memory_manager = mm;
	}

	bool has_mm()
	{
		return memory_manager != 0;
	}
	
	void reset()
	{
		extensions.free_entire_cache();
	};

	void deep_copy_from(const ManagedPayload & other)
	{
		for(unsigned int i=0; i<other.extensions.size(); i++)
		{
			if(other.extensions[i])
			{
				if(!extensions[i])
				{
					tlm::tlm_extension_base *ext = other.extensions[i]->clone();
					if(ext)
					{
						if(has_mm())
						{
							set_auto_extension(i, ext);
						}
						else
						{
							set_extension(i, ext);
						}
					}
				}
				else
				{
					extensions[i]->copy_from(*other.extensions[i]);
				}
			}
		}
	}
	
	void update_extensions_from(const ManagedPayload & other)
	{
		for(unsigned int i=0; i<other.extensions.size(); i++)
		{
			if(other.extensions[i])
			{
				if(extensions[i])
				{
					extensions[i]->copy_from(*other.extensions[i]);
				}
			}
		}
	}
	
	void free_all_extensions()
	{
		extensions.free_entire_cache();
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			if(extensions[i])
			{
				extensions[i]->free();
				extensions[i] = 0;
			}
		}
	}
	
	
	virtual ~ManagedPayload()
	{
		for(unsigned int i=0; i<extensions.size(); i++)
			if(extensions[i]) extensions[i]->free();
	}
	
	template <typename T> T* set_extension(T* ext)
	{
		return static_cast<T*>(set_extension(T::ID, ext));
	}
	
	tlm::tlm_extension_base* set_extension(unsigned int index, tlm::tlm_extension_base* ext)
	{
		tlm::tlm_extension_base* tmp = extensions[index];
		extensions[index] = ext;
		return tmp;
	}
	
	template <typename T> T* set_auto_extension(T* ext)
	{
		return static_cast<T*>(set_auto_extension(T::ID, ext));
	}
	
	tlm::tlm_extension_base* set_auto_extension(unsigned int index, tlm::tlm_extension_base* ext)
	{
		tlm::tlm_extension_base* tmp = extensions[index];
		extensions[index] = ext;
		if (!tmp) extensions.insert_in_cache(&extensions[index]);
		assert(memory_manager != 0);
		return tmp;
	}
	
	template <typename T> void get_extension(T*& ext) const
	{
		ext = get_extension<T>();
	}
	
	template <typename T> T* get_extension() const
	{
		return static_cast<T*>(get_extension(T::ID));
	}
	
	tlm::tlm_extension_base* get_extension(unsigned int index) const
	{
		return extensions[index];
	}
	
	template <typename T> void clear_extension(const T* ext)
	{
		clear_extension<T>();
	}
	
	template <typename T> void clear_extension()
	{
		clear_extension(T::ID);
	}
	
	template <typename T> void release_extension(T* ext)
	{
		release_extension<T>();
	}
	
	template <typename T> void release_extension()
	{
		release_extension(T::ID);
	}

    void resize_extensions()
    {
        extensions.expand(tlm::max_num_extensions());
    }

protected:

	ManagedPayload(const ManagedPayload& x) :
		extensions(tlm::max_num_extensions())
	{
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			extensions[i] = x.get_extension(i);
		}
	}
	
	ManagedPayload& operator= (const ManagedPayload& x)
	{
		for(unsigned int i=0; i<extensions.size(); i++)
		{
			extensions[i] = x.get_extension(i);
		}
		return (*this);
	}

private:
	void clear_extension(unsigned int index)
	{
		extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
	}
	
	void release_extension(unsigned int index)
	{
		if (memory_manager)
		{
			extensions.insert_in_cache(&extensions[index]);
		}
		else
		{
			extensions[index]->free();
			extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
		}
	}
	
	tlm::tlm_array<tlm::tlm_extension_base*> extensions;
	tlm::tlm_mm_interface* memory_manager;
	unsigned int ref_count;
};

template <typename PAYLOAD>
class PayloadFabric : public tlm::tlm_mm_interface
{
public:
	PayloadFabric()
	{
	}

	~PayloadFabric()
	{
		while(!free_list.empty())
		{
			PAYLOAD *payload = free_list.front();
			free_list.pop();
			delete payload;
			std::cerr << "PayloadFabric::~PayloadFabric: deleting payload" << std::endl;
		}
	}

	PAYLOAD *allocate()
	{
		PAYLOAD *payload;

		if(!free_list.empty())
		{
			payload = free_list.front();
			free_list.pop();
			payload->acquire();
			return payload;
		}

		payload = new PAYLOAD();
		payload->set_mm(this);
		payload->acquire();
		return payload;
	}

	void free(tlm::tlm_generic_payload *_payload)
	{
		PAYLOAD *payload = reinterpret_cast<PAYLOAD *>(_payload);
		payload->free_all_extensions();
		free_list.push(payload);
	}
private:
	std::queue<PAYLOAD *> free_list;
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_TLM_HH__
