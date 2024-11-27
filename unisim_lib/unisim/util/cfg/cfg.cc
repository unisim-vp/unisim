/*
 *  Copyright (c) 2024,
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

#include <unisim/util/cfg/cfg.hh>
#include <unisim/util/json/json.tcc>

namespace unisim {
namespace util {
namespace cfg {

using unisim::util::json::JSON_Value;
using unisim::util::json::JSON_String;
using unisim::util::json::JSON_Number;
using unisim::util::json::JSON_Integer;
using unisim::util::json::JSON_Float;
using unisim::util::json::JSON_Boolean;
using unisim::util::json::JSON_Null;
using unisim::util::json::JSON_Object;
using unisim::util::json::JSON_Member;
using unisim::util::json::JSON_Array;

//==============================================================================
//                               Static variables
//==============================================================================

static unsigned next_extension_id[3] = { 0, 0, 0 };

//==============================================================================
//                               Definitions
//==============================================================================

// Allocate an extension ID of a particular extension type
unsigned AllocateExtensionId(ExtensionType ext_type)
{
	return next_extension_id[ext_type]++;
}

// Get number of extension IDs for a particular extension type
unsigned ExtensionIds(ExtensionType ext_type)
{
	return next_extension_id[ext_type];
}

// Helper function to convert a string to HTML
std::string StringToHTML(const std::string& s)
{
	std::string r;
	for(std::string::size_type pos = 0; pos < s.length(); ++pos)
	{
		char ch = s[pos];
		switch(ch)
		{
			case '\n': r += "<BR/>"; break;
			case ' ': r += "&nbsp;"; break;
			case '\t': r += "&ensp;"; break;
			case '<': r += "&lt;"; break;
			case '>': r += "&gt;"; break;
			case '&': r += "&amp;"; break;
			case '"': r += "&quot;"; break;
			case '\'': r += "&apos;"; break;
			default: r += ch;
		}
	}
	return r;
}

// Helper class for instruction type
void Type::Print(std::ostream& stream) const
{
	switch(value)
	{
		case STANDARD: stream << "standard"; break;
		case JUMP    : stream << "jump";     break;
		case BRANCH  : stream << "branch";   break;
		case CALL    : stream << "call";     break;
		case RETURN  : stream << "return";   break;
	}
}

void Type::Set(const std::string& s)
{
	if(s == "standard") value = STANDARD;
	else if(s == "jump") value = JUMP;
	else if(s == "branch") value = BRANCH;
	else if(s == "call") value = CALL;
	else if(s == "return") value = RETURN;
}

// Helper class for control flow mode
void Mode::Print(std::ostream& stream) const
{
	switch(value)
	{
		case DIRECT  : stream << "direct";   break;
		case INDIRECT: stream << "indirect"; break;
	}
}

void Mode::Set(const std::string& s)
{
	if(s == "direct") value = DIRECT;
	else if(s == "indirect") value = INDIRECT;
}

// Helper class for node category
void Category::Print(std::ostream& stream) const
{
	switch(value)
	{
		case ENTRY  : stream << "entry";   break;
		case BLOCK  : stream << "block";   break;
		case PHANTOM: stream << "phantom"; break;
		case EXIT   : stream << "exit";    break;
		case HALT   : stream << "halt";    break;
		case UNKNOWN: stream << "unknown"; break;
	}
}

void Category::Set(const std::string& s)
{
	if(s == "entry") value = ENTRY;
	else if(s == "block") value = BLOCK;
	else if(s == "phantom") value = PHANTOM;
	else if(s == "exit") value = EXIT;
	else if(s == "halt") value = HALT;
	else if(s == "unknown") value = UNKNOWN;
}

// Helper class for edge tag
void EdgeTag::Print(std::ostream& stream) const
{
	switch(value)
	{
		case EMPTY      : stream << "empty";       break;
		case TARGET     : stream << "target";      break;
		case FALLTHROUGH: stream << "fallthrough"; break;
	}
}

void EdgeTag::Set(const std::string& s)
{
	if(s == "empty") value = EMPTY;
	else if(s == "target") value = TARGET;
	else if(s == "fallthrough") value = FALLTHROUGH;
}

//////////////////////////////// ExtensionBase ////////////////////////////////

ExtensionBase::~ExtensionBase()
{
}

void ExtensionBase::Free()
{
	delete this;
}

///////////////////////////////// EdgeValueBase ///////////////////////////////

EdgeValueBase::EdgeValueBase(const JSON_Value& value)
	: count(0)
	, tag(EMPTY)
{
	const JSON_Object& edge_object = value.AsObject();
	if((const std::string&) edge_object["kind"].AsString() != "edge") throw std::runtime_error("Expected \"edge\" as \"kind\"");
	
	count = edge_object["count"].AsInteger();
	
	if(edge_object.HasProperty("tag"))
	{
		const std::string& edge_tag_name = (const std::string&) edge_object["tag"].AsString();
		EdgeTag edge_tag_struct(edge_tag_name);
		tag = edge_tag_struct;
	}
}

void EdgeValueBase::Save(JSON_Value& value) const
{
	JSON_Object& edge_object = value.AsObject();
	
	edge_object.Add(new JSON_Member("count", new JSON_Integer(count)));
	if(tag != EMPTY) edge_object.Add(new JSON_Member("tag", new JSON_String(ToString(EdgeTag(tag)))));
}

////////////////////////////////// NodeBase ///////////////////////////////////

JSON_Object *NodeBase::SaveAsObject() const
{
	JSON_Object *object = new JSON_Object();
	object->Add(new JSON_Member("kind", new JSON_String("node")));
	object->Add(new JSON_Member("category", new JSON_String(ToString(Category(GetCategory())))));
	return object;
}

} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim
