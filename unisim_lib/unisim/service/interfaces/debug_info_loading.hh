/*
 *  Copyright (c) 2012,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_DEBUG_INFO_LOADING_HH__
#define __UNISIM_SERVICE_INTERFACES_DEBUG_INFO_LOADING_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/blob/blob.hh>

namespace unisim {
namespace service {
namespace interfaces {

struct ExecutableBinaryFile : ServiceInterface
{
	virtual const char *GetFilename() const = 0;                      // absolute path of executable binary file
	virtual unisim::util::blob::FileFormat GetFileFormat() const = 0; // file format
	virtual bool IsEnabled() const = 0;                               // whether this executable binary file is considered when searching in debug infos
	virtual void Enable(bool enable = true) = 0;                      // enable/disable whether this executable binary file is considered when searching in debug infos
};

struct ExecutableBinaryFileScanner : ServiceInterface
{
  virtual void Append(unisim::service::interfaces::ExecutableBinaryFile *exec_bin_file) = 0;
};

class DebugInfoLoading : public ServiceInterface
{
public:
	virtual bool LoadDebugInfo(const char *filename) = 0;
	virtual void ScanExecutableBinaryFiles(ExecutableBinaryFileScanner& scanner) const = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
