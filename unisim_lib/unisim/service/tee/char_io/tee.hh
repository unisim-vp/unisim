/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__
#define __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <string>

namespace unisim {
namespace service {
namespace tee {
namespace char_io {

template <unsigned int MAX_IMPORTS = 8>
class Tee
	: public unisim::kernel::Service<interfaces::CharIO>
	, public unisim::kernel::Client<interfaces::CharIO>
{
public:
	unisim::kernel::ServiceExport<interfaces::CharIO> char_io_export;
	unisim::kernel::ServiceImport<interfaces::CharIO> *char_io_import[MAX_IMPORTS];
	unisim::kernel::ServiceImport<interfaces::CharIO>& char_io_import_n(unsigned idx) { return *char_io_import[idx]; }

	Tee(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Tee();

	virtual void Setup(interfaces::CharIO*) override;

	virtual void ResetCharIO();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
};

template <unsigned int MAX_IMPORTS>
Tee<MAX_IMPORTS>::Tee(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, unisim::kernel::Service<interfaces::CharIO>(name, parent)
	, unisim::kernel::Client<interfaces::CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, char_io_import()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		char_io_import[i] = new kernel::ServiceImport<interfaces::CharIO>((std::string("char-io-import[") + std::to_string(i) + "]").c_str(), this);
	}
}

template <unsigned int MAX_IMPORTS>
Tee<MAX_IMPORTS>::~Tee()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
		delete char_io_import[i];
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::Setup(interfaces::CharIO*)
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
		char_io_import_n(i).RequireSetup();
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::ResetCharIO()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		if(unisim::kernel::ServiceImport<interfaces::CharIO>& import = char_io_import_n(i))
		{
			import->ResetCharIO();
		}
	}
}

template <unsigned int MAX_IMPORTS>
bool Tee<MAX_IMPORTS>::GetChar(char& c)
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		if(unisim::kernel::ServiceImport<interfaces::CharIO>& import = char_io_import_n(i))
		{
			if(import->GetChar(c)) return true;
		}
	}
	
	return false;
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::PutChar(char c)
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		if(unisim::kernel::ServiceImport<interfaces::CharIO>& import = char_io_import_n(i))
		{
			import->PutChar(c);
		}
	}
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::FlushChars()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		if(unisim::kernel::ServiceImport<interfaces::CharIO>& import = char_io_import_n(i))
		{
			import->FlushChars();
		}
	}
}

} // end of namespace char_io
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__
