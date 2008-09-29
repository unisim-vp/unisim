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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

/*
 *  s19_loader.cc      s19 loader, using UNISIM RAM.
 */


#include <unisim/service/loader/s19_loader/s19_loader.hh>

namespace unisim {
namespace service {
namespace loader {
namespace s19_loader {


S19_Loader::S19_Loader(char const *name, S19_Loader::MODE memMode, Object *parent) :
	Object(name,parent),
	Client<Memory<physical_address_t> >(name, parent),
	Client<SymbolTableBuild<physical_address_t> >(name, parent),
	Service<Loader<physical_address_t> >(name, parent),
	memory_import("memory-import", this),
	symbol_table_build_import("symbol-table-build-import", this),
	loader_export("loader-export", this),
	filename(),
	entry_point(0),
	base_addr(0),
	top_addr(0),
	force_use_virtual_address(false),
	param_filename("filename", this, filename),
	param_base_addr("base-addr", this, base_addr),
	param_force_use_virtual_address("force-use-virtual-address", this, force_use_virtual_address),
	isFirstDataRec(true),
	memoryMode(memMode)
	
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_build_import); 
}

S19_Loader::~S19_Loader() 
{ 
}

void S19_Loader::OnDisconnect()
{
}

void S19_Loader::Reset() 
{
	if(memory_import) memory_import->Reset();
}

physical_address_t S19_Loader::GetEntryPoint() const
{ 
	return entry_point;
}

physical_address_t S19_Loader::GetTopAddr() const
{  
	return top_addr;
}

physical_address_t S19_Loader::GetStackBase() const
{  // TODO
	return 0;
}

bool S19_Loader::Setup() {
	 
	int             linenum;            /* tracks line number in bootstrap file */
	char            srec[S_RECORD_SIZE];          /* holds S-record from bootstrap file */
	unsigned int    status;             /* general status variable */
	int             n, j;               /* temp registers */
	FILE            *bootptr;           /* pointer to bootstrap file */
	bool			success;

	if(symbol_table_build_import) symbol_table_build_import->Reset();
	
	if(filename.empty()) return true;
	
	linenum = 0;

	bootptr = fopen(filename.c_str(), "r");
	if (!bootptr)  {
		ShowError(ERR_NOFILE,0,NULL);
		return false;
	}

	cerr << Object::GetName() << ": Load file \"" << filename << "\" to simulated RAM." << endl;
	cerr << Object::GetName() << ": Load started..." << endl;

	while (!feof(bootptr))  {
		linenum++;
		fgets(srec, S_RECORD_SIZE, bootptr);
		success = ProcessRecord(linenum,srec);
	}

	fclose(bootptr);

	cerr << Object::GetName() << ": File \"" << filename << "\" successfully Loaded." << endl;

	return success;
}

void S19_Loader::GetPagedAddress(s19_address_t s19_addr, page_t &page, address_t &cpu_address)
{
	if (memoryMode == GNUGCC) 
	{
		if (s19_addr < 0x10000) {
			page = 0;
			cpu_address = (address_t) s19_addr;
		} else {
			page = (page_t) ((s19_addr - 0x10000) / 0x4000);
			cpu_address = (address_t) ((s19_addr - 0x10000) % 0x4000);
		}
	} 
	else
	{
		if (memoryMode == LINEAR) {
			page = (page_t) (s19_addr / 0x4000);
			cpu_address = (address_t) ((s19_addr % 0x4000) + 0x8000);
		} else { // BANKED
			page = (page_t) (s19_addr / 0x10000);
			cpu_address = (address_t) (s19_addr % 0x10000);
		}
	}
	
}

// TODO: This function is a duplicate of once in MMC class.
//       I have to see how to rearange the project to remove this duplication !!!
physical_address_t S19_Loader::GetFlashAddress(page_t page, address_t logicalAddress)
{
	static const physical_address_t FLASH_PHYSICAL_ADDRESS_FIXED_BITS = 0x00400000;
	static const uint8_t FLASH_ADDRESS_SIZE = 14;
	static const address_t FLASH_CPU_ADDRESS_BITS = 0x3FFF;
	static const uint8_t PPAGE_LOW	= 0xE0;		// low ppage (flash page) register value

	if (page < PPAGE_LOW) {
		// throw "non-valid accesses to memory"
	}

	if (((page == 0xFD) && (logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) ||
		((page == 0xFF) && (logicalAddress > 0xBFFF)) ||
		((logicalAddress > 0x7FFF) && (logicalAddress < 0xC000)))
	{
		return FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) page << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_CPU_ADDRESS_BITS & logicalAddress);
	}
	else
	{
		return logicalAddress;
	}

}

bool  S19_Loader::ProcessRecord(int linenum, char srec[S_RECORD_SIZE])
{
	int     cnt, s5cnt;
	int     chksum;
	int     tchksum;
	unsigned char     sdata[254];
	physical_address_t     flash_address;
	s19_address_t s19_addr;
	address_t cpu_address;
	page_t page;
	int     n, sdataIndex, nDataByte;
	int		addrSize;

	if (srec[0] == '\0')  return true;           /* just in case */
	if (srec[0] == '\n')  return true;           /* just in case */
	if (srec[0] == '*')  return true;            /* * in column 1 = comment */
	if (srec[0] != 'S')  {                  /* no S in column 1... */
		ShowError(ERR_BADREC,linenum,srec); /* show bad record format */
		return false;                            /* exit fatally */
	}


	sscanf(srec+2, "%2x", &cnt);            /* get the number of bytes in rec */
	chksum = cnt;

	switch (srec[1]) {
		case S0: {	/* S0 = header, ignore it */
			return true; 
		} break;
		
		case S5: {
		    /* S5 = A record containing the number of S1, S2, and S3 records
		     *      transmitted in a particular block.
		     */
			sscanf(srec+4, "%4x", &s5cnt);           /* get addr of this rec */
			chksum += (s5cnt >> 8);
			chksum += (s5cnt & 0xff);
	
			sscanf(srec+2+(cnt*2), "%2x", &tchksum);
			if ((tchksum + (chksum & 0xff)) != 0xff)  {
				ShowError(ERR_BADCHKSUM,linenum,srec);
				return false;
			}
			
			return true;

		} break;
		
		case S7: {	/* S7 = A termination record for a block S3,
					 * may contain entry point. default 0x00000000 
					 */
			sscanf(srec+4, "%8x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return true;
		} break;
		
		case S8: {	/* S8 = A termination record for a block S2,
					 * may contain entry point. default 0x000000 
					 */
			sscanf(srec+4, "%6x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return true;
		} break;
		
		case S9: {	/* S9 = A termination record for a block S1,
					 * may contain entry point. default 0x0000 
					 */
			sscanf(srec+4, "%4x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return true;
		} break;
		
		default: {
		
			switch (srec[1]) {
				case S1: { /* A record containing code/data and the 2-byte address (offset) at which the code/data is reside */
					addrSize = 2;	// S1: address is coded on 4 hex digits (2 bytes)
					sscanf(srec+4, "%4x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 8);
					chksum += (s19_addr & 0xff);
				} break;
				
				case S2: { /* A record containing code/data and the 3-byte address (offset) at which the code/data is reside */
					addrSize = 3;	// S2: address is coded on 6 hex digits (3 bytes)
					sscanf(srec+4, "%6x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 16);
					chksum += ((s19_addr >> 8) & 0xff);			
					chksum += (s19_addr & 0xff);
				} break;
				
				case S3: { /* A record containing code/data and the 4-byte address (offset) at which the code/data is reside */
					addrSize = 4;	// S3: address is coded on 8 hex digits (4 bytes)		
					sscanf(srec+4, "%8x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 24);
					chksum += ((s19_addr >> 16) & 0xff);			
					chksum += ((s19_addr >> 8) & 0xff);			
					chksum += (s19_addr & 0xff);
				} break;
				
				default: {
				    /* not (S1 or S2 or S3), unsupported record */
		    		/* show the error */
					ShowError(ERR_NOSUPPORT,linenum,srec);  /* show the error */
					return false;
				}
			}

			nDataByte = (cnt-addrSize-1); 
			for (sdataIndex=0; sdataIndex<nDataByte; sdataIndex++)  {
				sscanf(srec+(addrSize+2+sdataIndex)*2, "%2x", &sdata[sdataIndex]);
				chksum += sdata[sdataIndex];

			}

			
			sscanf(srec+2+(cnt*2), "%2x", &tchksum);

			if ((tchksum + (chksum & 0xff)) != 0xff)  {
				cerr << "check sum " << chksum << "\n";
				ShowError(ERR_BADCHKSUM,linenum,srec);
				return false;
			}
		
			if (isFirstDataRec) {
				isFirstDataRec = false;
				entry_point = s19_addr;
			}
			
			if (s19_addr == 0xFFFE) { // 0xFFFE is the Reset Vector Address for the CPU12
				entry_point = (uint16_t)(sdata[0] << 8) + sdata[1];
			}
			
			top_addr = s19_addr;
			
			GetPagedAddress(s19_addr, page, cpu_address);
			flash_address = GetFlashAddress(page, cpu_address);
			
			return memWrite(flash_address, sdata,sdataIndex);
		}
	}
	
}

bool S19_Loader::memWrite(physical_address_t addr, const void *buffer, uint32_t size) {

	bool success = false;
	
	if(memory_import)
	{
		if(size > 0) 
		{
			if(!memory_import->WriteMemory(addr, buffer, size))
			{
				cerr << Object::GetName() << ": Can't write into memory (@0x" << hex << addr << " - @0x" << (addr +  size - 1) << dec << ")" << endl;
			}
			else 
			{
				cerr << Object::GetName() << ": write into memory (@0x" << hex << addr << " - @0x" << (addr +  size - 1) << dec << ")" << endl;
				success = true;
			}
		}
	}

	return success;
}


void  S19_Loader::ShowError(int  errnum, int linenum, char srec[S_RECORD_SIZE])
{
	cerr << Object::GetName() << ": ";
	switch (errnum) {
		case ERR_NOFILE: cerr << "Error: Unable to open \"" << filename << "\""; break;
		case ERR_BADREC: cerr << "Error: Bad S19 record."; break;
		case ERR_NOSUPPORT: cerr << "Error: Unsupported S-record format; must be S0, S1 or S9."; break;
		case ERR_BADADDR: cerr << "Error: Address is out of range for this MCU."; break;
		case ERR_BADCHKSUM: cerr << "Error: Record checksum is bad."; break;
		case ERR_BADFILENAME: cerr << "Error: Illegal character in file name."; break;
		default: cerr << "Error: Unknown!";
	}
	
	if (linenum)  {
		cerr << "\nline " << linenum << " " << srec ;
	}
	
	cerr << endl;
}

} // end UNISIM namespace
} // end SERVICE namespace
} // end LOADER namespace
} // end S19_LOADER

