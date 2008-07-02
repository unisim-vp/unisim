/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2008 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 3.0 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

*****************************************************************************/

// This source code derives from the proof of concept implementation for the TLM 2.0 standard
// The original C++ file was TLM-2008-06-09/include/tlm/tlm_h/tlm_trans/tlm_generic_payload/tlm_gp.h
// The extension and memory management mechanisms have been extracted from class tlm_generic_payload to
// create class ManagedPayload which enable us to define our own payloads (for everything not a
// memory mapped bus) while being as nearest as possible to the TLM 2.0 way of managing payload memory and
// extensions. We have added a custom implementation of tlm_mm_interface (PayloadFabric) as no
// default implementation exists in the TLM 2.0 standard.

#ifndef __UNISIM_KERNEL_TLM2_TLM_HH__
#define __UNISIM_KERNEL_TLM2_TLM_HH__

#include <tlm.h>
#include <queue>

namespace unisim {
namespace kernel {
namespace tlm2 {

//---------------------------------------------------------------------------
// The managed payload class:
//---------------------------------------------------------------------------

class ManagedPayload {
public:
    ManagedPayload()
        : m_extensions(tlm::max_num_extensions())
        ,m_mm(0)
        ,m_ref_count(0)
    {
    }

    explicit ManagedPayload(tlm::tlm_mm_interface *mm)
        : m_extensions(tlm::max_num_extensions())
        ,m_mm(mm)
        ,m_ref_count(0)
    {
    }


    /* --------------------------------------------------------------------- */
    /* Memory management mechanism:                                          */
    /* --------------------------------------------------------------------- */

    void acquire(){assert(m_mm != 0); m_ref_count++;}
    void release(){assert(m_mm != 0); if (--m_ref_count==0) m_mm->free(reinterpret_cast<tlm::tlm_generic_payload *>(this));}
    int get_ref_count(){return m_ref_count;}
    void set_mm(tlm::tlm_mm_interface* mm) { m_mm = mm; }
    bool has_mm() { return m_mm != 0; }

    void reset(){
      //should the other members be reset too?
      m_extensions.free_entire_cache();
    };

public:
    // non-virtual deep-copying of the object
    void deep_copy_from(const ManagedPayload & other)
    {
        // deep copy extensions (sticky and non-sticky)
        for(unsigned int i=0; i<other.m_extensions.size(); i++)
        {
            if(other.m_extensions[i])
            {                       //original has extension i
                if(!m_extensions[i])
                {                   //We don't: clone.
                    tlm::tlm_extension_base *ext = other.m_extensions[i]->clone();
                    if(ext)			//extension may not be clonable.
                    {
                        if(has_mm())
                        {           //mm can take care of removing cloned extensions
                            set_auto_extension(i, ext);
                        }
                        else
                        {           // no mm, user will call free_all_extensions().
                            set_extension(i, ext);
                        }
                    }
                }
                else
                {                   //We already have such extension. Copy original over it.
                    m_extensions[i]->copy_from(*other.m_extensions[i]);
                }
            }
        }
    }

    void update_extensions_from(const ManagedPayload & other)
    {
        // deep copy extensions that are already present
        for(unsigned int i=0; i<other.m_extensions.size(); i++)
        {
            if(other.m_extensions[i])
            {                       //original has extension i
                if(m_extensions[i])
                {                   //We have it too. copy.
                    m_extensions[i]->copy_from(*other.m_extensions[i]);
                }
            }
        }
    }

    // Free all extensions. Useful when reusing a cloned transaction that doesn't have memory manager.
    // normal and sticky extensions are freed and extension array cleared.
    void free_all_extensions()
    {
        m_extensions.free_entire_cache();
        for(unsigned int i=0; i<m_extensions.size(); i++)
        {
            if(m_extensions[i])
            {
                m_extensions[i]->free();
                m_extensions[i] = 0;
            }
        }
    }
    //--------------
    // Destructor
    //--------------
    virtual ~ManagedPayload() {
        for(unsigned int i=0; i<m_extensions.size(); i++)
            if(m_extensions[i]) m_extensions[i]->free();
    }

    /* --------------------------------------------------------------------- */
    /* Dynamic extension mechanism:                                          */
    /* --------------------------------------------------------------------- */
    /* The extension mechanism is intended to enable initiator modules to    */
    /* optionally and transparently add data fields to the                   */
    /* tlm_generic_payload or any user defined payload.                      */
    /* Target modules are free to check for extensions  */
    /* and may or may not react to the data in the extension fields. The     */
    /* definition of the extensions' semantics is solely in the              */
    /* responsibility of the user.                                           */
    /*                                                                       */
    /* The following rules apply:                                            */
    /*                                                                       */
    /* - Every extension class must be derived from tlm_extension, e.g.:     */
    /*     class my_extension : public tlm_extension<my_extension> { ... }   */
    /*                                                                       */
    /* - A tlm_generic_payload or any user defined payload object should be  */
    /*   constructed after C++ static initialization time.                   */
    /*   This way it is guaranteed that the extension array is of sufficient */
    /*   size to hold all possible extensions. Alternatively, the initiator  */
    /*   module can enforce a valid extension array size by calling the      */
    /*   resize_extensions() method once before the first transaction with   */
    /*   the payload object is initiated.                                    */
    /*                                                                       */
    /* - Initiators should use the the set_extension(e) or clear_extension(e)*/
    /*   methods for manipulating the extension array. The type of the       */
    /*   argument must be a pointer to the specific registered extension     */
    /*   type (my_extension in the above example) and is used to             */
    /*   automatically locate the appropriate index in the array.            */
    /*                                                                       */
    /* - Targets can check for a specific extension by calling               */
    /*   get_extension(e). e will point to zero if the extension is not      */
    /*   present.                                                            */
    /*                                                                       */
    /* --------------------------------------------------------------------- */

