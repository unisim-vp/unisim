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
#include <fstream>
#include <sstream>
#include <string>
#include <unisim/util/xml/xml.hh>

namespace unisim {
namespace service {
namespace sdl {

using namespace std;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS>
VideoMode<ADDRESS>::VideoMode()
	: fb_addr(0)
	, width(0)
	, height(0)
	, depth(0)
	, fb_bytes_per_line(0)
{
}

template <class ADDRESS>
VideoMode<ADDRESS>::VideoMode(ADDRESS _fb_addr, uint32_t _width, uint32_t _height, uint32_t _depth, uint32_t _fb_bytes_per_line)
	: fb_addr(_fb_addr)
	, width(_width)
	, height(_height)
	, depth(_depth)
	, fb_bytes_per_line(_fb_bytes_per_line)
{
}

template <class ADDRESS>
SDL<ADDRESS>::SDL(const char *name, Object *parent) :
	Object(name, parent),
	Service<Video<ADDRESS> >(name, parent),
	Client<Memory<ADDRESS> >(name, parent),
	Service<Keyboard>(name, parent),
	logger(*this),
	video_export("video-export", this),
	memory_import("memory-import", this),
	keyboard_export("keyboard-export", this),
#if defined(HAVE_SDL)
	sdl_mutex(0),
	kbd_mutex(0),
	surface(0),
	screen(0),
#endif
	refresh_period(0),
	alive(false),
	refresh(false),
	mode_set(false),
	bmp_out_filename(),
	keymap_filename(),
	learn_keymap_filename("learned_keymap.xml"),
	bmp_out_file_number(0),
	verbose_setup(false),
	verbose_run(false),
	param_refresh_period("refresh-period", this, refresh_period),
	param_bmp_out_filename("bmp-out-filename", this, bmp_out_filename),
	param_keymap_filename("keymap-filename", this, keymap_filename),
	param_verbose_setup("verbose-setup", this, verbose_setup),
	param_verbose_run("verbose-run", this, verbose_run)
{
#if defined(HAVE_SDL)
	sdlk_string_map["backspace"] = SDLK_BACKSPACE;
	sdlk_string_map["tab"] = SDLK_TAB;
	sdlk_string_map["clear"] = SDLK_CLEAR;
	sdlk_string_map["return"] = SDLK_RETURN;
	sdlk_string_map["pause"] = SDLK_PAUSE;
	sdlk_string_map["escape"] = SDLK_ESCAPE;
	sdlk_string_map["space"] = SDLK_SPACE;
	sdlk_string_map["exclaim"] = SDLK_EXCLAIM;
	sdlk_string_map["quotedbl"] = SDLK_QUOTEDBL;
	sdlk_string_map["hash"] = SDLK_HASH;
	sdlk_string_map["dollar"] = SDLK_DOLLAR;
	sdlk_string_map["ampersand"] = SDLK_AMPERSAND;
	sdlk_string_map["quote"] = SDLK_QUOTE;
	sdlk_string_map["leftparen"] = SDLK_LEFTPAREN;
	sdlk_string_map["rightparen"] = SDLK_RIGHTPAREN;
	sdlk_string_map["asterisk"] = SDLK_ASTERISK;
	sdlk_string_map["plus"] = SDLK_PLUS;
	sdlk_string_map["comma"] = SDLK_COMMA;
	sdlk_string_map["minus"] = SDLK_MINUS;
	sdlk_string_map["period"] = SDLK_PERIOD;
	sdlk_string_map["slash"] = SDLK_SLASH;
	sdlk_string_map["0"] = SDLK_0;
	sdlk_string_map["1"] = SDLK_1;
	sdlk_string_map["2"] = SDLK_2;
	sdlk_string_map["3"] = SDLK_3;
	sdlk_string_map["4"] = SDLK_4;
	sdlk_string_map["5"] = SDLK_5;
	sdlk_string_map["6"] = SDLK_6;
	sdlk_string_map["7"] = SDLK_7;
	sdlk_string_map["8"] = SDLK_8;
	sdlk_string_map["9"] = SDLK_9;
	sdlk_string_map["colon"] = SDLK_COLON;
	sdlk_string_map["semicolon"] = SDLK_SEMICOLON;
	sdlk_string_map["less"] = SDLK_LESS;
	sdlk_string_map["equals"] = SDLK_EQUALS;
	sdlk_string_map["greater"] = SDLK_GREATER;
	sdlk_string_map["question"] = SDLK_QUESTION;
	sdlk_string_map["at"] = SDLK_AT;
	sdlk_string_map["leftbracket"] = SDLK_LEFTBRACKET;
	sdlk_string_map["backslash"] = SDLK_BACKSLASH;
	sdlk_string_map["rightbracket"] = SDLK_RIGHTBRACKET;
	sdlk_string_map["caret"] = SDLK_CARET;
	sdlk_string_map["underscore"] = SDLK_UNDERSCORE;
	sdlk_string_map["backquote"] = SDLK_BACKQUOTE;
	sdlk_string_map["a"] = SDLK_a;
	sdlk_string_map["b"] = SDLK_b;
	sdlk_string_map["c"] = SDLK_c;
	sdlk_string_map["d"] = SDLK_d;
	sdlk_string_map["e"] = SDLK_e;
	sdlk_string_map["f"] = SDLK_f;
	sdlk_string_map["g"] = SDLK_g;
	sdlk_string_map["h"] = SDLK_h;
	sdlk_string_map["i"] = SDLK_i;
	sdlk_string_map["j"] = SDLK_j;
	sdlk_string_map["k"] = SDLK_k;
	sdlk_string_map["l"] = SDLK_l;
	sdlk_string_map["m"] = SDLK_m;
	sdlk_string_map["n"] = SDLK_n;
	sdlk_string_map["o"] = SDLK_o;
	sdlk_string_map["p"] = SDLK_p;
	sdlk_string_map["q"] = SDLK_q;
	sdlk_string_map["r"] = SDLK_r;
	sdlk_string_map["s"] = SDLK_s;
	sdlk_string_map["t"] = SDLK_t;
	sdlk_string_map["u"] = SDLK_u;
	sdlk_string_map["v"] = SDLK_v;
	sdlk_string_map["w"] = SDLK_w;
	sdlk_string_map["x"] = SDLK_x;
	sdlk_string_map["y"] = SDLK_y;
	sdlk_string_map["z"] = SDLK_z;
	sdlk_string_map["delete"] = SDLK_DELETE;
	sdlk_string_map["world_0"] = SDLK_WORLD_0;
	sdlk_string_map["world_1"] = SDLK_WORLD_1;
	sdlk_string_map["world_2"] = SDLK_WORLD_2;
	sdlk_string_map["world_3"] = SDLK_WORLD_3;
	sdlk_string_map["world_4"] = SDLK_WORLD_4;
	sdlk_string_map["world_5"] = SDLK_WORLD_5;
	sdlk_string_map["world_6"] = SDLK_WORLD_6;
	sdlk_string_map["world_7"] = SDLK_WORLD_7;
	sdlk_string_map["world_8"] = SDLK_WORLD_8;
	sdlk_string_map["world_9"] = SDLK_WORLD_9;
	sdlk_string_map["world_10"] = SDLK_WORLD_10;
	sdlk_string_map["world_11"] = SDLK_WORLD_11;
	sdlk_string_map["world_12"] = SDLK_WORLD_12;
	sdlk_string_map["world_13"] = SDLK_WORLD_13;
	sdlk_string_map["world_14"] = SDLK_WORLD_14;
	sdlk_string_map["world_15"] = SDLK_WORLD_15;
	sdlk_string_map["world_16"] = SDLK_WORLD_16;
	sdlk_string_map["world_17"] = SDLK_WORLD_17;
	sdlk_string_map["world_18"] = SDLK_WORLD_18;
	sdlk_string_map["world_19"] = SDLK_WORLD_19;
	sdlk_string_map["world_20"] = SDLK_WORLD_20;
	sdlk_string_map["world_21"] = SDLK_WORLD_21;
	sdlk_string_map["world_22"] = SDLK_WORLD_22;
	sdlk_string_map["world_23"] = SDLK_WORLD_23;
	sdlk_string_map["world_24"] = SDLK_WORLD_24;
	sdlk_string_map["world_25"] = SDLK_WORLD_25;
	sdlk_string_map["world_26"] = SDLK_WORLD_26;
	sdlk_string_map["world_27"] = SDLK_WORLD_27;
	sdlk_string_map["world_28"] = SDLK_WORLD_28;
	sdlk_string_map["world_29"] = SDLK_WORLD_29;
	sdlk_string_map["world_30"] = SDLK_WORLD_30;
	sdlk_string_map["world_31"] = SDLK_WORLD_31;
	sdlk_string_map["world_32"] = SDLK_WORLD_32;
	sdlk_string_map["world_33"] = SDLK_WORLD_33;
	sdlk_string_map["world_34"] = SDLK_WORLD_34;
	sdlk_string_map["world_35"] = SDLK_WORLD_35;
	sdlk_string_map["world_36"] = SDLK_WORLD_36;
	sdlk_string_map["world_37"] = SDLK_WORLD_37;
	sdlk_string_map["world_38"] = SDLK_WORLD_38;
	sdlk_string_map["world_39"] = SDLK_WORLD_39;
	sdlk_string_map["world_40"] = SDLK_WORLD_40;
	sdlk_string_map["world_41"] = SDLK_WORLD_41;
	sdlk_string_map["world_42"] = SDLK_WORLD_42;
	sdlk_string_map["world_43"] = SDLK_WORLD_43;
	sdlk_string_map["world_44"] = SDLK_WORLD_44;
	sdlk_string_map["world_45"] = SDLK_WORLD_45;
	sdlk_string_map["world_46"] = SDLK_WORLD_46;
	sdlk_string_map["world_47"] = SDLK_WORLD_47;
	sdlk_string_map["world_48"] = SDLK_WORLD_48;
	sdlk_string_map["world_49"] = SDLK_WORLD_49;
	sdlk_string_map["world_50"] = SDLK_WORLD_50;
	sdlk_string_map["world_51"] = SDLK_WORLD_51;
	sdlk_string_map["world_52"] = SDLK_WORLD_52;
	sdlk_string_map["world_53"] = SDLK_WORLD_53;
	sdlk_string_map["world_54"] = SDLK_WORLD_54;
	sdlk_string_map["world_55"] = SDLK_WORLD_55;
	sdlk_string_map["world_56"] = SDLK_WORLD_56;
	sdlk_string_map["world_57"] = SDLK_WORLD_57;
	sdlk_string_map["world_58"] = SDLK_WORLD_58;
	sdlk_string_map["world_59"] = SDLK_WORLD_59;
	sdlk_string_map["world_60"] = SDLK_WORLD_60;
	sdlk_string_map["world_61"] = SDLK_WORLD_61;
	sdlk_string_map["world_62"] = SDLK_WORLD_62;
	sdlk_string_map["world_63"] = SDLK_WORLD_63;
	sdlk_string_map["world_64"] = SDLK_WORLD_64;
	sdlk_string_map["world_65"] = SDLK_WORLD_65;
	sdlk_string_map["world_66"] = SDLK_WORLD_66;
	sdlk_string_map["world_67"] = SDLK_WORLD_67;
	sdlk_string_map["world_68"] = SDLK_WORLD_68;
	sdlk_string_map["world_69"] = SDLK_WORLD_69;
	sdlk_string_map["world_70"] = SDLK_WORLD_70;
	sdlk_string_map["world_71"] = SDLK_WORLD_71;
	sdlk_string_map["world_72"] = SDLK_WORLD_72;
	sdlk_string_map["world_73"] = SDLK_WORLD_73;
	sdlk_string_map["world_74"] = SDLK_WORLD_74;
	sdlk_string_map["world_75"] = SDLK_WORLD_75;
	sdlk_string_map["world_76"] = SDLK_WORLD_76;
	sdlk_string_map["world_77"] = SDLK_WORLD_77;
	sdlk_string_map["world_78"] = SDLK_WORLD_78;
	sdlk_string_map["world_79"] = SDLK_WORLD_79;
	sdlk_string_map["world_80"] = SDLK_WORLD_80;
	sdlk_string_map["world_81"] = SDLK_WORLD_81;
	sdlk_string_map["world_82"] = SDLK_WORLD_82;
	sdlk_string_map["world_83"] = SDLK_WORLD_83;
	sdlk_string_map["world_84"] = SDLK_WORLD_84;
	sdlk_string_map["world_85"] = SDLK_WORLD_85;
	sdlk_string_map["world_86"] = SDLK_WORLD_86;
	sdlk_string_map["world_87"] = SDLK_WORLD_87;
	sdlk_string_map["world_88"] = SDLK_WORLD_88;
	sdlk_string_map["world_89"] = SDLK_WORLD_89;
	sdlk_string_map["world_90"] = SDLK_WORLD_90;
	sdlk_string_map["world_91"] = SDLK_WORLD_91;
	sdlk_string_map["world_92"] = SDLK_WORLD_92;
	sdlk_string_map["world_93"] = SDLK_WORLD_93;
	sdlk_string_map["world_94"] = SDLK_WORLD_94;
	sdlk_string_map["world_95"] = SDLK_WORLD_95;
	sdlk_string_map["kp0"] = SDLK_KP0;
	sdlk_string_map["kp1"] = SDLK_KP1;
	sdlk_string_map["kp2"] = SDLK_KP2;
	sdlk_string_map["kp3"] = SDLK_KP3;
	sdlk_string_map["kp4"] = SDLK_KP4;
	sdlk_string_map["kp5"] = SDLK_KP5;
	sdlk_string_map["kp6"] = SDLK_KP6;
	sdlk_string_map["kp7"] = SDLK_KP7;
	sdlk_string_map["kp8"] = SDLK_KP8;
	sdlk_string_map["kp9"] = SDLK_KP9;
	sdlk_string_map["kp_period"] = SDLK_KP_PERIOD;
	sdlk_string_map["kp_divid"] = SDLK_KP_DIVIDE;
	sdlk_string_map["kp_multiply"] = SDLK_KP_MULTIPLY;
	sdlk_string_map["kp_minus"] = SDLK_KP_MINUS;
	sdlk_string_map["kp_plus"] = SDLK_KP_PLUS;
	sdlk_string_map["kp_enter"] = SDLK_KP_ENTER;
	sdlk_string_map["kp_equals"] = SDLK_KP_EQUALS;
	sdlk_string_map["kp_up"] = SDLK_UP;
	sdlk_string_map["kp_down"] = SDLK_DOWN;
	sdlk_string_map["kp_right"] = SDLK_RIGHT;
	sdlk_string_map["kp_left"] = SDLK_LEFT;
	sdlk_string_map["kp_insert"] = SDLK_INSERT;
	sdlk_string_map["kp_home"] = SDLK_HOME;
	sdlk_string_map["kp_end"] = SDLK_END;
	sdlk_string_map["kp_pageup"] = SDLK_PAGEUP;
	sdlk_string_map["kp_pagedown"] = SDLK_PAGEDOWN;
	sdlk_string_map["f1"] = SDLK_F1;
	sdlk_string_map["f2"] = SDLK_F2;
	sdlk_string_map["f3"] = SDLK_F3;
	sdlk_string_map["f4"] = SDLK_F4;
	sdlk_string_map["f5"] = SDLK_F5;
	sdlk_string_map["f6"] = SDLK_F6;
	sdlk_string_map["f7"] = SDLK_F7;
	sdlk_string_map["f8"] = SDLK_F8;
	sdlk_string_map["f9"] = SDLK_F9;
	sdlk_string_map["f10"] = SDLK_F10;
	sdlk_string_map["f11"] = SDLK_F11;
	sdlk_string_map["f12"] = SDLK_F12;
	sdlk_string_map["f13"] = SDLK_F13;
	sdlk_string_map["f14"] = SDLK_F14;
	sdlk_string_map["f15"] = SDLK_F15;
	sdlk_string_map["numlock"] = SDLK_NUMLOCK;
	sdlk_string_map["capslock"] = SDLK_CAPSLOCK;
	sdlk_string_map["scrollock"] = SDLK_SCROLLOCK;
	sdlk_string_map["rshift"] = SDLK_RSHIFT;
	sdlk_string_map["lshift"] = SDLK_LSHIFT;
	sdlk_string_map["rctrl"] = SDLK_RCTRL;
	sdlk_string_map["lctrl"] = SDLK_LCTRL;
	sdlk_string_map["ralt"] = SDLK_RALT;
	sdlk_string_map["lalt"] = SDLK_LALT;
	sdlk_string_map["rmeta"] = SDLK_RMETA;
	sdlk_string_map["lmeta"] = SDLK_LMETA;
	sdlk_string_map["lupper"] = SDLK_LSUPER;
	sdlk_string_map["rupper"] = SDLK_RSUPER;
	sdlk_string_map["mode"] = SDLK_MODE;
	sdlk_string_map["compose"] = SDLK_COMPOSE;
	sdlk_string_map["help"] = SDLK_HELP;
	sdlk_string_map["print"] = SDLK_PRINT;
	sdlk_string_map["sysreq"] = SDLK_SYSREQ;
	sdlk_string_map["break"] = SDLK_BREAK;
	sdlk_string_map["menu"] = SDLK_MENU;
	sdlk_string_map["power"] = SDLK_POWER;
	sdlk_string_map["euro"] = SDLK_EURO;
	sdlk_string_map["undo"] = SDLK_UNDO;
#endif
}

template <class ADDRESS>
SDL<ADDRESS>::~SDL()
{
	if(alive)
	{
		alive = false;
#if defined(HAVE_SDL)
		SDL_WaitThread(refresh_thread, 0);
		SDL_WaitThread(event_handling_thread, 0);
#endif
	}
#if defined(HAVE_SDL)
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = 0;
	}
	SDL_DestroyMutex(sdl_mutex);
	SDL_DestroyMutex(kbd_mutex);
	SDL_Quit();
#endif
}

template <class ADDRESS>
void SDL<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
bool SDL<ADDRESS>::Setup()
{
	if(!memory_import) return false;
#if defined(HAVE_SDL)
	if(unlikely(verbose_setup))
	{
		logger << DebugInfo << "Initializing SDL..." << EndDebugInfo;
	}
	if(SDL_Init(0) < 0)
	{
		logger << DebugError << "Can't initialize SDL: " << SDL_GetError() << EndDebugError;
		return false;
	}
	if(unlikely(verbose_setup))
	{
		logger << DebugInfo << "SDL initialized" << EndDebugInfo;
	}

	// Keyboard keymap
	unisim::util::xml::Node *xml_node = 0;
	if(!keymap_filename.empty())
	{
		unisim::util::xml::Parser *parser = new unisim::util::xml::Parser();
		xml_node = parser->Parse(keymap_filename);
		delete parser;
	}

	if(xml_node)
	{
		list<unisim::util::xml::Node *>::const_iterator xml_child;
		const list<unisim::util::xml::Node *> *xml_childs = xml_node->Childs();

		for(xml_child = xml_childs->begin(); xml_child != xml_childs->end(); xml_child++)
		{
			if((*xml_child)->Name() != string("key"))
			{
				unisim::util::xml::Error((*xml_child)->Filename(), (*xml_child)->LineNo(), "WARNING! expected tag key, ignoring tag %s", (*xml_child)->Name().c_str());
			}
			else
			{
				const list<unisim::util::xml::Property *> *xml_props = (*xml_child)->Properties();
				list<unisim::util::xml::Property *>::const_iterator xml_prop;

				SDLKey sdlk = SDLK_UNKNOWN;
				unsigned int loc = 0;
				string sdlk_string;
				string loc_string;

				for(xml_prop = xml_props->begin(); xml_prop != xml_props->end(); xml_prop++)
				{
					if((*xml_prop)->Name() == string("sdlk"))
					{
						sdlk_string = (*xml_prop)->Value();

						map<string, SDLKey>::iterator sdlk_iter = sdlk_string_map.find(sdlk_string);
						if(sdlk_iter != sdlk_string_map.end())
						{
							sdlk = (*sdlk_iter).second;
						}
					}
					else
					{
						if((*xml_prop)->Name() == string("loc"))
						{
							loc_string = (*xml_prop)->Value();
							std::istringstream sstr(loc_string, std::istringstream::in);
							sstr >> loc;
							if(sstr.fail())
							{
								loc = 0;
							}
						}
						else
						{
							unisim::util::xml::Error((*xml_prop)->Filename(), (*xml_prop)->LineNo(), "WARNING! ignoring property %s of tag %s", (*xml_prop)->Name().c_str(), xml_node->Name().c_str());
						}
					}
				}

				if(sdlk != SDLK_UNKNOWN && loc != 0)
				{
					if(unlikely(verbose_setup))
					{
						logger << DebugInfo << "SDL Key \"" << sdlk_string << "\" -> Key #" << loc << endl;
					}
					keymap[sdlk] = loc;
				}
				else
				{
					unisim::util::xml::Error((*xml_child)->Filename(), (*xml_child)->LineNo(), "WARNING! ignoring unrecognized key", (*xml_child)->Name().c_str());
				}
			}
		}
	}
	else
	{
		logger << DebugError << "No keymap available" << EndDebugError;
		if(!SDL_SetVideoMode(320, 240, 0, SDL_ANYFORMAT))
		{
			logger << DebugError << "Can't set video mode: " << SDL_GetError() << EndDebugError;
			logger << DebugInfo << "Can't learn keyboard layout" << EndDebugInfo;
			return false;
		}
		LearnKeyboard();
		return false;
	}

	delete xml_node;

	sdl_mutex = SDL_CreateMutex();
	kbd_mutex = SDL_CreateMutex();
#else
	logger << DebugWarning << "No host video output nor input devices available" << EndDebugWarning;
#endif
	
	bmp_out_file_number = 0;
	alive = true;
	refresh = false;

#if defined(HAVE_SDL)
	// create a thread for refreshing display on screen
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
#endif

	return true;
}

#if defined(HAVE_SDL)
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
#endif

template <class ADDRESS>
bool SDL<ADDRESS>::GetKeyAction(unisim::service::interfaces::Keyboard::KeyAction& key_action)
{
	bool ret = false;
#if defined(HAVE_SDL)
	SDL_mutexP(kbd_mutex);
#endif
	if(!kbd_key_action_fifo.empty())
	{
		key_action = kbd_key_action_fifo.front();
		kbd_key_action_fifo.pop_front();
		ret = true;
	}
#if defined(HAVE_SDL)
	SDL_mutexV(kbd_mutex);
#endif
	return ret;
}

template <class ADDRESS>
void SDL<ADDRESS>::PushKeyAction(const unisim::service::interfaces::Keyboard::KeyAction& key_action)
{
#if defined(HAVE_SDL)
	SDL_mutexP(kbd_mutex);
#endif
	kbd_key_action_fifo.push_back(key_action);
#if defined(HAVE_SDL)
	SDL_mutexV(kbd_mutex);
#endif
}

#if defined(HAVE_SDL)

template <class ADDRESS>
void SDL<ADDRESS>::ProcessKeyboardEvent(SDL_KeyboardEvent& kbd_ev)
{
	if(kbd_ev.keysym.sym == SDLK_RCTRL)
	{
		if(unlikely(verbose_run))
		{
			logger << DebugInfo << "Host key " << ((kbd_ev.type == SDL_KEYUP) ? "up" : "down") << EndDebugInfo;
		}
		KeyAction key_action;

		key_action.action = (kbd_ev.type == SDL_KEYUP) ? unisim::service::interfaces::Keyboard::KeyAction::KEY_UP : unisim::service::interfaces::Keyboard::KeyAction::KEY_DOWN;

		key_action.key_num = 58; // Left Control
		PushKeyAction(key_action);
		key_action.key_num = 60; // Left Alt
		PushKeyAction(key_action);
	}
	else
	{
		map<SDLKey, uint8_t>::iterator keymap_iter = keymap.find(kbd_ev.keysym.sym);
		uint8_t key_num = (keymap_iter != keymap.end()) ? (*keymap_iter).second : 0;

		if(unlikely(verbose_run))
		{
			logger << DebugInfo << "Key #" << (unsigned int) key_num << " (" << (unsigned int) kbd_ev.keysym.sym << ")" << ((kbd_ev.type == SDL_KEYUP) ? "up" : "down") << EndDebugInfo;
		}

		if(key_num)
		{
			KeyAction key_action;
			key_action.key_num = key_num;
			key_action.action = (kbd_ev.type == SDL_KEYUP) ? unisim::service::interfaces::Keyboard::KeyAction::KEY_UP : unisim::service::interfaces::Keyboard::KeyAction::KEY_DOWN;
			PushKeyAction(key_action);
		}
	}
}

template <class ADDRESS>
void SDL<ADDRESS>::EventLoop()
{
	SDL_mutexP(sdl_mutex);
	if(unlikely(verbose_setup))
	{
		logger << DebugInfo << "Initializing SDL Video subsystem..." << EndDebugInfo;
	}
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		logger << DebugError << "Can't initialize SDL Video subsystem: " << SDL_GetError() << EndDebugError;
		return;
	}
	if(unlikely(verbose_setup))
	{
		logger << DebugInfo << "SDL Video subsystem initialized" << EndDebugInfo;
	}
	SDL_mutexV(sdl_mutex);

