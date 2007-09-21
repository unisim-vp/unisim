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

#ifndef __UNISIM_SERVICE_SDL_SDL_TCC__
#define __UNISIM_SERVICE_SDL_SDL_TCC__

#include <iostream>
#include <sstream>
#include <string>

namespace unisim {
namespace service {
namespace sdl {

using namespace std;

template <class ADDRESS>
SDL<ADDRESS>::SDL(const char *name, Object *parent) :
	Object(name, parent),
	Service<Video<ADDRESS> >(name, parent),
	Client<Memory<ADDRESS> >(name, parent),
	Service<Keyboard>(name, parent),
	Client<Logger>(name, parent),
	video_export("video-export", this),
	memory_import("memory-import", this),
	keyboard_export("keyboard-export", this),
	logger_import("logger-import", this),
	sdl_mutex(0),
	kbd_mutex(0),
	surface(0),
	fb_addr(0),
	width(0),
	height(0),
	depth(0),
	fb_bytes_per_line(0),
	refresh_period(0),
	alive(false),
	refresh(false),
	mode_set(false),
	screen(0),
	bmp_out_filename(),
	bmp_out_file_number(0),
	param_refresh_period("refresh-period", this, refresh_period),
	param_bmp_out_filename("bmp-out-filename", this, bmp_out_filename)
{
}

template <class ADDRESS>
SDL<ADDRESS>::~SDL()
{
	if(alive)
	{
		alive = false;
		SDL_WaitThread(refresh_thread, 0);
		SDL_WaitThread(event_handling_thread, 0);
	}
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = 0;
	}
	SDL_DestroyMutex(sdl_mutex);
	SDL_DestroyMutex(kbd_mutex);
	SDL_Quit();
}

template <class ADDRESS>
void SDL<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
bool SDL<ADDRESS>::Setup()
{
	if(!memory_import) return false;
	cerr << Object::GetName() << ": Initializing SDL..." << endl;
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cerr << Object::GetName() << ": WARNING! Can't initialize SDL: " << SDL_GetError() << endl;
		return false;
	}
	cerr << Object::GetName() << ": SDL initialized" << endl;

	sdl_mutex = SDL_CreateMutex();
	kbd_mutex = SDL_CreateMutex();
	
	bmp_out_file_number = 0;
	// create a thread for refreshing display on screen
	alive = true;
	refresh = false;
		
	if(!(refresh_thread = SDL_CreateThread((int (*)(void *)) RefreshThread, this)))
	{
		alive = false;
		return false;
	}

	if(!(event_handling_thread = SDL_CreateThread((int (*)(void *)) EventHandlingThread, this)))
	{
		alive = false;
		return false;
	}
	
