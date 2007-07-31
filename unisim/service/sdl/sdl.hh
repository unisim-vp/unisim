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

#ifndef __UNISIM_SERVICE_SDL_SDL_HH__
#define __UNISIM_SERVICE_SDL_SDL_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/logger.hh>
#include <unisim/service/interfaces/video.hh>
#include <unisim/service/interfaces/keyboard.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>

#include <inttypes.h>
#include <SDL/SDL.h>

#include <list>
#include <string>

namespace unisim {
namespace service {
namespace sdl {

using std::list;
using std::string;
using namespace unisim::util::endian;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::Logger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Video;
using unisim::service::interfaces::Keyboard;

template <class ADDRESS>
class SDL :
	public Service<Video<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Service<Keyboard>,
	public Client<Logger>
{
public:
	ServiceExport<Video<ADDRESS> > video_export;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceExport<Keyboard> keyboard_export;
	ServiceImport<Logger> logger_import;
	
	SDL(const char *name, Object *parent = 0);
	virtual ~SDL();

	virtual void OnDisconnect();
	virtual bool Setup();

	virtual bool SetVideoMode(ADDRESS fb_addr, uint32_t width, uint32_t height, uint32_t depth, uint32_t fb_bytes_per_line);
	virtual void RefreshDisplay();
	virtual bool GetScancode(uint8_t& scancode);
	
private:
	bool mode_set;
	SDL_Surface *surface;
	bool alive;
	bool refresh;
	SDL_Surface *screen;
	SDL_Thread *refresh_thread;
	SDL_Thread *event_handling_thread;
	SDL_mutex *sdl_mutex;
	SDL_mutex *kbd_mutex;
	uint32_t refresh_period;
	ADDRESS fb_addr;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t fb_bytes_per_line;
	unsigned int bmp_out_file_number;
	string bmp_out_filename;
	list<uint8_t> kbd_scancode_fifo;
	Parameter<uint32_t> param_refresh_period;
	Parameter<string> param_bmp_out_filename;

	static int RefreshThread(SDL<ADDRESS> *sdl);
	static int EventHandlingThread(SDL<ADDRESS> *sdl);
	void RefreshLoop();
	void EventLoop();
	void Blit();
	void PushScancode(uint8_t scancode);
	uint8_t Translate(SDLKey keysym);
	void ProcessKeyboardEvent(SDL_KeyboardEvent& key);
};

} // end of namespace sdl
} // end of namespace service
} // end of namespace unisim

#endif