	SDL_Event sdl_ev;
	
	while(alive)
	{
		SDL_Delay(10);
		SDL_mutexP(sdl_mutex);
		if(SDL_PollEvent(&sdl_ev))
		{
			switch(sdl_ev.type)
			{
				case SDL_USEREVENT:
					switch(sdl_ev.user.code)
					{
						case EV_SET_VIDEO_MODE:
							HandleSetVideoMode(*(const VideoMode<ADDRESS> *) sdl_ev.user.data1);
							break;
						case EV_BLIT:
							HandleBlit();
							break;
					}
					break;
				case SDL_VIDEOEXPOSE:
					refresh = true;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					ProcessKeyboardEvent(sdl_ev.key);
					break;
				case SDL_QUIT:
					exit(0); // TO IMPROVE!
			}
		}
		SDL_mutexV(sdl_mutex);
	}
}

template <class ADDRESS>
void SDL<ADDRESS>::RefreshLoop()
{
	while(alive)
	{
		if(refresh)
		{
			refresh = false;
			Blit();
		}
		SDL_Delay(refresh_period);
	}
}

template <class ADDRESS>
bool SDL<ADDRESS>::HandleSetVideoMode(const VideoMode<ADDRESS>& _video_mode)
{
	video_mode = _video_mode;

	unsigned int red_offset;
	unsigned int green_offset;
	unsigned int blue_offset;
	unsigned int red_bits;
	unsigned int green_bits;
	unsigned int blue_bits;
	
	switch(video_mode.depth)
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
			logger << DebugError << "Unsupported pixel depth " << video_mode.depth << "(you should try 15, 16 or 24)" << EndDebugError;
			return false;
	}
	