	return true;
}

template <class ADDRESS>
int SDL<ADDRESS>::RefreshThread(SDL<ADDRESS> *sdl)
{
	sdl->RefreshLoop();
	return 0;
}

template <class ADDRESS>
int SDL<ADDRESS>::EventHandlingThread(SDL<ADDRESS> *sdl)
{
	sdl->EventLoop();
	return 0;
}

template <class ADDRESS>
bool SDL<ADDRESS>::GetScancode(uint8_t& scancode)
{
	bool ret = false;
	SDL_mutexP(kbd_mutex);
	if(!kbd_scancode_fifo.empty())
	{
		scancode = kbd_scancode_fifo.front();
		kbd_scancode_fifo.pop_front();
		ret = true;
	}
	SDL_mutexV(kbd_mutex);
	return ret;
}

template <class ADDRESS>
void SDL<ADDRESS>::PushScancode(uint8_t scancode)
{
	SDL_mutexP(kbd_mutex);
	kbd_scancode_fifo.push_back(scancode);
	SDL_mutexV(kbd_mutex);
}

template <class ADDRESS>
uint8_t SDL<ADDRESS>::Translate(SDLKey key)
{
//	cerr << "key = " << (unsigned int) key << endl;
	switch(key)
	{
		case SDLK_BACKSPACE: return 15;
		case SDLK_TAB: return 16;
		case SDLK_CLEAR: return 0xff; // what is it ?
		case SDLK_RETURN: return 43;
		case SDLK_PAUSE: return 126;
		case SDLK_ESCAPE: return 110;
		case SDLK_SPACE: return 61;
		case SDLK_EXCLAIM: return 2;
		case SDLK_QUOTEDBL: return 41;
		case SDLK_HASH: return 4;
		case SDLK_DOLLAR: return 5;
		case SDLK_AMPERSAND: return 8;
		case SDLK_QUOTE: return 41;
		case SDLK_LEFTPAREN: return 10;
		case SDLK_RIGHTPAREN: return 11;
		case SDLK_ASTERISK: return 9;
		case SDLK_PLUS: return 42;
		case SDLK_COMMA: return 53;
		case SDLK_MINUS: return 42;
		case SDLK_PERIOD: return 54;
		case SDLK_SLASH: return 55;
		case SDLK_0: return 12;
		case SDLK_1: return 2;
		case SDLK_2: return 3;
		case SDLK_3: return 4;
		case SDLK_4: return 5;
		case SDLK_5: return 6;
		case SDLK_6: return 7;
		case SDLK_7: return 8;
		case SDLK_8: return 9;
		case SDLK_9: return 10;
		case SDLK_COLON: return 40;
		case SDLK_SEMICOLON: return 40;
		case SDLK_LESS: return 53;
		case SDLK_EQUALS: return 42;
		case SDLK_GREATER: return 54;
		case SDLK_QUESTION: return 55;
		case SDLK_AT: return 3;
// /*		/* 
// 		Skip uppercase letters
// 		*/
// 		case SDLK_LEFTBRACKET: return 27;
// 		case SDLK_BACKSLASH: return 45;
// 		case SDLK_RIGHTBRACKET	= 93,
// 		case SDLK_CARET		= 94,
// 		case SDLK_UNDERSCORE		= 95,
// 		case SDLK_BACKQUOTE		= 96,
// 		case SDLK_a			= 97,
// 		case SDLK_b			= 98,
// 		case SDLK_c			= 99,
// 		case SDLK_d			= 100,
// 		case SDLK_e			= 101,
// 		case SDLK_f			= 102,
// 		case SDLK_g			= 103,
// 		case SDLK_h			= 104,
// 		case SDLK_i			= 105,
// 		case SDLK_j			= 106,
// 		case SDLK_k			= 107,
// 		case SDLK_l			= 108,
// 		case SDLK_m			= 109,
// 		case SDLK_n			= 110,
// 		case SDLK_o			= 111,
// 		case SDLK_p			= 112,
// 		case SDLK_q			= 113,
// 		case SDLK_r			= 114,
// 		case SDLK_s			= 115,
// 		case SDLK_t			= 116,
// 		case SDLK_u			= 117,
// 		case SDLK_v			= 118,
// 		case SDLK_w			= 119,
// 		case SDLK_x			= 120,
// 		case SDLK_y			= 121,
// 		case SDLK_z			= 122,
// 		case SDLK_DELETE		= 127,
// 		/* End of ASCII mapped keysyms */
// 	
// 		/* International keyboard syms */
// 		case SDLK_WORLD_0		= 160,		/* 0xA0 */
// 		case SDLK_WORLD_1		= 161,
// 		case SDLK_WORLD_2		= 162,
// 		case SDLK_WORLD_3		= 163,
// 		case SDLK_WORLD_4		= 164,
// 		case SDLK_WORLD_5		= 165,
// 		case SDLK_WORLD_6		= 166,
// 		case SDLK_WORLD_7		= 167,
// 		case SDLK_WORLD_8		= 168,
// 		case SDLK_WORLD_9		= 169,
// 		case SDLK_WORLD_10		= 170,
// 		case SDLK_WORLD_11		= 171,
// 		case SDLK_WORLD_12		= 172,
// 		case SDLK_WORLD_13		= 173,
// 		case SDLK_WORLD_14		= 174,
// 		case SDLK_WORLD_15		= 175,
// 		case SDLK_WORLD_16		= 176,
// 		case SDLK_WORLD_17		= 177,
// 		case SDLK_WORLD_18		= 178,
// 		case SDLK_WORLD_19		= 179,
// 		case SDLK_WORLD_20		= 180,
// 		case SDLK_WORLD_21		= 181,
// 		case SDLK_WORLD_22		= 182,
// 		case SDLK_WORLD_23		= 183,
// 		case SDLK_WORLD_24		= 184,
// 		case SDLK_WORLD_25		= 185,
// 		case SDLK_WORLD_26		= 186,
// 		case SDLK_WORLD_27		= 187,
// 		case SDLK_WORLD_28		= 188,
// 		case SDLK_WORLD_29		= 189,
// 		case SDLK_WORLD_30		= 190,
// 		case SDLK_WORLD_31		= 191,
// 		case SDLK_WORLD_32		= 192,
// 		case SDLK_WORLD_33		= 193,
// 		case SDLK_WORLD_34		= 194,
// 		case SDLK_WORLD_35		= 195,
// 		case SDLK_WORLD_36		= 196,
// 		case SDLK_WORLD_37		= 197,
// 		case SDLK_WORLD_38		= 198,
// 		case SDLK_WORLD_39		= 199,
// 		case SDLK_WORLD_40		= 200,
// 		case SDLK_WORLD_41		= 201,
// 		case SDLK_WORLD_42		= 202,
// 		case SDLK_WORLD_43		= 203,
// 		case SDLK_WORLD_44		= 204,
// 		case SDLK_WORLD_45		= 205,
// 		case SDLK_WORLD_46		= 206,
// 		case SDLK_WORLD_47		= 207,
// 		case SDLK_WORLD_48		= 208,
// 		case SDLK_WORLD_49		= 209,
// 		case SDLK_WORLD_50		= 210,
// 		case SDLK_WORLD_51		= 211,
// 		case SDLK_WORLD_52		= 212,
// 		case SDLK_WORLD_53		= 213,
// 		case SDLK_WORLD_54		= 214,
// 		case SDLK_WORLD_55		= 215,
// 		case SDLK_WORLD_56		= 216,
// 		case SDLK_WORLD_57		= 217,
// 		case SDLK_WORLD_58		= 218,
// 		case SDLK_WORLD_59		= 219,
// 		case SDLK_WORLD_60		= 220,
// 		case SDLK_WORLD_61		= 221,
// 		case SDLK_WORLD_62		= 222,
// 		case SDLK_WORLD_63		= 223,
// 		case SDLK_WORLD_64		= 224,
// 		case SDLK_WORLD_65		= 225,
// 		case SDLK_WORLD_66		= 226,
// 		case SDLK_WORLD_67		= 227,
// 		case SDLK_WORLD_68		= 228,
// 		case SDLK_WORLD_69		= 229,
// 		case SDLK_WORLD_70		= 230,
// 		case SDLK_WORLD_71		= 231,
// 		case SDLK_WORLD_72		= 232,
// 		case SDLK_WORLD_73		= 233,
// 		case SDLK_WORLD_74		= 234,
// 		case SDLK_WORLD_75		= 235,
// 		case SDLK_WORLD_76		= 236,
// 		case SDLK_WORLD_77		= 237,
// 		case SDLK_WORLD_78		= 238,
// 		case SDLK_WORLD_79		= 239,
// 		case SDLK_WORLD_80		= 240,
// 		case SDLK_WORLD_81		= 241,
// 		case SDLK_WORLD_82		= 242,
// 		case SDLK_WORLD_83		= 243,
// 		case SDLK_WORLD_84		= 244,
// 		case SDLK_WORLD_85		= 245,
// 		case SDLK_WORLD_86		= 246,
// 		case SDLK_WORLD_87		= 247,
// 		case SDLK_WORLD_88		= 248,
// 		case SDLK_WORLD_89		= 249,
// 		case SDLK_WORLD_90		= 250,
// 		case SDLK_WORLD_91		= 251,
// 		case SDLK_WORLD_92		= 252,
// 		case SDLK_WORLD_93		= 253,
// 		case SDLK_WORLD_94		= 254,
// 		case SDLK_WORLD_95		= 255,		/* 0xFF */
// 	
// 		/* Numeric keypad */
// 		case SDLK_KP0		= 256,
// 		case SDLK_KP1		= 257,
// 		case SDLK_KP2		= 258,
// 		case SDLK_KP3		= 259,
// 		case SDLK_KP4		= 260,
// 		case SDLK_KP5		= 261,
// 		case SDLK_KP6		= 262,
// 		case SDLK_KP7		= 263,
// 		case SDLK_KP8		= 264,
// 		case SDLK_KP9		= 265,
// 		case SDLK_KP_PERIOD		= 266,
// 		case SDLK_KP_DIVIDE		= 267,
// 		case SDLK_KP_MULTIPLY	= 268,
// 		case SDLK_KP_MINUS		= 269,
// 		case SDLK_KP_PLUS		= 270,
// 		case SDLK_KP_ENTER		= 271,
// 		case SDLK_KP_EQUALS		= 272,
// 	
// 		/* Arrows + Home/End pad */
// 		case SDLK_UP			= 273,
// 		case SDLK_DOWN		= 274,
// 		case SDLK_RIGHT		= 275,
// 		case SDLK_LEFT		= 276,
// 		case SDLK_INSERT		= 277,
// 		case SDLK_HOME		= 278,
// 		case SDLK_END		= 279,
// 		case SDLK_PAGEUP		= 280,
// 		case SDLK_PAGEDOWN		= 281,
// 	
// 		/* Function keys */
// 		case SDLK_F1			= 282,
// 		case SDLK_F2			= 283,
// 		case SDLK_F3			= 284,
// 		case SDLK_F4			= 285,
// 		case SDLK_F5			= 286,
// 		case SDLK_F6			= 287,
// 		case SDLK_F7			= 288,
// 		case SDLK_F8			= 289,
// 		case SDLK_F9			= 290,
// 		case SDLK_F10		= 291,
// 		case SDLK_F11		= 292,
// 		case SDLK_F12		= 293,
// 		case SDLK_F13		= 294,
// 		case SDLK_F14		= 295,
// 		case SDLK_F15		= 296,
// 	
// 		/* Key state modifier keys */
// 		case SDLK_NUMLOCK		= 300,
// 		case SDLK_CAPSLOCK		= 301,
// 		case SDLK_SCROLLOCK		= 302,
// 		case SDLK_RSHIFT		= 303,
// 		case SDLK_LSHIFT		= 304,
// 		case SDLK_RCTRL		= 305,
// 		case SDLK_LCTRL		= 306,
// 		case SDLK_RALT		= 307,
// 		case SDLK_LALT		= 308,
// 		case SDLK_RMETA		= 309,
// 		case SDLK_LMETA		= 310,
// 		case SDLK_LSUPER		= 311,		/* Left "Windows" key */
// 		case SDLK_RSUPER		= 312,		/* Right "Windows" key */
// 		case SDLK_MODE		= 313,		/* "Alt Gr" key */
// 		case SDLK_COMPOSE		= 314,		/* Multi-key compose key */
// 	
// 		/* Miscellaneous function keys */
// 		case SDLK_HELP		= 315,
// 		case SDLK_PRINT		= 316,
// 		case SDLK_SYSREQ		= 317,
// 		case SDLK_BREAK		= 318,
// 		case SDLK_MENU		= 319,
// 		case SDLK_POWER		= 320,		/* Power Macintosh power key */
// 		case SDLK_EURO		= 321,		/* Some european keyboards */
// 		case SDLK_UNDO		= 322,		/* Atari keyboard has Undo */*/
		
	}
	return 0;
}

template <class ADDRESS>
void SDL<ADDRESS>::ProcessKeyboardEvent(SDL_KeyboardEvent& key)
{
/*	cerr << Object::GetName() << ": ";
	switch(key.type)
	{
		case SDL_KEYDOWN:
			cerr << "Pressing ";
			break;
		case SDL_KEYUP:
			cerr << "Releasing ";
			break;
	}
	cerr << SDL_GetKeyName(key.keysym.sym) << "(scancode=" << (unsigned int) key.keysym.scancode << ")" <<  endl;*/
	
	uint8_t scancode = Translate(key.keysym.sym);
	
	switch(key.type)
	{
		case SDL_KEYDOWN:
//			cerr << "Pressing ";
			scancode += 0x80;
			break;
		case SDL_KEYUP:
//			cerr << "Releasing ";
			break;
	}
	PushScancode(scancode);
}

template <class ADDRESS>
void SDL<ADDRESS>::EventLoop()
{
	SDL_Event sdl_ev;
	
	while(alive)
	{
		SDL_Delay(1);
		SDL_mutexP(sdl_mutex);
		if(SDL_PollEvent(&sdl_ev))
		{
			switch(sdl_ev.type)
			{
				case SDL_VIDEOEXPOSE:
					refresh = true;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					ProcessKeyboardEvent(sdl_ev.key);
					break;
				case SDL_QUIT:
					break;
			}
		}
		SDL_mutexV(sdl_mutex);
	}
}

template <class ADDRESS>
void SDL<ADDRESS>::RefreshLoop()
{
	SDL_Event sdl_ev;
	
	while(alive)
	{
		if(refresh)
		{
			Blit();
			refresh = false;
		}
		SDL_Delay(refresh_period);
	}
}

template <class ADDRESS>
bool SDL<ADDRESS>::SetVideoMode(ADDRESS fb_addr, uint32_t width, uint32_t height, uint32_t depth, uint32_t fb_bytes_per_line)
{
	SDL_mutexP(sdl_mutex);
	
	this->fb_addr = fb_addr;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->fb_bytes_per_line = fb_bytes_per_line;
	
	unsigned int red_offset;
	unsigned int green_offset;
	unsigned int blue_offset;
	unsigned int red_bits;
	unsigned int green_bits;
	unsigned int blue_bits;
	
	switch(depth)
	{
		case 15:
			red_offset = 10;
			green_offset = 5;
			blue_offset = 0;
			red_bits = 5;
			green_bits = 5;
			blue_bits = 5;
			break;
		case 16:
			red_offset = 11;
			green_offset = 5;
			blue_offset = 0;
			red_bits = 5;
			green_bits = 6;
			blue_bits = 5;
			break;
		case 24:
		case 32:
			red_offset = 16;
			green_offset = 8;
			blue_offset = 0;
			red_bits = 8;
			green_bits = 8;
			blue_bits = 8;
			break;
		default:
			cerr << Object::GetName() << ": Unsupported pixel depth " << depth << "(you should try 15, 16 or 24)" << endl;
			return false;
	}
	
	unsigned int red_mask = ((1 << red_bits) - 1) << red_offset;
	unsigned int green_mask = ((1 << green_bits) - 1) << green_offset;
	unsigned int blue_mask = ((1 << blue_bits) - 1) << blue_offset;
	
	// Initialize video display
	cerr << Object::GetName() << ": Initializing video mode " << width << " pixels x " << height << " pixels x " << depth << " bits per pixel..." << endl;

	unsigned int sdl_depth = (depth + 7) & ~7;
	screen = SDL_SetVideoMode(width, height, sdl_depth, SDL_HWSURFACE);
	if(!screen)
	{
		cerr << Object::GetName() << ": WARNING! Can't set video mode using a hardware surface: " << SDL_GetError() << endl;
		cerr << Object::GetName() << ": Trying with a software surface" << endl;
		screen = SDL_SetVideoMode(width, height, sdl_depth, SDL_SWSURFACE);
		if(!screen)
		{
			cerr << Object::GetName() << ": ERROR! Still can't set video mode using a software surface: " << SDL_GetError() << endl;
			SDL_mutexV(sdl_mutex);
			return false;
		}
	}
	cerr << Object::GetName() << ": Video mode set" << endl;

	stringstream sstr;
	sstr << Object::GetName() << " [" << width << "x" << height << "x" << depth << "]";
	string title = sstr.str();
	SDL_WM_SetCaption(title.c_str(), title.c_str());
	
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = 0;
	}
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, sdl_depth, red_mask, green_mask, blue_mask, 0);
	if(!surface)
	{
		cerr << Object::GetName() << ": Can't create SW surface" << endl;
		SDL_mutexV(sdl_mutex);
		return false;
	}

	//SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
	
	mode_set = true;
	SDL_mutexV(sdl_mutex);
	return true;
}

