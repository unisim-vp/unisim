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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__

#include <inttypes.h>
#include <cmath>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/util/hash_table/hash_table.hh"
#include "unisim/kernel/service/service.hh"

#include "unisim/service/interfaces/Fault_Injector.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace ram {

using unisim::util::hash_table::HashTable;
using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;

using unisim::service::interfaces::Fault_Injector;

/* forward declaration */
template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE> class Memory;

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE = 1024 * 1024>
class MemoryPage
{
public:
	MemoryPage(PHYSICAL_ADDR _key, uint8_t initial_state = 0x00);
	~MemoryPage();
private:
	friend class HashTable<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> >;
	friend class Memory<PHYSICAL_ADDR, PAGE_SIZE>;
	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *next;
	uint8_t *storage;

};

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE = 1024 * 1024>
class Memory : public Service<unisim::service::interfaces::Memory<PHYSICAL_ADDR> >, public Client<Fault_Injector<PHYSICAL_ADDR> >
{
public:

	/*bool has_fault;
	//uint8_t* content;
	uint8_t* recov_data;
	//uint8_t* mem_recov;
	uint8_t recov_addr;*/


	/* exported services */
	ServiceExport<unisim::service::interfaces::Memory<PHYSICAL_ADDR> > memory_export;
	
	/* imported services */

	ServiceImport<unisim::service::interfaces::Memory<PHYSICAL_ADDR> > memory_import;
	ServiceImport<unisim::service::interfaces::Fault_Injector<PHYSICAL_ADDR> > fault_import;


	Memory(const  char *name, Object *parent = 0); //, PHYSICAL_ADDR org, uint64_t bytesize);
	virtual ~Memory();
	
	/* service methods */

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	
	/* unisim::service::interfaces::Memory methods */
	virtual void Reset();
	virtual bool WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size);

	bool WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	bool ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	void *GetDirectAccess(PHYSICAL_ADDR physical_addr, PHYSICAL_ADDR& physical_start_addr, PHYSICAL_ADDR& physical_end_addr);


////////////////////03/06/2014

/*

		float evalPe(int X, int B) {


				return  exp(pow (X, B));

				//return (rand()/age);
			}

			void inject_fault(uint8_t* val,uint8_t size)
			{
				uint8_t* error_value;
				error_value = new uint8_t[size];

				memset(error_value, 0, size);

				memcpy (val, error_value, size);

			}

			void compute_fault(uint8_t* val,uint8_t size, uint8_t* &error_value)
			{
				error_value = new uint8_t[size];
				memset(error_value, 0, size);

			}

			std::ostream& displayTab(std::ostream& os, uint8_t* tab, int size)
			{
				os << "0x";
				for (int i=0; i<size; i++)
				{
					os << std::hex << (unsigned int) tab[i];
				}

				return os;
			}

	public:
			void error_provoq(uint8_t* content,PHYSICAL_ADDR phy_addr, uint8_t size, bool fault_enable)
			{

				if (!fault_enable)
					return;

				const int seuil = 80;

				if (evalPe(2 , 1) < seuil)
				{
					has_fault = true;

					recov_data = new uint8_t[size];

					memcpy(recov_data, content, size);

					recov_addr = phy_addr;

		// cas v1
					// calcule et injection de la faute
		//			inject_fault(content, size);


		// cas v2
					// calcule de la faute
					uint8_t* val_ptr;
					compute_fault(content, size, val_ptr);
					if (has_fault) {
						// injection de la faute precedement calculée
						memcpy(content, val_ptr, size);

						std::cout << " at index :" << std::hex << phy_addr << " oldVa=" << std::hex;
						displayTab(std::cout, recov_data, size);
						std::cout << " newVal=";
						displayTab(std::cout, content, size);

						std::cout << std::dec;
						std::cout << " ERROR occured memory content is corrupted "<< std::endl;

					}
					// fin cas v2

				}
			}



*/










//////////////////////////////////



	/* void restor(PHYSICAL_ADDR n, uint8_t size)

		 	{
		//	 	 if (recov_addr == n) {
		//		 	 mem_recov[n]= recov_data;
		//		 	 has_fault = false;
		//			 cout << "at index " << n << " recovered data =" <<  mem_recov[n] << endl;
		//	 	 }


			 	memcpy(mem_recov[recov_addr], recov_data, size);
			 	 has_fault = false;
				 cout << "at index " << hex << recov_addr*2 << " recovered data =" << hex << mem_recov[recov_addr] << endl;

		 	} */


	PHYSICAL_ADDR GetLowAddress();
	PHYSICAL_ADDR GetHighAddress();





protected:
	PHYSICAL_ADDR org;
	PHYSICAL_ADDR bytesize;
	PHYSICAL_ADDR lo_addr;
	PHYSICAL_ADDR hi_addr;
	PHYSICAL_ADDR memory_usage;
	
private:
	HashTable<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> > hash_table;
	
	Parameter<PHYSICAL_ADDR> param_org;
	Parameter<PHYSICAL_ADDR> param_bytesize;
	Statistic<PHYSICAL_ADDR> stat_memory_usage;

	uint8_t initial_byte_value;
	Parameter<uint8_t> param_initial_byte_value;

	bool read_fault_enable;
	Parameter<bool> param_read_fault_enable;
	bool write_fault_enable;
	Parameter<bool> param_write_fault_enable;

};

} // end of namespace ram
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim 

#endif //__UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__
