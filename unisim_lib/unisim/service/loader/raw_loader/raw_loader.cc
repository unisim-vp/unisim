/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include <inttypes.h>
#include <string>
#include <fstream>
#include "unisim/service/loader/raw_loader/raw_loader.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace service {
namespace loader {
namespace raw_loader {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::Memory;

RawLoader ::
RawLoader(const char *name, Object *parent)
	: Object(name, parent)
	, Client<Memory<uint64_t> >(name, parent)
	, memory_import("memory-import", this)
	, filename()
	, base_addr(0)
	, size(0)
	, verbose(0)
	, param_filename("filename", this, filename,
			"Location of the raw file to load.")
	, param_base_addr("base-addr", this, base_addr,
			"Target base address.")
	, param_size("size", this, size,
			"Number of bytes to copy."
			" If 0 then all the file contents will be copied."
			" If smaller than the file, the contents of the file will"
			" be cropped."
			" If bigger than the file size, all the file will be copied.")
	, param_verbose("verbose", this, verbose,
			"Set verbose mode (0 not active, otherwise active).")
	, logger(*this)
{
	Object::SetupDependsOn(memory_import);
}

RawLoader ::
~RawLoader()
{
}

bool
RawLoader ::
Setup()
{
	bool success = true;

	if ( !filename.empty() )
	{
		std::string location =
			Object::GetSimulator()->SearchSharedDataFile(filename.c_str());
		std::ifstream is(location.c_str(), 
				std::ifstream::in | std::ifstream::binary);
		if ( success && is.fail() )
		{
			logger << DebugError 
				<< "Can't open input \"" << filename << "\"" 
				<< EndDebugError;
			success = false;
		}

		if ( success && memory_import )
		{
			if ( verbose )
			{
				logger << DebugInfo
					<< "Loading \"" << filename << "\" ("
					<< location << ")"
					<< EndDebugInfo;
			}

			char c;
			bool finished = false;
			for ( uint64_t i = 0; 
					success &&
					((!finished && (size == 0)) | 
					 (!finished && (i < size)));
					i++ )
			{
				is.read(&c, 1);
				if ( is.fail() )
				{
					if ( is.eof() )
						finished = true;
					else
					{
						logger << DebugError
							<< "Error while loading \"" << filename << "\""
							<< EndDebugError;
						success = false;
					}
				}
				else
				{
					if ( !memory_import->WriteMemory(base_addr + i,
								&c, 1) )
					{
						logger << DebugError
							<< "Error while trying to write \"" 
							<< filename << "\" into memory."
							<< " Failure at byte " << i << "."
							<< EndDebugError;
						success = false;
					}
				}
			}
		}
		else 
		{
			if ( verbose )
			{
				logger << DebugInfo
					<< "Not loading \"" << filename << "\" ("
					<< location << ") because the memory is not connected to"
					<< " the loader."
					<< EndDebugInfo;
			}
		}
	}
	return success;
}

} // end of namespace raw_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

