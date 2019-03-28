/*
 *  Copyright (c) 2019,
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

#ifndef __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__
#define __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <pthread.h>

namespace unisim {
namespace service {
namespace web_terminal {

/////////////////////////// Forward declarations //////////////////////////////

struct UTF8_Parser;
struct ScreenBuffer;
struct InputBuffer;

//////////////////////////////// to_string<> //////////////////////////////////

template <typename T>
std::string to_string(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

///////////////////////////////// Enums ///////////////////////////////////////

enum Intensity
{
	DECREASED_INTENSITY = -1,
	NORMAL_INTENSITY    =  0,
	INCREASED_INTENSITY = +1
};

enum Blink
{
	NO_BLINK    = 0,
	SLOW_BLINK  = 1,
	RAPID_BLINK = 2,
};

///////////////////////////// OStreamContext //////////////////////////////////

class OStreamContext
{
public:
	OStreamContext(std::ostream& _os) : os(_os), fill(os.fill()), flags(os.flags()) {}
	~OStreamContext() { os.fill(fill); os.flags(flags); }
private:
	std::ostream& os;
	std::ostream::char_type fill;
	std::ios_base::fmtflags flags;
};

//////////////////////////////// Field<> //////////////////////////////////////

template <typename T, unsigned int BITPOS, unsigned int BITWIDTH>
struct Field
{
	typedef T TYPE;
	
	template <typename V>
	static TYPE Get(const V& packed_value)
	{
		return static_cast<TYPE>((packed_value >> BITPOS) & ((V(1) << BITWIDTH) - 1));
	}
	
	template <typename V>
	static void Set(V& packed_value, const TYPE& field_value)
	{
		packed_value = (packed_value & ~(((V(1) << BITWIDTH) - 1) << BITPOS)) | ((static_cast<V>(field_value) & ((V(1) << BITWIDTH) - 1)) << BITPOS);
	}
};

/////////////////////////////// GraphicRendition //////////////////////////////

struct GraphicRendition
{
	static const char *color_table[16];
	
	GraphicRendition() : packed_value(0) {}
	GraphicRendition(const GraphicRendition& o) : packed_value(o.packed_value) {}
	
	Intensity GetIntensity() const { return Get<INTENSITY>(); }
	bool GetItalic() const { return Get<ITALIC>(); }
	bool GetUnderline() const { return Get<UNDERLINE>(); }
	Blink GetBlink() const { return Get<BLINK>(); }
	bool GetReverse() const { return Get<REVERSE>(); }
	bool GetCrossedOut() const { return Get<CROSSED_OUT>(); }
	unsigned int GetForeground() const { return Get<FOREGROUND>(); }
	unsigned int GetBackground() const { return Get<BACKGROUND>(); }
	
	void SetIntensity(Intensity intensity) { Set<INTENSITY>(intensity); }
	void SetItalic(bool v = true) { Set<ITALIC>(v); }
	void SetUnderline(bool v = true) { Set<UNDERLINE>(v); }
	void SetBlink(Blink blink) { Set<BLINK>(blink); }
	void SetReverse(bool v = true) { Set<REVERSE>(v); }
	void SetCrossedOut(bool v = true) { Set<CROSSED_OUT>(v); }
	void SetForeground(unsigned int fg) { Set<FOREGROUND>(fg); }
	void SetBackground(unsigned int bg) { Set<BACKGROUND>(bg); }
	
	GraphicRendition& operator = (const GraphicRendition& o) { packed_value = o.packed_value; return *this; }
	int operator == (const GraphicRendition& o) const { return packed_value == o.packed_value; }
	int operator != (const GraphicRendition& o) const { return packed_value != o.packed_value; }
	int operator < (const GraphicRendition& o) const { return packed_value < o.packed_value; }
	
	void Reset()
	{
		packed_value = 0;
	}
private:
	typedef Field<Intensity, 0, 2>  INTENSITY;
	typedef Field<bool, 2, 1>  ITALIC;
	typedef Field<bool, 3, 1>  UNDERLINE;
	typedef Field<Blink, 4, 2>  BLINK;
	typedef Field<bool, 6, 1>  REVERSE;
	typedef Field<bool, 7, 1>  CROSSED_OUT;
	typedef Field<unsigned int, 8, 4> FOREGROUND;
	typedef Field<unsigned int, 12, 4> BACKGROUND;
	
	template <typename FIELD>
	typename FIELD::TYPE Get() const
	{
		return FIELD::Get(packed_value);
	}
	
	template <typename FIELD>
	void Set(const typename FIELD::TYPE& field_value)
	{
		FIELD::Set(packed_value, field_value);
	}
	
	uint16_t packed_value;
};

///////////////////////////// UnicodeCharacter ////////////////////////////////

struct UnicodeCharacter
{
	static const uint32_t NAC = 0xDEADBEEF;
	
	UnicodeCharacter() : code_point(NAC) {}
	UnicodeCharacter(const UnicodeCharacter& uc) : code_point(uc.code_point) {}
	UnicodeCharacter(uint32_t _code_point) : code_point(_code_point) {}
	uint32_t GetCodePoint() const { return code_point; }
	
	UnicodeCharacter& operator = (const UnicodeCharacter& uc) { code_point = uc.code_point; return *this; }
	UnicodeCharacter& operator = (uint32_t _code_point) { code_point = _code_point; return *this; }
	UnicodeCharacter& operator = (char c) { code_point = (uint8_t) c; return *this; }
	
	int operator < (const UnicodeCharacter& uc) const { return code_point < uc.code_point; }
	int operator <= (const UnicodeCharacter& uc) const { return code_point <= uc.code_point; }
	int operator == (const UnicodeCharacter& uc) const { return code_point == uc.code_point; }
	int operator >= (const UnicodeCharacter& uc) const { return code_point >= uc.code_point; }
	int operator > (const UnicodeCharacter& uc) const { return code_point > uc.code_point; }
	
	int operator < (uint32_t _code_point) const { return code_point < _code_point; }
	int operator <= (uint32_t _code_point) const { return code_point <= _code_point; }
	int operator == (uint32_t _code_point) const { return code_point == _code_point; }
	int operator >= (uint32_t _code_point) const { return code_point >= _code_point; }
	int operator > (uint32_t _code_point) const { return code_point > _code_point; }
	
	int operator < (char c) const { return code_point < (uint8_t) c; }
	int operator <= (char c) const { return code_point <= (uint8_t) c; }
	int operator == (char c) const { return code_point == (uint8_t) c; }
	int operator >= (char c) const { return code_point >= (uint8_t) c; }
	int operator > (char c) const { return code_point > (uint8_t) c; }

	operator uint32_t () const { return code_point; }
private:
	uint32_t code_point;
	
	friend class UTF8_Parser;
};

std::ostream& operator << (std::ostream& os, const UnicodeCharacter& uc);

////////////////////////////// UTF8_Parser ////////////////////////////////////

struct UTF8_Parser
{
	UTF8_Parser();
	UnicodeCharacter *Parse(uint8_t c);
private:
	int state;
	int n;
	UnicodeCharacter uc;
};

//////////////////////////// ScreenBufferCharacter ////////////////////////////

struct ScreenBufferCharacter
{
	UnicodeCharacter c;
	GraphicRendition gr;
	
	void Erase()
	{
		c = 0u;
		gr.Reset();
	}
};

///////////////////////////// ScreenBufferLine ////////////////////////////////

struct ScreenBufferLine
{
	unsigned int display_width;
	ScreenBufferCharacter *storage;
	
	ScreenBufferLine(unsigned int _display_width)
		: display_width(_display_width)
		, storage(new ScreenBufferCharacter[display_width])
	{
		Erase();
	}
	
	~ScreenBufferLine()
	{
		delete storage;
	}
	
	void Erase()
	{
		memset(storage, 0, display_width * sizeof(ScreenBufferCharacter));
	}
	
	ScreenBufferCharacter& operator [] (unsigned int colno)
	{
		return storage[colno - 1];
	}
};

///////////////////////////// ScreenBufferIterator ////////////////////////////

struct ScreenBufferIterator
{
	ScreenBufferIterator(const ScreenBufferIterator& o);
	ScreenBufferIterator& operator = (const ScreenBufferIterator& o);
	ScreenBufferIterator& operator ++ ();
	int operator == (const ScreenBufferIterator& o) const;
	int operator != (const ScreenBufferIterator& o) const;
	ScreenBufferLine& operator * ();
	
private:
	friend struct ScreenBuffer;
	
	ScreenBuffer *screen_buffer;
	unsigned int scan_index;
	unsigned int line_index;
	
	ScreenBufferIterator(ScreenBuffer *_screen_buffer, unsigned int _scan_index, unsigned int _line_index);
	void Copy(const ScreenBufferIterator& o);
	void Set(unsigned int _scan_index, unsigned int _line_index);
};

//////////////////////////////// ScreenBuffer /////////////////////////////////

struct ScreenBuffer
{
//         index
//   
//           0        --> +--------------------------------+        +<-+
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        |  |
// window_start_index --> +--------------------------------+  <--+  |  |      ^
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//                        |                                |  V  |  |  |      |
//     top_line_index --> +--------------------------------+  +  |  |  |      |
//                        |                                |  |  |  |  |      | display_height
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//                        |                                |  |  |  |  |      |
//   window_end_index --> +--------------------------------+  +-->  |  |      V
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        |  |
//                        |                                |        V  |
//    max_height - 1  --> +--------------------------------+        +-->

	unsigned int display_width;
	unsigned int display_height;
	unsigned int height;
	unsigned int max_height;
	unsigned int cursor_colno;
	unsigned int cursor_lineno;
	unsigned int save_cursor_colno;
	unsigned int save_cursor_lineno;
	typedef std::vector<ScreenBufferLine *> ScreenBufferLines;
	unsigned int window_start_index;
	unsigned int top_line_index;
	unsigned int window_end_index;
	ScreenBufferIterator begin_it;
	ScreenBufferIterator end_it;
	ScreenBufferLines lines;
	GraphicRendition gr;
	UTF8_Parser utf8_parser;
	
	ScreenBuffer(unsigned int _display_width, unsigned int _display_height, unsigned int _max_height);
	~ScreenBuffer();
	
	const ScreenBufferIterator& Begin() const;
	const ScreenBufferIterator& End() const;
	unsigned int Next(unsigned int line_index) const;
	
	unsigned int GetCursorColNo() const;
	unsigned int GetCursorLineNo() const;

	void CarriageReturn();
	void LineFeed();
	
	ScreenBufferLine& GetLine(unsigned int line_index);
	bool IsAtCursorLinePos(const ScreenBufferIterator& it) const;
	unsigned int TranslateLineNoToLineIndex(unsigned int lineno) const;
	ScreenBufferLine& operator [] (unsigned int lineno);
	
	void PutChar(char c);
	void MoveCursorUp(unsigned int n);
	void MoveCursorDown(unsigned int n);
	void MoveCursorForward(unsigned int n);
	void MoveCursorBack(unsigned int n);
	void MoveCursorNextLine(unsigned int n);
	void MoveCursorPreviousLine(unsigned int n);
	void MoveCursorHorizontalAbsolute(unsigned int colno);
	void MoveCursorTo(unsigned int colno, unsigned int lineno);
	void EraseInDisplay(unsigned int n);
	void EraseInLine(unsigned int n);
	void Reserve(unsigned int line_index);
	void NewLine();
	void ScrollUp(unsigned int n);
	void ScrollDown(unsigned int n);
	void SelectGraphicsRendition(unsigned int n);
	void ReportDeviceStatus(InputBuffer& input_buffer);
	void SaveCursorPosition();
	void RestoreCursorPosition();
};

///////////////////////////////// InputBuffer /////////////////////////////////

struct InputBuffer
{
	InputBuffer(unsigned int _size)
		: size(_size)
		, front_index(0)
		, back_index(0)
		, fill_level(0)
		, storage(size)
	{
	}
	
	void Push(char c)
	{
		if(fill_level != size)
		{
// 			std::cerr << "Pushing " << (unsigned int)(unsigned char) c << std::endl;
			storage[back_index] = c;
			++back_index;
			if(back_index >= size) back_index = 0;
			fill_level++;
		}
	}
	
	void Push(const char *s)
	{
		char c = *s++;
		if(c)
		{
			do
			{
				Push(c);
				c = *s++;
			}
			while(c);
		}
	}
	
	void Push(const std::string& s)
	{
		std::size_t n = s.length();
		for(std::size_t i = 0; i < n; i++)
		{
			Push(s[i]);
		}
	}
	
	bool Empty() const
	{
		return fill_level == 0;
	}
	
	bool Full() const
	{
		return fill_level == size;
	}
	
	char Front() const
	{
		return storage[front_index];
	}
	
	void Pop()
	{
		if(fill_level)
		{
			++front_index;
			if(front_index >= size) front_index = 0;
			fill_level--;
		}
	}
	
	unsigned int size;
	unsigned int front_index;
	unsigned int back_index;
	unsigned int fill_level;
	std::vector<char> storage;
};

/////////////////////////////////// KeyEvent //////////////////////////////////

struct KeyEvent
{
	KeyEvent() : key(), csam(0) {}
	
	KeyEvent(const std::string& _key)
		: key(_key)
		, csam(0)
	{
	}
	
	KeyEvent& KEY(const std::string& _key) { key = _key; return *this; }
	KeyEvent& CTRL(bool v = true) { Set<CTRL_FIELD>(v); return *this; }
	KeyEvent& SHIFT(bool v = true) { Set<SHIFT_FIELD>(v); return *this; }
	KeyEvent& ALT(bool v = true) { Set<ALT_FIELD>(v); return *this; }
	KeyEvent& META(bool v = true) { Set<META_FIELD>(v); return *this; }
	
	const std::string& GetKey() const { return key; }
	bool GetCTRL() const { return Get<CTRL_FIELD>(); }
	bool GetSHIFT() const { return Get<SHIFT_FIELD>(); }
	bool GetALT() const { return Get<ALT_FIELD>(); }
	bool GetMETA() const { return Get<META_FIELD>(); }
	
	int operator < (const KeyEvent& o) const { return (key < o.key) || ((key >= o.key) && (csam < o.csam)); }
private:
	std::string key;
	uint8_t csam;
	
	typedef Field<bool, 0, 1>  CTRL_FIELD;
	typedef Field<bool, 1, 1>  SHIFT_FIELD;
	typedef Field<bool, 2, 1>  ALT_FIELD;
	typedef Field<bool, 3, 1>  META_FIELD;

	template <typename FIELD>
	typename FIELD::TYPE Get() const
	{
		return FIELD::Get(csam);
	}

	template <typename FIELD>
	void Set(const typename FIELD::TYPE& field_value)
	{
		FIELD::Set(csam, field_value);
	}
};

inline std::ostream& operator << (std::ostream& os, const KeyEvent& key_event)
{
	return os << "KeyEvent(key=\"" << key_event.GetKey() << "\",ctrl=" << key_event.GetCTRL() << ",shift=" << key_event.GetSHIFT() << ",alt=" << key_event.GetALT() << ",meta=" << key_event.GetMETA() << ")";
}

///////////////////////////////// WebTerminal /////////////////////////////////

class WebTerminal
	: public unisim::kernel::service::Service<unisim::service::interfaces::CharIO>
	, public unisim::kernel::service::Service<unisim::service::interfaces::HttpServer>
{
public:
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	WebTerminal(const char *name, unisim::kernel::service::Object *parent = 0);
	virtual ~WebTerminal();
	
	virtual bool BeginSetup();
	
	// unisim::service::interfaces::CharIO
	virtual void Reset();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
	
	// unisim::service::interfaces::HttpServer
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
	
private:
	unisim::kernel::logger::Logger logger;

	bool verbose;
	unisim::kernel::service::Parameter<bool> param_verbose;
	unsigned int display_width;
	unisim::kernel::service::Parameter<unsigned int> param_display_width;
	unsigned int display_height;
	unisim::kernel::service::Parameter<unsigned int> param_display_height;
	unsigned int buffer_height;
	unisim::kernel::service::Parameter<unsigned int> param_buffer_height;
	unsigned int input_buffer_size;
	unisim::kernel::service::Parameter<unsigned int> param_input_buffer_size;
	double min_display_refresh_period;
	unisim::kernel::service::Parameter<double> param_min_display_refresh_period;
	double max_display_refresh_period;
	unisim::kernel::service::Parameter<double> param_max_display_refresh_period;
	std::string title;
	unisim::kernel::service::Parameter<std::string> param_title;
	bool implicit_cr_in_every_lf;
	unisim::kernel::service::Parameter<bool> param_implicit_cr_in_every_lf;
	
	bool activity;
	double refresh_period;
	int state;
	bool got_arg;
	unsigned int arg;
	unsigned int num_args;
	unsigned int args[2];
	ScreenBuffer screen_buffer;
//	ScreenBuffer alt_screen_buffer;
	InputBuffer input_buffer;

	typedef std::map<KeyEvent, std::string> KEVT2ANSI;
	KEVT2ANSI kevt2ansi;
	
	pthread_mutex_t mutex;
	
	void Receive(const KeyEvent& key_event);
	void Lock();
	void Unlock();
};

} // end of namespace web_terminal
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__
