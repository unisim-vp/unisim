/*
 *  Copyright (c) 2017,
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

#include <unisim/service/debug/profiler/profiler.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access() and mkdir()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

template class Sample<signed char>;
template class Sample<short>;
template class Sample<int>;
template class Sample<long>;
template class Sample<long long>;
template class Sample<unsigned char>;
template class Sample<bool>;
template class Sample<unsigned short>;
template class Sample<unsigned int>;
template class Sample<unsigned long>;
template class Sample<unsigned long long>;
template class Sample<double>;
template class Sample<sc_core::sc_time>;

////////////////////////////// FileFormat /////////////////////////////////////

std::ostream& operator << (std::ostream& os, FileFormat f_fmt)
{
	switch(f_fmt)
	{
		case F_FMT_TEXT: os << "text"; break;
		case F_FMT_HTML: os << "html"; break;
		case F_FMT_CSV: os << "csv"; break;
		case F_FMT_SVG: os << "svg"; break;
		default: os << "?"; break;
	}
	
	return os;
}

std::string FileFormatSuffix(FileFormat f_fmt)
{
	switch(f_fmt)
	{
		case F_FMT_TEXT: return std::string(".txt");
		case F_FMT_HTML: return std::string(".html");
		case F_FMT_CSV: return std::string(".csv");
		case F_FMT_SVG: return std::string(".svg");
		default: return std::string();
	}
	
	return std::string();
}

////////////////////////////// CSV_Reader /////////////////////////////////////

std::ostream& operator << (std::ostream& os, CSV_Reader csv_reader)
{
	switch(csv_reader)
	{
		case MS_EXCEL    : os << "Microsoft Excel"; break;
		case LIBRE_OFFICE: os << "Libre Office"; break;
		default          : os << "Unknown CSV reader"; break;
	}
	return os;
}

///////////////////////////// c_string_to_CSV /////////////////////////////////

std::string c_string_to_CSV(const char *s)
{
	std::stringstream sstr;
	
	sstr << "\"";
	
	char c = *s;

	if(c)
	{
		do
		{
			switch(c)
			{
				case '"':
					sstr << "\"\"";
					break;
				default:
					sstr << c;
			}
		}
		while((c = *(++s)));
	}
	
	sstr << "\"";
	
	return sstr.str();
}

//////////////////////////////// MakeDir //////////////////////////////////////

void MakeDir(const char *dirname)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(dirname);
#else
	mkdir(dirname, S_IRWXU);
#endif
}

/////////////////////////////// RealPath //////////////////////////////////////

std::string RealPath(const char *dirname)
{
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	char dir_path[FILENAME_MAX + 1];
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	char dir_path[PATH_MAX + 1];
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	char *dir_path_pointer = realpath(dirname, dir_path);
	if(dir_path_pointer == dir_path)
	{
		return std::string(dir_path);
	}
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(GetFullPathName(dirname, sizeof(dir_path), dir_path, NULL))
	{
		return std::string(dir_path);
	}
#endif
	return std::string(dirname);
}

//////////////////////////// FilenameIndex ////////////////////////////////////

unsigned int FilenameIndex::IndexFilename(const std::string& filename)
{
	typename std::map<std::string, unsigned int>::const_iterator it = index.find(filename);
	
	if(it != index.end())
	{
		return (*it).second;
	}
	
	unsigned int filename_id = index.size();
	index.insert(std::pair<std::string, unsigned int>(filename, filename_id));
	return filename_id;
}

/////////////////////////////// FileVisitor ////////////////////////////////////

FileVisitor::FileVisitor(const std::string& _output_directory, ReportFormat _r_fmt, const std::string& _csv_delimiter, CSV_Reader _csv_reader, const std::string& _csv_hyperlink, const std::string& _csv_arg_separator, std::ostream& _warn_log)
	: file()
	, output_directory(_output_directory)
	, r_fmt(_r_fmt)
	, csv_delimiter(_csv_delimiter)
	, csv_reader(_csv_reader)
	, csv_hyperlink(_csv_hyperlink)
	, csv_arg_separator(_csv_arg_separator)
	, warn_log(_warn_log)
	, dir_path(output_directory)
{
	MakeDir(output_directory.c_str());
	output_directory = RealPath(output_directory.c_str());
}

const std::string& FileVisitor::GetCSVDelimiter() const
{
	return csv_delimiter;
}

CSV_Reader FileVisitor::GetCSVReader() const
{
	return csv_reader;
}

const std::string& FileVisitor::GetCSVHyperlink() const
{
	return csv_hyperlink;
}

const std::string& FileVisitor::GetCSVArgSeparator() const
{
	return csv_arg_separator;
}

const std::string& FileVisitor::GetRoot() const
{
	static std::string null_str;
	return null_str;
}

const std::string& FileVisitor::GetDomain() const
{
	static std::string null_str;
	return null_str;
}

ReportFormat FileVisitor::GetReportFormat() const
{
	return r_fmt;
}

std::string FileVisitor::GetFilePath(const std::string& filename) const
{
	return unisim::kernel::service::Simulator::Instance()->SearchSharedDataFile(filename.c_str());
}

const std::string& FileVisitor::GetDirPath() const
{
	return dir_path;
}

bool FileVisitor::Visit(const std::string& dirname, const std::string& filename, const Printer& printer)
{
	dir_path = output_directory;
	if(!dirname.empty())
	{
		dir_path += '/';
		dir_path += dirname;
	}
	MakeDir(dir_path.c_str());
	std::string file_path(output_directory);
	if(!dirname.empty())
	{
		file_path += '/';
		file_path += dirname;
	}
	file_path += '/';
	file_path += filename;
	
	file.open(file_path.c_str(), std::ios::out);
	
	if(file.fail())
	{
		warn_log << "Can't create \"" << file_path << "\"" << std::endl;
		return false;
	}
	
	printer.Print(file, *this);
	
	file.close();
	
	return true;
}

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

//////////////////////////// Variable<CSV_Reader> ////////////////////////////////

using unisim::service::debug::profiler::CSV_Reader;
using unisim::service::debug::profiler::LIBRE_OFFICE;
using unisim::service::debug::profiler::MS_EXCEL;

template <> Variable<CSV_Reader>::Variable(const char *_name, Object *_object, CSV_Reader& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
	AddEnumeratedValue("libre-office");
	AddEnumeratedValue("ms-excel");
}

template <>
const char *Variable<CSV_Reader>::GetDataTypeName() const
{
	return "unisim::service::debug::profiler::CSV_Reader";
}

template <>
VariableBase::DataType Variable<CSV_Reader>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<CSV_Reader>::GetBitSize() const
{
	return 1;
}

template <> Variable<CSV_Reader>::operator bool () const { return *storage != MS_EXCEL; }
template <> Variable<CSV_Reader>::operator long long () const { return *storage; }
template <> Variable<CSV_Reader>::operator unsigned long long () const { return *storage; }
template <> Variable<CSV_Reader>::operator double () const { return (double)(*storage); }
template <> Variable<CSV_Reader>::operator std::string () const
{
	switch(*storage)
	{
		case MS_EXCEL: return std::string("ms-excel");
		case LIBRE_OFFICE: return std::string("libre-office");
	}
	return std::string("?");
}

template <> VariableBase& Variable<CSV_Reader>::operator = (bool value)
{
	if(IsMutable())
	{
		CSV_Reader tmp = *storage;
		switch((unsigned int) value)
		{
			case MS_EXCEL:
			case LIBRE_OFFICE:
				tmp = (CSV_Reader)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<CSV_Reader>::operator = (long long value)
{
	if(IsMutable())
	{
		CSV_Reader tmp = *storage;
		switch(value)
		{
			case MS_EXCEL:
			case LIBRE_OFFICE:
				tmp = (CSV_Reader) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<CSV_Reader>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		CSV_Reader tmp = *storage;
		switch(value)
		{
			case MS_EXCEL:
			case LIBRE_OFFICE:
				tmp = (CSV_Reader) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<CSV_Reader>::operator = (double value)
{
	if(IsMutable())
	{
		CSV_Reader tmp = *storage;
		switch((unsigned int) value)
		{
			case MS_EXCEL:
			case LIBRE_OFFICE:
				tmp = (CSV_Reader)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<CSV_Reader>::operator = (const char *value)
{
	if(IsMutable())
	{
		CSV_Reader tmp = *storage;
		if(std::string(value) == std::string("ms-excel")) tmp = MS_EXCEL;
		else if(std::string(value) == std::string("libre-office")) tmp = LIBRE_OFFICE;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<CSV_Reader>;

} // end of service namespace
} // end of kernel namespace
} // end of unisim namespace