	unsigned int red_mask = ((1 << red_bits) - 1) << red_offset;
	unsigned int green_mask = ((1 << green_bits) - 1) << green_offset;
	unsigned int blue_mask = ((1 << blue_bits) - 1) << blue_offset;
	
	// Initialize video display
	if(unlikely(verbose_run))
	{
		logger << DebugInfo << "Initializing video mode " << video_mode.width << " pixels x " << video_mode.height << " pixels x " << video_mode.depth << " bits per pixel..." << EndDebugInfo;
	}

	unsigned int sdl_depth = (video_mode.depth + 7) & ~7;
	screen = SDL_SetVideoMode(video_mode.width, video_mode.height, sdl_depth, SDL_SWSURFACE);
	if(!screen)
	{
		if(unlikely(verbose_run))
		{
			logger << DebugWarning << "Can't set video mode using a hardware surface: " << SDL_GetError() << EndDebugWarning;
			logger << DebugInfo << "Trying with a software surface" << EndDebugWarning;
		}
		screen = SDL_SetVideoMode(video_mode.width, video_mode.height, sdl_depth, SDL_SWSURFACE);
		if(!screen)
		{
			logger << DebugError << "Still can't set video mode using a software surface: " << SDL_GetError() << EndDebugError;
			return false;
		}
	}
	if(unlikely(verbose_run))
	{
		logger << DebugInfo << "Video mode set" << EndDebugInfo;
	}

