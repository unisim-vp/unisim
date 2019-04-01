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

#include <unisim/service/web_terminal/web_terminal.hh>
#include <set>

namespace unisim {
namespace service {
namespace web_terminal {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/////////////////////////////// GraphicRendition //////////////////////////////

const char *GraphicRendition::color_table[16] = {
	/*  0: black          */ "rgb(0,0,0)",
	/*  1: red            */ "rgb(170,0,0)",
	/*  2: green          */ "rgb(0,170,0)",
	/*  3: yellow         */ "rgb(170,85,0)",
	/*  4: blue           */ "rgb(0,0,170)",
	/*  5: magenta        */ "rgb(170,0,170)",
	/*  6: cyan           */ "rgb(0,170,170)",
	/*  7: white          */ "rgb(170,170,170)",
	/*  8: bright black   */ "rgb(85,85,85)",
	/*  9: bright red     */ "rgb(255,85,85)",
	/* 10: bright green   */ "rgb(85,255,85)",
	/* 11: bright yellow  */ "rgb(255,255,85)",
	/* 12: bright blue    */ "rgb(85,85,255)",
	/* 13: bright magenta */ "rgb(255,85,255)",
	/* 14: bright cyan    */ "rgb(85,255,255)",
	/* 15: bright white   */ "rgb(255,255,255)"
};

////////////////////////////// UTF8_Parser ////////////////////////////////////

UTF8_Parser::UTF8_Parser()
	: state(0)
	, n(0)
	, uc()
{
}

UnicodeCharacter *UTF8_Parser::Parse(uint8_t c)
{
	switch(state)
	{
		case 0:
			if(c < 128)
			{
				uc.code_point = c;
				return &uc;
			}

			if((c & 0xe0) == (0xc0))
			{
				// 2-byte UTF-8 character
				uc.code_point = c & 0x1f;
				n = 1;
				state = 1;
			}
			else if((c & 0xf0) == (0xe0))
			{
				// 3-byte UTF-8 character
				uc.code_point = c & 0xf;
				n = 2;
				state = 1;
			}
			else if((c & 0xf8) == (0xf0))
			{
				// 4-byte UTF-8 character
				uc.code_point = c & 0x7;
				n = 3;
				state = 1;
			}
			
			break;
			
		case 1:
			if((c & 0xc0) == 0x80)
			{
				// n-th byte
				uc.code_point = (uc.code_point << 6) | (c & 0x3f);
				if(--n == 0)
				{
					state = 0;
					return &uc;
				}
			}
			else
			{
				state = 0;
			}
			break;
	}
	
	return 0;
}

///////////////////////////// UnicodeCharacter ////////////////////////////////

std::ostream& operator << (std::ostream& os, const UnicodeCharacter& uc)
{
	uint32_t code_point = uc.GetCodePoint();
	if(code_point <= 0x7f)
	{
		// 1-byte UTF-8 character
		os << (char) code_point;
	}
	else if(code_point <= 0x7ff)
	{
		// 2-byte UTF-8 character: xxxxxyyyyyy => 110xxxxx  10yyyyyy
		uint16_t v = 0xc080 | ((code_point << 2) & 0x1f00) | (code_point & 0x3f);
		os << (char)(v >> 8) << (char) v;
	}
	else if(code_point <= 0xffff)
	{
		// 3-byte UTF-8 character: xxxxyyyyyyzzzzzz => 1110xxxx 10yyyyyy 10zzzzzz
		uint32_t v = 0xe08080 | ((code_point << 4) & 0x1f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		os << (char)(v >> 16) << (char)(v >> 8) << (char) v;
	}
	else if(code_point <= 0x10ffff)
	{
		// 4-byte UTF-8 character: wwwxxxxxxyyyyyyzzzzzz => 11110www 10xxxxxx 10yyyyyy 10zzzzzz
		uint32_t v = 0xf0808080 | ((code_point << 6) & 0x7000000) | ((code_point << 4) & 0x3f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		os << (char)(v >> 24) << (char)(v >> 16) << (char)(v >> 8) << (char) v;
	}
	
	return os;
}

///////////////////////////// ScreenBufferLine ////////////////////////////////

ScreenBufferLine::ScreenBufferLine(unsigned int _display_width)
	: display_width(_display_width)
	, storage(new ScreenBufferCharacter[display_width])
{
	Erase();
}

ScreenBufferLine::~ScreenBufferLine()
{
	delete storage;
}

void ScreenBufferLine::Erase()
{
	memset(storage, 0, display_width * sizeof(ScreenBufferCharacter));
}

ScreenBufferCharacter& ScreenBufferLine::operator [] (unsigned int colno)
{
	return storage[colno - 1];
}

///////////////////////////// ScreenBufferIterator ////////////////////////////

ScreenBufferIterator::ScreenBufferIterator(const ScreenBufferIterator& o)
{
	Copy(o);
}

ScreenBufferIterator& ScreenBufferIterator::operator = (const ScreenBufferIterator& o)
{
	Copy(o);
	return *this;
}

ScreenBufferIterator& ScreenBufferIterator::operator ++ ()
{
//	std::cerr << scan_index << "," << line_index << " => ";
	++scan_index;
	line_index = screen_buffer->Next(line_index);
//	std::cerr << scan_index << "," << line_index << std::endl;
	return *this;
}

int ScreenBufferIterator::operator == (const ScreenBufferIterator& o) const
{
	return (screen_buffer == o.screen_buffer) && (scan_index == o.scan_index);
}

int ScreenBufferIterator::operator != (const ScreenBufferIterator& o) const
{
	return (screen_buffer != o.screen_buffer) || (scan_index != o.scan_index);
}

ScreenBufferLine& ScreenBufferIterator::operator * ()
{
	return screen_buffer->GetLine(line_index);
}

ScreenBufferIterator::ScreenBufferIterator(ScreenBuffer *_screen_buffer, unsigned int _scan_index, unsigned int _line_index)
	: screen_buffer(_screen_buffer)
	, scan_index(_scan_index)
	, line_index(_line_index)
{
}

void ScreenBufferIterator::Copy(const ScreenBufferIterator& o)
{
	screen_buffer = o.screen_buffer;
	scan_index = o.scan_index;
	line_index = o.line_index;
}

void ScreenBufferIterator::Set(unsigned int _scan_index, unsigned int _line_index)
{
	scan_index = _scan_index;
	line_index = _line_index;
}

//////////////////////////////// ScreenBuffer /////////////////////////////////

ScreenBuffer::ScreenBuffer(unsigned int _display_width, unsigned int _display_height, unsigned int _max_height)
	: display_width(_display_width)
	, display_height(_display_height)
	, height(display_height)
	, max_height(_max_height)
	, cursor_colno(1)
	, cursor_lineno(1)
	, save_cursor_colno(1)
	, save_cursor_lineno(1)
	, window_start_index(0)
	, top_line_index(0)
	, window_end_index(height)
	, begin_it(this, 0, 0)
	, end_it(this, height, window_end_index)
	, lines()
	, gr()
	, utf8_parser()
{
	lines.reserve(max_height);
	for(unsigned int i = 0; i < height; i++)
	{
		lines.push_back(new ScreenBufferLine(display_width));
	}
	gr.SetForeground(7);
	gr.SetBackground(0);
}

ScreenBuffer::~ScreenBuffer()
{
	for(unsigned int i = 0; i < height; i++)
	{
		ScreenBufferLine *line = lines[i];
		delete line;
	}
}

const ScreenBufferIterator& ScreenBuffer::Begin() const
{
	return begin_it;
}

const ScreenBufferIterator& ScreenBuffer::End() const
{
	return end_it;
}

unsigned int ScreenBuffer::Next(unsigned int line_index) const
{
	assert(line_index < height); 
	++line_index;
	if(line_index == height) line_index = 0;
	if(line_index == window_start_index) return top_line_index;
	if(line_index == window_end_index) return window_start_index;
	if(line_index == top_line_index) return height;
	return line_index;
}

unsigned int ScreenBuffer::GetCursorColNo() const
{
	return cursor_colno;
}

unsigned int ScreenBuffer::GetCursorLineNo() const
{
	return cursor_lineno;
}

void ScreenBuffer::CarriageReturn()
{
	cursor_colno = 1;
}

void ScreenBuffer::LineFeed()
{
	if(cursor_lineno < display_height)
	{
		cursor_lineno++;
	}
	else
	{
		NewLine();
	}
}

ScreenBufferLine& ScreenBuffer::GetLine(unsigned int line_index)
{
	assert(line_index < height);
//	std::cerr << "GetLine(" << line_index << ")" << std::endl;
	return *lines[line_index];
}

bool ScreenBuffer::IsAtCursorLinePos(const ScreenBufferIterator& it) const
{
	return it.line_index == TranslateLineNoToLineIndex(cursor_lineno);
}

unsigned int ScreenBuffer::TranslateLineNoToLineIndex(unsigned int lineno) const
{
	assert((lineno > 0) && (lineno <= display_height));
	unsigned int line_index = top_line_index;
	for(unsigned int i = 1; i < lineno; i++, line_index = Next(line_index));

	unsigned int d = (top_line_index < window_end_index) ? (window_end_index - top_line_index) : (height - top_line_index + window_end_index);
	unsigned int line_index2 = ((lineno - 1) < d) ? (top_line_index + lineno - 1) : (lineno - d + window_start_index);
	if(line_index2 >= height) line_index2 -= height;
	assert(line_index2 == line_index);
	assert(line_index < height);
	return line_index;
}

ScreenBufferLine& ScreenBuffer::operator [] (unsigned int lineno)
{
	return *lines[TranslateLineNoToLineIndex(lineno)];
}

void ScreenBuffer::PutChar(char c)
{
	UnicodeCharacter *uc = utf8_parser.Parse(c);
	if(uc)
	{
// 		{
// 			OStreamContext(std::cerr);
// 			std::cerr << "got U+";
// 			std::cerr.fill('0');
// 			std::cerr.width(4);
// 			std::cerr << std::hex << uc->GetCodePoint() << std::endl;
// 		}
		ScreenBufferLine& sbl = (*this)[cursor_lineno];
		ScreenBufferCharacter& sbc = sbl[cursor_colno];
		sbc.c = *uc;
		sbc.gr = gr;
		cursor_colno++;
		if(cursor_colno > display_width)
		{
			CarriageReturn();
			LineFeed();
		}
	}
}

void ScreenBuffer::MoveCursorUp(unsigned int n)
{
	if((n + 1) < cursor_lineno)
	{
		cursor_lineno -= n;
	}
	else
	{
		cursor_lineno = 1;
	}
}

void ScreenBuffer::MoveCursorDown(unsigned int n)
{
	if(n < (display_height - cursor_lineno))
	{
		cursor_lineno += n;
	}
	else
	{
		cursor_lineno = display_height;
	}
}

void ScreenBuffer::MoveCursorForward(unsigned int n)
{
	if(n < (display_width - cursor_colno))
	{
		cursor_colno += n;
	}
	else
	{
		cursor_colno = display_width;
	}
}

void ScreenBuffer::MoveCursorBack(unsigned int n)
{
	if((n + 1) < cursor_colno)
	{
		cursor_colno -= n;
	}
	else
	{
		cursor_colno = 1;
	}
}

void ScreenBuffer::MoveCursorNextLine(unsigned int n)
{
	MoveCursorDown(n);
	cursor_colno = 1;
}

void ScreenBuffer::MoveCursorPreviousLine(unsigned int n)
{
	MoveCursorUp(n);
	cursor_colno = 1;
}

void ScreenBuffer::MoveCursorHorizontalAbsolute(unsigned int colno)
{
	cursor_colno = std::min(colno, display_width);
}

void ScreenBuffer::MoveCursorTo(unsigned int colno, unsigned int lineno)
{
	cursor_colno = std::min(colno, display_width);
	cursor_lineno = std::min(lineno, display_height);
}

void ScreenBuffer::EraseInDisplay(unsigned int n)
{
	/* Clears part of the screen.
		* If n is 0 (or missing), clear from cursor to end of screen.
		* If n is 1, clear from cursor to beginning of the screen.
		* If n is 2, clear entire screen (and moves cursor to upper left on DOS ANSI.SYS).
		* If n is 3, clear entire screen and delete all lines saved in the scrollback buffer (this feature was added for xterm and is supported by other terminal applications).
		*/ 
	if(n > 3) return;
	int inc = (n != 1) ? +1 : -1;
	unsigned int start = (n & 2) ? 1 : cursor_lineno;
	unsigned int end = (n & 2) ? ((n & 1) ? (height + 1) : (display_height + 1)) : ((n & 1) ? 0 : (display_height + 1));
	for(unsigned int lineno = start; lineno != end; lineno += inc)
	{
		(*this)[lineno].Erase();
	}
}

void ScreenBuffer::EraseInLine(unsigned int n)
{
	if(n > 2) return;
	ScreenBufferLine& sbl = (*this)[cursor_lineno];
	/* Erases part of the line.
		* If n is 0 (or missing), clear from cursor to the end of the line.
		* If n is 1, clear from cursor to beginning of the line.
		* If n is 2, clear entire line. Cursor position does not change.
		*/
	unsigned int start = n ? 1 : cursor_colno;
	unsigned int end = ((n != 1) ? display_width : cursor_colno) + 1;
	memset(&sbl[start], 0, (end - start) * sizeof(ScreenBufferCharacter));
}

void ScreenBuffer::Reserve(unsigned int line_index)
{
	if(line_index >= height)
	{
		do
		{
			lines.push_back(new ScreenBufferLine(display_width));
			height++;
		}
		while(line_index >= height);
	}
}

void ScreenBuffer::NewLine()
{
	Reserve(window_end_index);
	ScreenBufferLine *new_line = lines[window_end_index];
	new_line->Erase();

	++window_start_index;
	if(window_start_index >= max_height) window_start_index = 0;
	++top_line_index;
	if(top_line_index >= max_height) top_line_index = 0;
	++window_end_index;
	if(window_end_index >= max_height) window_end_index = 0;
	begin_it.Set(0, (height < max_height) ? 0 : window_end_index);
	end_it.Set(height, window_end_index);
}

void ScreenBuffer::ScrollUp(unsigned int n)
{
	for(unsigned int i = 0; i < n; i++)
	{
		ScreenBufferLine *new_line = lines[top_line_index];
		new_line->Erase();

		++top_line_index;
		if(top_line_index == height) top_line_index = 0;
		if(top_line_index == window_end_index) top_line_index = window_start_index;
	}
}

void ScreenBuffer::ScrollDown(unsigned int n)
{
	for(unsigned int i = 0; i < n; i++)
	{
		if(top_line_index == window_start_index) top_line_index = (window_end_index > 0) ? (window_end_index - 1) : (height - 1);
		else if(top_line_index == 0) top_line_index = height - 1;
		else --top_line_index;

		ScreenBufferLine *new_line = lines[top_line_index];
		new_line->Erase();
	}
}

void ScreenBuffer::SelectGraphicsRendition(unsigned int n)
{
	switch(n)
	{
		case 0:
			gr.Reset();
			break;
		case 1:
			gr.SetIntensity(INCREASED_INTENSITY);
			break;
		case 2:
			gr.SetIntensity(DECREASED_INTENSITY);
			break;
		case 3:
			gr.SetItalic();
			break;
		case 4:
			gr.SetUnderline();
			break;
		case 5:
			gr.SetBlink(SLOW_BLINK);
			break;
		case 6:
			gr.SetBlink(RAPID_BLINK);
			break;
		case 7:
			gr.SetReverse();
			break;
		case 9:
			gr.SetCrossedOut();
			break;
		case 22:
			gr.SetIntensity(NORMAL_INTENSITY);
			break;
		case 23:
			gr.SetItalic(false);
			break;
		case 24:
			gr.SetUnderline(false);
			break;
		case 25:
			gr.SetBlink(NO_BLINK);
			break;
		case 27:
			gr.SetReverse(false);
			break;
		case 29:
			gr.SetCrossedOut(false);
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			gr.SetForeground(n - 30);
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			gr.SetBackground(n - 40);
			break;
	}
}

void ScreenBuffer::ReportDeviceStatus(InputBuffer& input_buffer)
{
	std::stringstream sstr;
	sstr << "\x1b[" << cursor_lineno << ";" << cursor_colno;
	std::string device_status(sstr.str());
	
	input_buffer.Push(device_status);
}

void ScreenBuffer::SaveCursorPosition()
{
	save_cursor_colno = cursor_colno;
	save_cursor_lineno = cursor_lineno;
}

void ScreenBuffer::RestoreCursorPosition()
{
	cursor_colno = save_cursor_colno;
	cursor_lineno = save_cursor_lineno;
}

///////////////////////////////// InputBuffer /////////////////////////////////

InputBuffer::InputBuffer(unsigned int _size)
	: size(_size)
	, front_index(0)
	, back_index(0)
	, fill_level(0)
	, storage(size)
{
}

void InputBuffer::Push(char c)
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

void InputBuffer::Push(const char *s)
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

void InputBuffer::Push(const std::string& s)
{
	std::size_t n = s.length();
	for(std::size_t i = 0; i < n; i++)
	{
		Push(s[i]);
	}
}

bool InputBuffer::Empty() const
{
	return fill_level == 0;
}

bool InputBuffer::Full() const
{
	return fill_level == size;
}

char InputBuffer::Front() const
{
	return storage[front_index];
}

void InputBuffer::Pop()
{
	if(fill_level)
	{
		++front_index;
		if(front_index >= size) front_index = 0;
		fill_level--;
	}
}

///////////////////////////////// WebTerminal /////////////////////////////////

WebTerminal::WebTerminal(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent, "An ANSI Terminal over HTTP")
	, unisim::kernel::service::Service<unisim::service::interfaces::CharIO>(name, parent)
	, unisim::kernel::service::Service<unisim::service::interfaces::HttpServer>(name, parent)
	, char_io_export("char-io-export", this)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, display_width(80)
	, param_display_width("display-width", this, display_width, "Number of displayed columns")
	, display_height(25)
	, param_display_height("display-height", this, display_height, "Number of displayed lines")
	, buffer_height(4 * display_height)
	, param_buffer_height("buffer-height", this, buffer_height, "Number of lines in buffer")
	, input_buffer_size(256)
	, param_input_buffer_size("input-buffer-size", this, input_buffer_size, "Input buffer size")
	, min_display_refresh_period(0.125)
	, param_min_display_refresh_period("min-display-refresh-period", this, min_display_refresh_period, "Minimum refresh period of display (in seconds)")
	, max_display_refresh_period(1.0)
	, param_max_display_refresh_period("max-display-refresh-period", this, max_display_refresh_period, "Maximum refresh period of display (in seconds)")
	, title(GetName())
	, param_title("title", this, title, "Title of this web terminal instance")
	, implicit_cr_in_every_lf(true)
	, param_implicit_cr_in_every_lf("implicit-cr-in-every-lf", this, implicit_cr_in_every_lf, "Implicit Carriage Return (CR, '\\r') in every Line Feed (LF, '\\n')")
	, activity(false)
	, refresh_period(min_display_refresh_period)
	, state(0)
	, screen_buffer(display_width, display_height, std::max(buffer_height, display_height))
	, input_buffer(input_buffer_size)
	, kevt2ansi()
	, mutex()
{
	pthread_mutex_init(&mutex, NULL);
	
	param_display_width.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_display_height.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_buffer_height.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_input_buffer_size.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	kevt2ansi[KeyEvent("Enter")] = "\r\n";
	kevt2ansi[KeyEvent("Backspace")] = "\x7f";
	kevt2ansi[KeyEvent("Delete")] = "\x1b[3~";
	kevt2ansi[KeyEvent("Tab")] = "\t";
	kevt2ansi[KeyEvent("ArrowUp")] = "\x1b[A";
	kevt2ansi[KeyEvent("ArrowDown")] = "\x1b[B";
	kevt2ansi[KeyEvent("ArrowRight")] = "\x1b[C";
	kevt2ansi[KeyEvent("ArrowLeft")] = "\x1b[D";
	kevt2ansi[KeyEvent("PageUp")] = "\x1b[5~";
	kevt2ansi[KeyEvent("PageDown")] = "\x1b[6~";
	kevt2ansi[KeyEvent("Home")] = "\x1b[1~";// "\x1b[H";
	kevt2ansi[KeyEvent("End")] = "\x1b[4~"; //"\x1b[F";
	kevt2ansi[KeyEvent("Escape")] = "\x1b";
	kevt2ansi[KeyEvent("Insert")] = "\x1b[2~";
	kevt2ansi[KeyEvent("F1")] = "\x1b[OP";
	kevt2ansi[KeyEvent("F2")] = "\x1b[OQ";
	kevt2ansi[KeyEvent("F3")] = "\x1b[OR";
	kevt2ansi[KeyEvent("F4")] = "\x1b[OS";
	kevt2ansi[KeyEvent("F5")] = "\x1b[15~";
	kevt2ansi[KeyEvent("F6")] = "\x1b[17~";
	kevt2ansi[KeyEvent("F7")] = "\x1b[18~";
	kevt2ansi[KeyEvent("F8")] = "\x1b[19~";
	kevt2ansi[KeyEvent("F9")] = "\x1b[20~";
	kevt2ansi[KeyEvent("F10")] = "\x1b[21~";
	kevt2ansi[KeyEvent("F11")] = "\x1b[23~";
	kevt2ansi[KeyEvent("F12")] = "\x1b[24~";
	kevt2ansi[KeyEvent("F13")] = "\x1b[25~";
	kevt2ansi[KeyEvent("F14")] = "\x1b[26~";
	kevt2ansi[KeyEvent("F15")] = "\x1b[28~";
	kevt2ansi[KeyEvent("F16")] = "\x1b[29~";
	kevt2ansi[KeyEvent("F17")] = "\x1b[31~";
	kevt2ansi[KeyEvent("F18")] = "\x1b[32~";
	kevt2ansi[KeyEvent("F19")] = "\x1b[33~";
	kevt2ansi[KeyEvent("F20")] = "\x1b[34~";
	kevt2ansi[KeyEvent("a").CTRL()] = "\x1";
	kevt2ansi[KeyEvent("b").CTRL()] = "\x2";
	kevt2ansi[KeyEvent("c").CTRL()] = "\x3";
	kevt2ansi[KeyEvent("d").CTRL()] = "\x4";
	kevt2ansi[KeyEvent("e").CTRL()] = "\x5";
	kevt2ansi[KeyEvent("f").CTRL()] = "\x6";
	kevt2ansi[KeyEvent("g").CTRL()] = "\x7";
	kevt2ansi[KeyEvent("h").CTRL()] = "\x8";
	kevt2ansi[KeyEvent("i").CTRL()] = "\x9";
	kevt2ansi[KeyEvent("j").CTRL()] = "\xa";
	kevt2ansi[KeyEvent("k").CTRL()] = "\xb";
	kevt2ansi[KeyEvent("l").CTRL()] = "\xc";
	kevt2ansi[KeyEvent("m").CTRL()] = "\n\r";
	kevt2ansi[KeyEvent("n").CTRL()] = "\xe";
	kevt2ansi[KeyEvent("o").CTRL()] = "\xf";
	kevt2ansi[KeyEvent("p").CTRL()] = "\x10";
	kevt2ansi[KeyEvent("q").CTRL()] = "\x11";
	kevt2ansi[KeyEvent("r").CTRL()] = "\x12";
	kevt2ansi[KeyEvent("s").CTRL()] = "\x13";
	kevt2ansi[KeyEvent("t").CTRL()] = "\x14";
	kevt2ansi[KeyEvent("u").CTRL()] = "\x15";
	kevt2ansi[KeyEvent("v").CTRL()] = "\x16";
	kevt2ansi[KeyEvent("w").CTRL()] = "\x17";
	kevt2ansi[KeyEvent("x").CTRL()] = "\x18";
	kevt2ansi[KeyEvent("y").CTRL()] = "\x19";
	kevt2ansi[KeyEvent("z").CTRL()] = "\x1a";

}

bool WebTerminal::BeginSetup()
{
	// Ensure that buffer is tall enough for display
	buffer_height = std::max(display_height, buffer_height);
	return true;
}

WebTerminal::~WebTerminal()
{
	pthread_mutex_destroy(&mutex);
}

// unisim::service::interfaces::CharIO
void WebTerminal::Reset()
{
}

bool WebTerminal::GetChar(char& c)
{
	if(!input_buffer.Empty())
	{
		c = input_buffer.Front();
		input_buffer.Pop();
		
		if(!input_buffer.Empty()) activity = true;
		return true;
	}
	return false;
}

void WebTerminal::Receive(const KeyEvent& key_event)
{
	activity = true;
	
	KEVT2ANSI::const_iterator it = kevt2ansi.find(key_event);
	if(it != kevt2ansi.end())
	{
		const std::string& sequence = (*it).second;
		
// 		std::cerr << key_event << " => \"";
		for(std::size_t i = 0; i < sequence.length(); i++)
		{
// 			char c = sequence[i];
// 			if((c >= ' ') && (c <= '~'))
// 			{
// 				std::cerr << c;
// 			}
// 			else
// 			{
// 				OStreamContext osc(std::cerr);
// 				std::cerr << "\\x" << std::hex;
// 				std::cerr.fill('0');
// 				std::cerr.width(2);
// 				std::cerr << (unsigned int) c;
// 			}
		}
		std::cerr << "\"" << std::endl;
		input_buffer.Push(sequence);
		return;
	}
	
	const std::string& key = key_event.GetKey();
	std::size_t n = key.length();
	if((n >= 1) && (n <= 4))
	{
		// maybe ASCII (not extended) or UTF-8
		char c = key[0];
		if((((unsigned char) c < 128) && (n == 1)) || (((unsigned char) c >= 128) && (n > 1)))
		{
			input_buffer.Push(c);
			for(std::size_t i = 1; i < n; i++)
			{
				c = key[i];
				input_buffer.Push(c);
			}
			return;
		}
	}
	
	if(verbose)
	{
		logger << DebugInfo << key_event << " is not recognized" << EndDebugInfo;
	}
}

void WebTerminal::PutChar(char c)
{
	Lock();
	activity = true;
	switch(state)
	{
		case 1:
			if(c == '[')
			{
				// got CSI
				state = 2;
				num_args = 0;
				got_arg = false;
				arg = 0;
				break;
			}
			
			// ignore previous ESC, process character as usual
			state = 0;
			
		case 0:
			if(c == 0x1b) // ESC ?
			{
				state = 1;
				break;
			}
			
			if(c == '\r')
			{
				screen_buffer.CarriageReturn();
				break;
			}
			
			if(c == '\n')
			{
				if(implicit_cr_in_every_lf) screen_buffer.CarriageReturn();
				screen_buffer.LineFeed();
				break;
			}
			
			if(c == '\t')
			{
				for(unsigned int i = 0; i < 8; i++)
				{
					screen_buffer.PutChar(0);
				}
			}
			
			screen_buffer.PutChar(c);
			break;
			
		case 2:
			if((c >= '0') && (c <= '9'))
			{
				arg = (arg * 10) + (c - '0');
				got_arg = true;
			}
			else
			{
				if(got_arg && (num_args < 2))
				{
					args[num_args++] = arg;
					arg = 0;
					got_arg = false;
				}
				
				if(c == ';')
				{
					break;
				}
				else
				{
					switch(c)
					{
						case 'A':
							screen_buffer.MoveCursorUp((num_args > 0) ? args[0] : 1);
							break;
						case 'B':
							screen_buffer.MoveCursorDown((num_args > 0) ? args[0] : 1);
							break;
						case 'C':
							screen_buffer.MoveCursorForward((num_args > 0) ? args[0] : 1);
							break;
						case 'D':
							screen_buffer.MoveCursorBack((num_args > 0) ? args[0] : 1);
							break;
						case 'E':
							screen_buffer.MoveCursorNextLine((num_args > 0) ? args[0] : 1);
							break;
						case 'F':
							screen_buffer.MoveCursorPreviousLine((num_args > 0) ? args[0] : 1);
							break;
						case 'G':
							screen_buffer.MoveCursorHorizontalAbsolute((num_args > 0) ? args[0] : 1);
							break;
						case 'H':
						case 'f':
							screen_buffer.MoveCursorTo((num_args > 0) ? args[0] : 1, (num_args > 1) ? args[1] : 1);
							break;
						case 'J':
							screen_buffer.EraseInDisplay((num_args > 0) ? args[0] : 0);
							break;
						case 'K':
							screen_buffer.EraseInLine((num_args > 0) ? args[0] : 0);
							break;
						case 'S':
							screen_buffer.ScrollUp((num_args > 0) ? args[0] : 1);
							break;
						case 'T':
							screen_buffer.ScrollDown((num_args > 0) ? args[0] : 1);
							break;
						case 'm':
							screen_buffer.SelectGraphicsRendition((num_args > 0) ? args[0] : 0);
							break;
						case 's':
							screen_buffer.SaveCursorPosition();
							break;
						case 'u':
							screen_buffer.RestoreCursorPosition();
							break;
					}
					state = 0;
				}
			}
			break;
	}
	Unlock();
}

void WebTerminal::FlushChars()
{
}

// unisim::service::interfaces::HttpServer
bool WebTerminal::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	Lock();
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
	{
		struct PropertySetter : public unisim::util::hypapp::Form_URL_Encoded_Decoder
		{
			PropertySetter() : key_event(), text() {}
			
			virtual bool FormAssign(const std::string& name, const std::string& value)
			{
				if(name == "key")
				{
					key_event.KEY(value);
				}
				else if(name == "ctrl")
				{
					key_event.CTRL(value == "1");
				}
				else if(name == "shift")
				{
					key_event.SHIFT(value == "1");
				}
				else if(name == "alt")
				{
					key_event.ALT(value == "1");
				}
				else if(name == "meta")
				{
					key_event.META(value == "1");
				}
				else if(name == "text")
				{
					text = value;
				}
				
				return true;
			}
			
			KeyEvent key_event;
			std::string text;
		};
		
		// Decode POST data
		PropertySetter property_setter;
		if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
		{
			if(!property_setter.text.empty())
			{
				input_buffer.Push(property_setter.text);
			}
			else if(!property_setter.key_event.GetKey().empty())
			{
				// Update input buffer with informations of keydown event in POST data
				Receive(property_setter.key_event);
			}
		}
		
		// Post/Redirect/Get pattern: got Post, so do Redirect
		response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
		response.SetHeaderField("Location", URI());
	}
	else
	{
		// Compute the set of graphic rendition
		std::set<std::pair<GraphicRendition, bool> > gr_set;
		for(ScreenBufferIterator it = screen_buffer.Begin(); it != screen_buffer.End(); ++it)
		{
			ScreenBufferLine& sbl = (*it);
			for(unsigned int colno = 1; colno <= display_width; colno++)
			{
				ScreenBufferCharacter& sbc = sbl[colno];
				bool under_cursor = screen_buffer.IsAtCursorLinePos(it) && (colno == screen_buffer.GetCursorColNo());
				gr_set.insert(std::pair<GraphicRendition, bool>(sbc.gr, under_cursor));
			}
		}
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html>" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(title) << "</title>" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"terminal emulator over HTTP\">" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
		response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
		response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/web_terminal/style.css\" type=\"text/css\" />" << std::endl;
		response << "\t\t<style>" << std::endl;
		
		// Generate CSS style for each graphic rendition
		std::map<std::pair<GraphicRendition, bool>, unsigned int> gr_class_map;
		unsigned int class_num = 0;
		for(std::set<std::pair<GraphicRendition, bool> >::const_iterator it = gr_set.begin(); it != gr_set.end(); it++, class_num++)
		{
			const std::pair<GraphicRendition, bool>& pair = *it;
			const GraphicRendition& gr = pair.first;
			bool under_cursor = pair.second;
			gr_class_map[pair] = class_num;
			
			response << "\t\t\t.c" << class_num << " { ";
			switch(gr.GetIntensity())
			{
				case DECREASED_INTENSITY: response << " font-weight:lighter;"; break;
				case NORMAL_INTENSITY: break;
				case INCREASED_INTENSITY: response << " font-weight:bold;"; break;
			}
			if(gr.GetItalic())
			{
				response << " font-style:italic;";
			}
			if(gr.GetUnderline() || gr.GetCrossedOut())
			{
				response << " text-decoration:";
				if(gr.GetUnderline()) response << " underline";
				if(gr.GetCrossedOut()) response << " line-through";
				response << ";";
			}
			Blink blink = gr.GetBlink();
			if(blink || under_cursor)
			{
				response << " animation:";
			}
			switch(blink)
			{
				case NO_BLINK: break;
				case SLOW_BLINK:
					response << "c" << class_num << "-blinker 1s step-start infinite";
					break;
				case RAPID_BLINK:
					response << "c" << class_num << "-blinker 0.5s step-start infinite";
					break;
			}
			if(under_cursor)
			{
				if(blink)
				{
					response << ",";
				}
				response << "c" << class_num << "-cursor-blinker 1s step-start infinite;";
			}
			else if(blink)
			{
				response << ";";
			}
			
			unsigned int fg = gr.GetReverse() ? gr.GetBackground() : gr.GetForeground();
			unsigned int bg = gr.GetReverse() ? gr.GetForeground() : gr.GetBackground();
			response << " color:" << GraphicRendition::color_table[fg] << ";";
			response << " background-color:" << GraphicRendition::color_table[bg] << ";";
			if(under_cursor)
			{
				response << " box-shadow:inset 0 -2px " << GraphicRendition::color_table[7] << ";";
			}
			response << " }" << std::endl;
			if(blink)
			{
				response << "\t\t\t@keyframes c" << class_num << "-blinker { 50% { color:" << GraphicRendition::color_table[bg] << "; } }" << std::endl;
			}
			if(under_cursor)
			{
				response << "\t\t\t@keyframes c" << class_num << "-cursor-blinker { 50% { box-shadow:none; } }" << std::endl;
			}
		}
		response << "\t\t</style>" << std::endl;
		response << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
		response << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/web_terminal/script.js\"></script>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body onload=\"gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
		refresh_period = activity ? min_display_refresh_period : std::min(refresh_period * 2.0, max_display_refresh_period);
		activity = false;
		response << "\t\t<div id=\"screen-buffer\" tabindex=\"-999\">" << std::endl;
		
		// Generate screen buffer graphical representation line by line
		GraphicRendition gr;
		bool open = false;
		// Iterate over each line of screen buffer
		for(ScreenBufferIterator it = screen_buffer.Begin(); it != screen_buffer.End(); ++it)
		{
			response << "\t\t\t";
			
			ScreenBufferLine& sbl = (*it);
			
			// Iterate over each column of screen buffer line
			for(unsigned int colno = 1; colno <= display_width; colno++)
			{
				ScreenBufferCharacter& sbc = sbl[colno];
				
				// Detect whether current line and column is under the cursor
				bool under_cursor = screen_buffer.IsAtCursorLinePos(it) && (colno == screen_buffer.GetCursorColNo());
				
				if(!open || (sbc.gr != gr) || under_cursor)
				{
					// close pre tag if it's open and when graphic rendition changes or the current line and column is under the cursor
					if(open)
					{
						response << "</pre>";
						open = false;
					}
					
					gr = sbc.gr; // remember what's the current graphic rendition

					// Find the CSS class corresponding to the current graphic rendition
					std::map<std::pair<GraphicRendition, bool>, unsigned int>::const_iterator gr_class_it = gr_class_map.find(std::pair<GraphicRendition, bool>(gr, under_cursor));
					if(gr_class_it != gr_class_map.end())
					{
						unsigned int class_num = (*gr_class_it).second;
						response << "<pre class=\"c" << class_num << "\">"; // open a pre with the right CSS class corresponding to the current graphic rendition
						open = true;
					}
				}
				
				// emit an UTF-8 character
				const UnicodeCharacter& c = sbc.c;
				if(c == (uint32_t) '<') response << "&lt;"; // print '<'
				else if((c >= 32u) && (c != 127u)) response << c; // printable ASCII/UTF-8 character
				else response << ' '; // unprintable ASCII/UTF-8 character
				if(under_cursor)
				{
					// close pre tag right after when the last emitted line and column is under the cursor
					response << "</pre>";
					open = false;
				}
			}
			if(open)
			{
				// close pre tag at the end of line if not already closed
				response << "</pre>";
				open = false;
			}
			response << "<br>" << std::endl; // insert line break
		}
		response << "\t\t</div>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}

	bool send_status = (req.GetRequestType() == unisim::util::hypapp::Request::HEAD) ? response.SendHeader(conn) : response.Send(conn);

	if(send_status)
	{
		if(verbose)
		{
			logger << DebugInfo << "sending HTTP response: done" << EndDebugInfo;
		}
	}
	else
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response" << EndDebugWarning;
	}
	
	Unlock();
	
	return send_status;
}

void WebTerminal::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
}

void WebTerminal::Lock()
{
	pthread_mutex_lock(&mutex);
}

void WebTerminal::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

} // end of namespace web_terminal
} // end of namespace service
} // end of namespace unisim
