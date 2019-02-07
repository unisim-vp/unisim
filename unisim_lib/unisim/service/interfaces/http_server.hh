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
 
#ifndef __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__
#define __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/util/hypapp/hypapp.hh>

namespace unisim {
namespace service {
namespace interfaces {

struct BrowserAction
{
	enum Tile
	{
		LEFT_TILE,
		TOP_MIDDLE_TILE,
		TOP_RIGHT_TILE,
		BOTTOM_TILE
	};
	
	inline BrowserAction();
	inline BrowserAction(const std::string& object_name, const std::string& name, const std::string& label, const std::string& tab_title, Tile tile, const std::string& path);
	inline BrowserAction(const BrowserAction& browser_action);
	inline void SetObjectName(const std::string& object_name);
	inline void SetName(const std::string& name);
	inline void SetLabel(const std::string& label);
	inline void SetTabTitle(const std::string& tab_title);
	inline void SetTile(Tile tile);
	inline void SetPath(const std::string& path);
	inline const std::string& GetObjectName() const;
	inline const std::string& GetName() const;
	inline const std::string& GetLabel() const;
	inline const std::string& GetTabTitle() const;
	inline Tile GetTile() const;
	inline const std::string& GetPath() const;
private:
	std::string object_name;
	std::string name;
	std::string label;
	std::string tab_title;
	Tile tile;
	std::string path;
};

inline std::ostream& operator << (std::ostream& os, const BrowserAction::Tile& tile);
inline std::ostream& operator << (std::ostream& os, const BrowserAction& browser_action);

struct BrowserActionScanner : public unisim::kernel::service::ServiceInterface
{
  virtual void Append(const BrowserAction& browser_action) = 0;
};

struct HttpServer : unisim::kernel::service::ServiceInterface
{
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn) = 0;
	virtual void ScanBrowserActions(BrowserActionScanner& scanner) = 0;
};

inline std::ostream& operator << (std::ostream& os, const BrowserAction::Tile& tile)
{
	switch(tile)
	{
		case BrowserAction::LEFT_TILE      : os << "left-tile";       break;
		case BrowserAction::TOP_MIDDLE_TILE: os << "top-middle-tile"; break;
		case BrowserAction::TOP_RIGHT_TILE : os << "top-right-tile";  break;
		case BrowserAction::BOTTOM_TILE    : os << "bottom-tile";     break;
		default                            : os << "unknown-tile";    break;
	}
	
	return os;
}

inline BrowserAction::BrowserAction()
	: object_name()
	, name()
	, label()
	, tab_title()
	, tile(TOP_MIDDLE_TILE)
	, path()
{
}

inline BrowserAction::BrowserAction(const std::string& _object_name, const std::string& _name, const std::string& _label, const std::string& _tab_title, Tile _tile, const std::string& _path)
	: object_name(_object_name)
	, name(_name)
	, label(_label)
	, tab_title(_tab_title)
	, tile(_tile)
	, path(_path)
{
}

inline BrowserAction::BrowserAction(const BrowserAction& browser_action)
	: object_name(browser_action.object_name)
	, name(browser_action.name)
	, label(browser_action.label)
	, tab_title(browser_action.tab_title)
	, tile(browser_action.tile)
	, path(browser_action.path)
{
}

inline void BrowserAction::SetObjectName(const std::string& _object_name)
{
	object_name = _object_name;
}

inline void BrowserAction::SetName(const std::string& _name)
{
	name = _name;
}

inline void BrowserAction::SetLabel(const std::string& _label)
{
	label = _label;
}

inline void BrowserAction::SetTabTitle(const std::string& _tab_title)
{
	tab_title = _tab_title;
}

inline void BrowserAction::SetTile(Tile _tile)
{
	tile = _tile;
}

inline void BrowserAction::SetPath(const std::string& _path)
{
	path = _path;
}

inline const std::string& BrowserAction::GetObjectName() const
{
	return object_name;
}

inline const std::string& BrowserAction::GetName() const
{
	return name;
}

inline const std::string& BrowserAction::GetLabel() const
{
	return label;
}

inline const std::string& BrowserAction::GetTabTitle() const
{
	return tab_title;
}

inline BrowserAction::Tile BrowserAction::GetTile() const
{
	return tile;
}

inline const std::string& BrowserAction::GetPath() const
{
	return path;
}

inline std::ostream& operator << (std::ostream& os, const BrowserAction& browser_action)
{
	os << "BrowserAction(object_name=\"" << browser_action.GetObjectName() << "\"";
	os << ",name=\"" << browser_action.GetName() << "\"";
	os << ",label=\"" << browser_action.GetLabel() << "\"";
	os << ",tab_title=\"" << browser_action.GetTabTitle() << "\"";
	os << ",tile=" << browser_action.GetTile();
	os << ",path=\"" << browser_action.GetPath() << "\"";
	return os << ")";
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__