	stringstream sstr;
	sstr << Object::GetName() << " [" << video_mode.width << "x" << video_mode.height << "x" << video_mode.depth << "]";
	string title = sstr.str();
	SDL_WM_SetCaption(title.c_str(), title.c_str());
	
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = 0;
	}
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, video_mode.width, video_mode.height, sdl_depth, red_mask, green_mask, blue_mask, 0);
	if(!surface)
	{
		logger << DebugError << "Can't create SW surface" << EndDebugError;
		return false;
	}

	//SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
	
	mode_set = true;

	return true;
}

template <class ADDRESS>
void SDL<ADDRESS>::HandleBlit()
{
	SDL_mutexP(sdl_mutex);
	
	uint32_t line;
	uint8_t *dst;
	uint8_t *src;
	ADDRESS scan_line_addr;
	
	if(SDL_LockSurface(surface) < 0) return;
	
	switch(video_mode.depth)
	{
		case 15:
		case 16:
		{
			uint32_t width = video_mode.width;
			uint16_t scan_line[width];
			
			for(line = 0, dst = (uint8_t *) surface->pixels, scan_line_addr = video_mode.fb_addr; line < video_mode.height; line++, dst += surface->pitch, scan_line_addr += video_mode.fb_bytes_per_line)
			{
				if(memory_import->ReadMemory(scan_line_addr, scan_line, 2 * video_mode.width))
				{
					BigEndian2Host((uint16_t *) dst, (uint16_t *) scan_line, video_mode.width);
				}
			}
			break;
		}
			
		case 24:
		case 32:
		{
			uint32_t width = video_mode.width;
			uint32_t scan_line[width];
			
			for(line = 0, dst = (uint8_t *) surface->pixels, scan_line_addr = video_mode.fb_addr; line < video_mode.height; line++, dst += surface->pitch, scan_line_addr += video_mode.fb_bytes_per_line)
			{
				if(memory_import->ReadMemory(scan_line_addr, scan_line, 4 * video_mode.width))
				{
					BigEndian2Host((uint32_t *) dst, (uint32_t *) scan_line, video_mode.width);
				}
			}
			break;
		}
	}
	SDL_UnlockSurface(surface);
			
	if(SDL_BlitSurface(surface, 0, screen, 0) != 0)
	{
		logger << DebugWarning << "Can't blit surface" << EndDebugWarning;
	}
	SDL_UpdateRect(screen, 0, 0, video_mode.width, video_mode.height);
	
	if(!bmp_out_filename.empty())
	{
		char filename[bmp_out_filename.length() + 12];
		sprintf(filename, "%s - %04u.bmp", bmp_out_filename.c_str(), bmp_out_file_number);
		if(SDL_SaveBMP(surface, filename) != 0)
		{
			logger << DebugWarning << "Can't save bitmap into file " << (const char *) filename << EndDebugWarning;
		}
		else
		{
			bmp_out_file_number++;
		}
	}

	SDL_mutexV(sdl_mutex);
}
#endif // HAVE_SDL


