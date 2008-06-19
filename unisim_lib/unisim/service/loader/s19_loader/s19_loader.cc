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


S19_Loader::S19_Loader(char const *name, Object *parent) :
	Object(name,parent),
	Client<Memory<uint32_t> >(name, parent),
	Client<SymbolTableBuild<uint32_t> >(name, parent),
	Service<Loader<uint32_t> >(name, parent),
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
	param_force_use_virtual_address("force-use-virtual-address", this, force_use_virtual_address)
	
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_build_import); 
}

S19_Loader::~S19_Loader() 
{ }

void S19_Loader::Reset() 
{
}

uint32_t S19_Loader::GetEntryPoint() const
{ //TODO
	return 0;
}

uint32_t S19_Loader::GetTopAddr() const
{  // TODO
	return 0;
}

uint32_t S19_Loader::GetStackBase() const
{  // TODO
	return 0;
}

int S19_Loader::Load()
{
	int             linenum;            /* tracks line number in bootstrap file */
	char            srec[256];          /* holds S-record from bootstrap file */
	unsigned int    status;             /* general status variable */
	FILE            *bootptr;           /* pointer to bootstrap file */
	int             n, j;               /* temp registers */

	if (strchr(filename.c_str(), '/') == NULL)  {
		ShowError(ERR_BADFILENAME,0,NULL);
		exit(2);
	}

	linenum = 0;

	bootptr = fopen(filename.c_str(), "r");
	if (!bootptr)  {
		ShowError(ERR_NOFILE,0,NULL);
		exit(1);
	}

	printf("\n\nLoad file %s to simulated RAM.", filename.c_str());

	printf("\n\nLoad started...");

	while (!feof(bootptr))  {
		linenum++;
		fgets(srec, 256, bootptr);
		ProcessRecord(linenum,srec);
	}

	fclose(bootptr);

	printf("\n\nFile %s successfully Loaded.", filename.c_str());

	return 0;	
}


void  S19_Loader::ProcessRecord(int linenum, char srec[256])
{
	int     cnt, s5cnt;
	int     chksum;
	int     tchksum;
	int     sdata;
	int     addr;
	int     n;
	int		addrSize;

	if (srec[0] == '\0')  return;           /* just in case */
	if (srec[0] == '\n')  return;           /* just in case */
	if (srec[0] == '*')  return;            /* * in column 1 = comment */
	if (srec[0] != 'S')  {                  /* no S in column 1... */
		ShowError(ERR_BADREC,linenum,srec); /* show bad record format */
		exit(2);                            /* exit fatally */
	}
	
    /* S0 = header, ignore it */
    /* S7 = A termination record for a block S3, ignore it */
    /* S8 = A termination record for a block S2, ignore it */
    /* S9 = A termination record for a block S1, ignore it */

	if ((srec[1] == S0) || (srec[1] == S7) ||
	    (srec[1] == S8) || (srec[1] == S9)) 
	{
	    return;
	} 

    /* S5 = A record containing the number of S1, S2, and S3 records
     *      transmitted in a particular block.
     */
	if (srec[1] == S5) {
		sscanf(srec+2, "%2x", &cnt);            /* get the number of bytes in rec */
		chksum = cnt;
		sscanf(srec+4, "%4x", &s5cnt);           /* get addr of this rec */
		chksum += (s5cnt >> 8);
		chksum += (s5cnt & 0xff);

		sscanf(srec+2+(cnt*2), "%2x", &tchksum);
		if ((tchksum + (chksum & 0xff)) != 0xff)  {
			ShowError(ERR_BADCHKSUM,linenum,srec);
			exit(2);
		}
	}
	
    /* not (S1 or S2 or S3), unsupported record */
    /* show the error */
	if ((srec[1] != S1) && (srec[1] != S2) && (srec[1] != S3)) 
	{
		ShowError(ERR_NOSUPPORT,linenum,srec);           /* show the error */
		exit(2);
	}

	sscanf(srec+2, "%2x", &cnt);            /* get the number of bytes in rec */
	chksum = cnt;

	switch (srec[1]) {
		case S1: { /* A record containing code/data and the 2-byte address (offset) at which the code/data is reside */
			addrSize = 4;	// S1: address is coded on 4 hex digits (2 bytes)
			sscanf(srec+4, "%4x", &addr);           /* get addr of this rec */
			chksum += (addr >> 8);
			chksum += (addr & 0xff);
		} break;
		
		case S2: { /* A record containing code/data and the 3-byte address (offset) at which the code/data is reside */
			addrSize = 6;	// S2: address is coded on 6 hex digits (3 bytes)
			sscanf(srec+4, "%6x", &addr);           /* get addr of this rec */
			chksum += (addr >> 16);
			chksum += ((addr >> 8) & 0xff);			
			chksum += (addr & 0xff);
		} break;
		
		case S3: { /* A record containing code/data and the 4-byte address (offset) at which the code/data is reside */
			addrSize = 8;	// S3: address is coded on 8 hex digits (4 bytes)		
			sscanf(srec+4, "%8x", &addr);           /* get addr of this rec */
			chksum += (addr >> 24);
			chksum += ((addr >> 16) & 0xff);			
			chksum += ((addr >> 8) & 0xff);			
			chksum += (addr & 0xff);
		} break;
		
		default: {
		    /* not (S1 or S2 or S3), unsupported record */
    		/* show the error */
			ShowError(ERR_NOSUPPORT,linenum,srec);  /* show the error */
			exit(2);
		}
	}

	for (n=2; n<(cnt-2); n++)  {
		sscanf(srec+addrSize+(n*2), "%2x", &sdata);
		chksum += sdata;
		
		busWrite(addr, sdata);
		
		addr++;
	}
	
	sscanf(srec+2+(cnt*2), "%2x", &tchksum);
	if ((tchksum + (chksum & 0xff)) != 0xff)  {
		printf("check sum %x\n",chksum);
		ShowError(ERR_BADCHKSUM,linenum,srec);
		exit(2);
	}
	
	
}


void  S19_Loader::ShowError(int  errnum, int linenum, char srec[256])
{

	switch (errnum) {
		case ERR_NOFILE: printf("\nError: Unable to open specified .S19 bootstrap file."); break;
		case ERR_BADREC: printf("\nError: Bad S19 record."); break;
		case ERR_NOSUPPORT: printf("\nError: Unsupported S-record format; must be S0, S1 or S9."); break;
		case ERR_BADADDR: printf("\nError: Address is out of range for this MCU."); break;
		case ERR_BADCHKSUM: printf("\nError: Record checksum is bad."); break;
		case ERR_BADFILENAME: printf("\nError: Illegal character in file name."); break;
		default: printf("\nError: Unknown!");
	}
	
	if (linenum)  {
		printf("\nline %d:  %s", linenum, srec);
	}
}

// TODO: use the tlm bus interface

void S19_Loader::busWrite(uint32_t addr, int data) {
	printf("\naddr %x  data  %x",addr,data);
}

} // end UNISIM namespace
} // end SERVICE namespace
} // end LOADER namespace
} // end S19_LOADER