template <class ADDRESS>
void SDL<ADDRESS>::Blit()
{
	if(!mode_set) return;

	SDL_mutexP(sdl_mutex);
	
	uint32_t line;
	uint8_t *dst;
	uint8_t *src;
	ADDRESS scan_line_addr;
	
	if(SDL_LockSurface(surface) < 0) return;
	
	switch(depth)
	{
		case 15:
		case 16:
		{
			uint16_t scan_line[width];
			
			for(line = 0, dst = (uint8_t *) surface->pixels, scan_line_addr = fb_addr; line < height; line++, dst += surface->pitch, scan_line_addr += fb_bytes_per_line)
			{
				if(memory_import->ReadMemory(scan_line_addr, scan_line, 2 * width))
				{
					BigEndian2Host((uint16_t *) dst, (uint16_t *) scan_line, width);
				}
			}
			break;
		}
			
		case 24:
		case 32:
		{
			uint32_t scan_line[width];
			
			for(line = 0, dst = (uint8_t *) surface->pixels, scan_line_addr = fb_addr; line < height; line++, dst += surface->pitch, scan_line_addr += fb_bytes_per_line)
			{
				if(memory_import->ReadMemory(scan_line_addr, scan_line, 4 * width))
				{
					BigEndian2Host((uint32_t *) dst, (uint32_t *) scan_line, width);
				}
			}
			break;
		}
	}
	SDL_UnlockSurface(surface);
			
	if(SDL_BlitSurface(surface, 0, screen, 0) != 0)
	{
		cerr << Object::GetName() << ": Can't blit surface" << endl;
	}
	SDL_UpdateRect(screen, 0, 0, width, height);
	
	if(!bmp_out_filename.empty())
	{
		char filename[bmp_out_filename.length() + 12];
		sprintf(filename, "%s - %04u.bmp", bmp_out_filename.c_str(), bmp_out_file_number);
		if(SDL_SaveBMP(surface, filename) != 0)
		{
			cerr << Object::GetName() << ": Can't save bitmap into file " << filename << endl;
		}
		else
		{
			bmp_out_file_number++;
		}
	}

	SDL_mutexV(sdl_mutex);
}

template <class ADDRESS>
void SDL<ADDRESS>::RefreshDisplay()
{
	refresh = true;
}

} // end of namespace sdl
} // end of namespace service
} // end of namespace unisim

#endif