template <class ADDRESS>
bool SDL<ADDRESS>::SetVideoMode(ADDRESS fb_addr, uint32_t width, uint32_t height, uint32_t depth, uint32_t fb_bytes_per_line)
{
	bool status = true;
#if defined(HAVE_SDL)
	SDL_mutexP(sdl_mutex);

	SDL_Event user_event;
	user_event.type = SDL_USEREVENT;
	user_event.user.code = EV_SET_VIDEO_MODE;
	user_event.user.data1 = new VideoMode<ADDRESS>(fb_addr, width, height, depth, fb_bytes_per_line);
	user_event.user.data2 = 0;
	
	status = (SDL_PushEvent(&user_event) == 0);

	SDL_mutexV(sdl_mutex);
#endif

	return status;
}

template <class ADDRESS>
void SDL<ADDRESS>::Blit()
{
#if defined(HAVE_SDL)
	if(!mode_set) return;

	SDL_mutexP(sdl_mutex);

	SDL_Event user_event;
	user_event.type = SDL_USEREVENT;
	user_event.user.code = EV_BLIT;
	user_event.user.data1 = 0;
	user_event.user.data2 = 0;
	
	SDL_PushEvent(&user_event);

	SDL_mutexV(sdl_mutex);
#endif
}

template <class ADDRESS>
void SDL<ADDRESS>::RefreshDisplay()
{
	refresh = true;
}

