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

#include <unisim/util/locate/locate.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access()
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>

namespace unisim {
namespace util {
namespace locate {

std::string Dirname(const std::string& path)
{
	std::size_t pos = path.find_last_of(
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		"\\/"
#else
		'/'
#endif
	);
	return (pos != std::string::npos) ? path.substr(0, pos) : path;
}

std::string Basename(const std::string& path)
{
	std::size_t pos = path.find_last_of(
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		"\\/"
#else
		'/'
#endif
	);
	return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}

bool ResolvePath(const std::string& unresolved_path, std::string& resolved_path)
{
	char resolved_path_buf[FILENAME_MAX + 1];

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	if(realpath(unresolved_path.c_str(), resolved_path_buf))
	{
		resolved_path = resolved_path_buf;
		return true;
	}
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	DWORD length = GetFullPathName(unresolved_path.c_str(), PATH_MAX + 1, resolved_path_buf, 0);
	if(length > 0)
	{
		resolved_path_buf[length] = 0;
		resolved_path = resolved_path_buf;
		return true;
	}
#endif
	return false;
}

std::string LocateFile(const std::string& filename, const LocateFileOptions& options)
{
	std::string out;
	return LocateFile(filename, out, options) ? out : filename;
}

bool LocateFile(const std::string& filename, std::string& match_file_path, const LocateFileOptions& options)
{
	if(filename.empty()) return false;
	
	if(ResolvePath(filename, match_file_path) && access(match_file_path.c_str(), R_OK) == 0)
	{
		return true;
	}
	
	std::string s;
	const char *p;

	std::vector<std::string> search_paths;

	int const num_passes = options.shared_directory.empty() ? 1 : 2;
	for(int pass = 0; pass < num_passes; ++pass)
	{
		s.clear();
		p = options.search_path.c_str();
		do
		{
			if(*p == 0 || *p == ';')
			{
				if(pass) search_paths.push_back(options.shared_directory + '/' + s);
				else search_paths.push_back(s);
				
				s.clear();
			}
			else
			{
				s += *p;
			}
		} while(*(p++));
	}
	
	std::vector<std::string> hierarchical_path;
	
	s.clear();
	p = filename.c_str();
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	bool match = false;
	
	int hierarchical_path_depth = hierarchical_path.size();
	
	if(hierarchical_path_depth > 0)
	{
		int num_search_paths = search_paths.size();
		int k;
		
		for(k = 0; k < num_search_paths; k++)
		{
			const std::string& search_path = search_paths[k];
			int i;
			for(i = 0; (!match) && (i < (options.lazy_match ? hierarchical_path_depth : 1)); i++)
			{
				std::string try_file_path = search_path;
				int j;
				for(j = i; j < hierarchical_path_depth; j++)
				{
					if(!try_file_path.empty()) try_file_path += '/';
					try_file_path += hierarchical_path[j];
				}
// 				std::cerr << "try_file_path=\"" << try_file_path << "\":";
				
				if(ResolvePath(try_file_path, match_file_path) && access(match_file_path.c_str(), R_OK) == 0)
				{
// 					std::cerr << "found" << std::endl;
					match = true;
				}
				else
				{
// 					std::cerr << "not found" << std::endl;
				}
			}
		}
	}
	
	return match;
}

} // end of namespace locate
} // end of namespace util
} // end of namespace unisim

#if 0
int main(int argc, const char *argv[])
{
	std::string r;
	unisim::util::locate::LocateFileOptions o;
	o.shared_directory = "/path/to/share";
	o.search_path = "abc;def";
	o.lazy_match = true;
	unisim::util::locate::LocateFile("toto.cc", r, o);
	return 0;
}
#endif
