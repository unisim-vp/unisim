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

#define DEBUG_LOCATE_FILE 0

#include <unisim/util/locate/locate.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access()
#endif

#include <unistd.h>
#include <string>
#include <vector>
#if DEBUG_LOCATE_FILE
#include <iostream>
#endif

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
	return (pos != std::string::npos) ? path.substr(0, pos) : std::string(".");
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

static bool IsDriveLetter(char c)
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

static char DriveLetter(const std::string& path)
{
	if(path.length() >= 2)
	{
		char c0 = path[0];
		char c1 = path[1];
		return (IsDriveLetter(c0) && (c1 == ':')) ? c0 : 0;
	}
	return 0;
}

bool IsAbsolutePath(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	return (path.length() >= 3) && DriveLetter(path) && (path[2] == '\\');
#else
	return !path.empty() && (path[0] == '/');
#endif
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

std::vector<std::string> LocateFileOptions::null_search_paths;
std::string LocateFileOptions::null_shared_directory;

std::string LocateFile(const std::string& filename, const LocateFileOptions& options)
{
	std::string out;
	return LocateFile(filename, out, options) ? out : filename;
}

bool LocateFile(const std::string& filename, std::string& match_file_path, const LocateFileOptions& options)
{
	// if filename is empty: fail
	if(filename.empty()) return false;
	// if filename is absolute or relative to cwd, exists, and is a readable file: success
	if(ResolvePath(filename, match_file_path) && access(match_file_path.c_str(), R_OK) == 0) return true;
	// if filename is not relative: fail
	if(IsAbsolutePath(filename)) return false;
	
	// filename is relative either to current working directory or to search paths
	
	// if there are no search paths and no shared directory and match is not lazy: fail
	if(options.search_paths.empty() && options.shared_directory.empty() && !options.lazy_match) return false;
	
	// determine file path separator of host
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	char separator = '\\';
#else
	char separator = '/';
#endif
	// if there are search paths and match is not lazy
	if(options.search_paths.empty() && !options.lazy_match)
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		if(DriveLetter(filename))
		{
			// on windows, if filename is relative to a drive: fail (because it should have been resolved earlier)
			return false;
		}
		else
#endif
		{
			// otherwise, simply look at shared_directory/filename
			std::string try_file_path(options.shared_directory);
			try_file_path += separator;
			try_file_path += filename;
			return ResolvePath(try_file_path, match_file_path) && (access(match_file_path.c_str(), R_OK) == 0);
		}
	}
	
	std::vector<std::string> parts;
	
	// if match is lazy
	if(options.lazy_match)
	{
		std::size_t pos = 0;
		std::size_t len = filename.length();
		
		// skip root drive part
		char c0 = filename[0];
		if(c0 == '/')
		{
			++pos;
		}
		else if(len >= 2)
		{
			char c1 = filename[1];
			if(IsDriveLetter(c0) && (c1 == ':'))
			{
				pos += 2;
				if(len >= 3)
				{
					char c2 = filename[2];
					if((c2 == '/') || (c2 == '\\')) ++pos;
				}
			}
		}
		
		// split filename into parts
		std::string part;
		while(pos < len)
		{
			char c = filename[pos];
			if((c == '/') || (c == '\\'))
			{
				if(!part.empty())
				{
					parts.push_back(part);
					part.clear();
				}
			}
			else
			{
				part += c;
			}
			++pos;
		}
		if(!part.empty())
		{
			parts.push_back(part);
		}
	}
	
	std::size_t depth = parts.size();
	if(!options.lazy_match || (depth > 0))
	{
		// get current working directory
		std::string cwd_path;
		char cwd_path_buf[FILENAME_MAX];
		if(getcwd(cwd_path_buf, sizeof(cwd_path_buf)))
		{
			cwd_path = cwd_path_buf;
		}
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		char curr_drive_letter = DriveLetter(cwd_path);
#endif
		int num_search_paths = options.search_paths.size();
		int const num_passes = options.shared_directory.empty() ? 1 : 2;
		// pass 0:
		//   - try cwd/filename (k = -1)
		//   - for each search_path try cwd/search_path/filename (k >= 0)
		// pass 1:
		//   - try shared_directory/filename (k = -1)
		//   - for each search_path try shared_directory/search_path/filename (k >= 0)
		//
		// Note: shared_directory or cwd_path maybe empty
		for(int pass = 0; pass < num_passes; ++pass)
		{
			for(int k = -1, max_k = num_search_paths ? (num_search_paths - 1) : 0; k <= max_k; ++k)
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				std::string
#else
				const std::string&
#endif
				opt_search_path = ((k >= 0) && (k < num_search_paths)) ? options.search_paths[k] : (pass ? options.shared_directory : cwd_path);
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				if(!opt_search_path.empty() && !IsAbsolutePath(opt_search_path) && curr_drive_letter)
				{
					char c0 = opt_search_path[0];
					if((c0 == '/') || (c0 == '\\'))
					{
						// make path absolute on current drive
						std::string drive_root(1, curr_drive_letter);
						drive_root += ':';
						drive_root += separator;
						opt_search_path.insert(0, drive_root);
					}
					else if(DriveLetter(opt_search_path))
					{
						// make path absolute on target drive
						opt_search_path.insert(2, 1, separator);
					}
				}
#endif
				if(pass || opt_search_path.empty() || IsAbsolutePath(opt_search_path) || !cwd_path.empty())
				{
					std::string search_path(IsAbsolutePath(opt_search_path) ? opt_search_path : (pass ? options.shared_directory : cwd_path));
					if(!opt_search_path.empty() && !IsAbsolutePath(opt_search_path))
					{
						search_path += separator;
						search_path += opt_search_path;
					}
					// if lazy match = false: try whole match
					// if lazy_match = true: try partial matches from longest to shortest suffixes
					if(options.lazy_match)
					{
						for(int i = 0; i < depth; ++i)
						{
							std::string try_file_path(search_path);
							for(int j = i; j < depth; ++j)
							{
								if(!try_file_path.empty()) try_file_path += separator;
								try_file_path += parts[j];
							}
#if DEBUG_LOCATE_FILE
							std::cerr << "Trying \"" << try_file_path << "\":" << std::endl;
#endif
							if(ResolvePath(try_file_path, match_file_path) && access(match_file_path.c_str(), R_OK) == 0)
							{
#if DEBUG_LOCATE_FILE
								std::cerr << "found" << std::endl;
#endif
								return true;
							}
						}
					}
					else
					{
						std::string try_file_path(search_path);
						try_file_path += separator;
						try_file_path += filename;
#if DEBUG_LOCATE_FILE
						std::cerr << "Trying \"" << try_file_path << "\":" << std::endl;
#endif
						if(ResolvePath(try_file_path, match_file_path) && access(match_file_path.c_str(), R_OK) == 0)
						{
#if DEBUG_LOCATE_FILE
							std::cerr << "found" << std::endl;
#endif
							return true;
						}
					}
				}
			}
		}
	}
	
#if DEBUG_LOCATE_FILE
	std::cerr << "not found" << std::endl;
#endif
	return false;
}

} // end of namespace locate
} // end of namespace util
} // end of namespace unisim

#if 0
int main(int argc, const char *argv[])
{
	std::string r;
	unisim::util::locate::LocateFileOptions o(
		{ "abc", "def" },
		"/path/to/share",
		true
	);
	unisim::util::locate::LocateFile("toto.cc", r, o);
	return 0;
}
#endif