#if defined(HAVE_SDL)
template <class ADDRESS>
void SDL<ADDRESS>::LearnKeyboard()
{
	cout <<  "Please make me learn your host keyboard layout." << endl
			<< "The following links should help you understanding the key numbering:" << endl
			<< "  - http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/scancode.doc" << endl
			<< "  - http://www.barcodeman.com/altek/mule/kbemulator" << endl;
	cout << "Learning will start in 10 seconds." << endl
	        << "Click on window to give it the keyboard focus, then press the asked keys on your keyboard." << endl
	        << "A key is automatically skipped after 10 seconds." << endl;
	cout.flush();
	SDL_Delay(10000);

	ofstream file(learn_keymap_filename.c_str(), ofstream::out);

	if(file.fail())
	{
		logger << DebugError << "Can't open file \"" << learn_keymap_filename << "\" for learning keymap" << EndDebugError;
		return;
	}

	file << "<keymap>" << endl;

	SDL_Event sdl_ev;
	unsigned int loc;
	
	for(loc = 1; loc < 127; loc++)
	{
		cout << "KBD Learn: Press Key #" << loc; cout.flush();
		bool pressed = false;
		unsigned int t = 0;

		while(!pressed && t < 1000)
		{
			if(SDL_PollEvent(&sdl_ev))
			{
				switch(sdl_ev.type)
				{
					case SDL_KEYDOWN:
						{
							if(sdl_ev.key.keysym.sym == SDLK_UNKNOWN) break;
							map<string, SDLKey>::iterator sdlk_iter;

							for(sdlk_iter = sdlk_string_map.begin(); sdlk_iter != sdlk_string_map.end(); sdlk_iter++)
							{
								if((*sdlk_iter).second == sdl_ev.key.keysym.sym)
								{
									string sdlk_string = (*sdlk_iter).first;
									file << "<key sdlk=\"" << sdlk_string << "\" loc=\"" << loc << "\"/>" << endl;
									cout << " " << sdlk_string << endl; cout.flush();
									pressed = true;
								}
							}
							break;
						}
					case SDL_QUIT:
						return;
				}
			}
			SDL_Delay(10);
			t++;
		}
		if(!pressed)
		{
			cout << " skipping..." << endl;
			cout.flush();
		}
	}

	file << "</keymap>" << endl;

	cout << "You keymap is in file \"" << learn_keymap_filename << "\"" << endl;
}
#endif

} // end of namespace sdl
} // end of namespace service
} // end of namespace unisim

#endif