    // Stick the pointer to an extension into the vector, return the
    // previous value:
    template <typename T> T* set_extension(T* ext)
    {
        return static_cast<T*>(set_extension(T::ID, ext));
    }

    // non-templatized version with manual index:
    tlm::tlm_extension_base* set_extension(unsigned int index,
                                      tlm::tlm_extension_base* ext)
    {
        tlm::tlm_extension_base* tmp = m_extensions[index];
        m_extensions[index] = ext;
        return tmp;
    }

    // Stick the pointer to an extension into the vector, return the
    // previous value and schedule its release
    template <typename T> T* set_auto_extension(T* ext)
    {
        return static_cast<T*>(set_auto_extension(T::ID, ext));
    }

    // non-templatized version with manual index:
    tlm::tlm_extension_base* set_auto_extension(unsigned int index,
                                           tlm::tlm_extension_base* ext)
    {
        tlm::tlm_extension_base* tmp = m_extensions[index];
        m_extensions[index] = ext;
        if (!tmp) m_extensions.insert_in_cache(&m_extensions[index]);
        assert(m_mm != 0);
        return tmp;
    }

    // Check for an extension, ext will point to 0 if not present
    template <typename T> void get_extension(T*& ext) const
    {
        ext = get_extension<T>();
    }
    template <typename T> T* get_extension() const
    {
        return static_cast<T*>(get_extension(T::ID));
    }
    // Non-templatized version with manual index:
    tlm::tlm_extension_base* get_extension(unsigned int index) const
    {
        return m_extensions[index];
    }

    //this call just removes the extension from the txn but does not
    // call free() or tells the MM to do so
    // it return false if there was active MM so you are now in an unsafe situation
    // recommended use: when 100% sure there is no MM
    template <typename T> void clear_extension(const T* ext)
    {
        clear_extension<T>();
    }

    //this call just removes the extension from the txn but does not
    // call free() or tells the MM to do so
    // it return false if there was active MM so you are now in an unsafe situation
    // recommended use: when 100% sure there is no MM
    template <typename T> void clear_extension()
    {
        clear_extension(T::ID);
    }

    //this call removes the extension from the txn and does
    // call free() or tells the MM to do so when the txn is finally done
    // recommended use: when not sure there is no MM
    template <typename T> void release_extension(T* ext)
    {
        release_extension<T>();
    }

    //this call removes the extension from the txn and does
    // call free() or tells the MM to do so when the txn is finally done
    // recommended use: when not sure there is no MM
    template <typename T> void release_extension()
    {
        release_extension(T::ID);
    }

private:
    // Non-templatized version with manual index
    void clear_extension(unsigned int index)
    {
        m_extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
    }
    // Non-templatized version with manual index
    void release_extension(unsigned int index)
    {
        if (m_mm)
        {
            m_extensions.insert_in_cache(&m_extensions[index]);
        }
        else
        {
            m_extensions[index]->free();
            m_extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
        }
    }

public:
    // Make sure the extension array is large enough. Can be called once by
    // an initiator module (before issuing the first transaction) to make
    // sure that the extension array is of correct size. This is only needed
    // if the initiator cannot guarantee that the generic payload object is
    // allocated after C++ static construction time.
    void resize_extensions()
    {
        m_extensions.expand(tlm::max_num_extensions());
    }

protected:
    //disabled copy ctor and assignment operator.
    // Copy constructor
    ManagedPayload(const ManagedPayload& x)
        : m_extensions(tlm::max_num_extensions())
    {
        // copy all extensions
        for(unsigned int i=0; i<m_extensions.size(); i++)
        {
            m_extensions[i] = x.get_extension(i);
        }
    }

    // Assignment operator
    ManagedPayload& operator= (const ManagedPayload& x)
    {
        // extension copy: all extension arrays must be of equal size by
        // construction (i.e. it must either be constructed after C++
        // static construction time, or the resize_extensions() method must
        // have been called prior to using the object)
        for(unsigned int i=0; i<m_extensions.size(); i++)
        {
            m_extensions[i] = x.get_extension(i);
        }
        return (*this);
    }
private:

    tlm::tlm_array<tlm::tlm_extension_base*> m_extensions;
    tlm::tlm_mm_interface*              m_mm;
    unsigned int                   m_ref_count;
};

//---------------------------------------------------------------------------
// The payload fabric class:
//---------------------------------------------------------------------------

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
		}
	}

	PAYLOAD *allocate()
	{
		PAYLOAD *payload;

		if(!free_list.empty())
		{
			payload = free_list.front();
			free_list.pop();
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
		free_list.push(payload);
	}
private:
	std::queue<PAYLOAD *> free_list;
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_TLM_HH__
