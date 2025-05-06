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

#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/util/json/json.tcc>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace unisim {
namespace kernel {
namespace config {
namespace json {

JSONConfigFileHelper::JSONConfigFileHelper(unisim::kernel::Simulator *_simulator)
	: simulator(_simulator)
{
	simulator->Register(this);
}

JSONConfigFileHelper::~JSONConfigFileHelper()
{
}

const char *JSONConfigFileHelper::GetName() const
{
	return "JSON";
}

bool JSONConfigFileHelper::SaveVariables(const char *filename, unisim::kernel::VariableBase::Type type)
{
	std::ofstream file(filename);

	if(file.fail()) return false;

	return SaveVariables(file, type);
}

bool JSONConfigFileHelper::SaveVariables(std::ostream& os, unisim::kernel::VariableBase::Type type)
{
	Indent indent;
	SaveVariables(os, /* object */ 0, type, indent);
	return true;
}

void JSONConfigFileHelper::SaveVariables(std::ostream& os, unisim::kernel::Object *object, unisim::kernel::VariableBase::Type type, Indent& indent)
{
	struct Visitor
	{
		JSONConfigFileHelper& json_config_file_helper;
		std::ostream& os;
		unisim::kernel::Object *object;
		unisim::kernel::VariableBase::Type type;
		Indent& indent;
		bool first;

		Visitor(JSONConfigFileHelper& _json_config_file_helper, std::ostream& _os, unisim::kernel::Object *_object, unisim::kernel::VariableBase::Type _type, Indent& _indent)
			: json_config_file_helper(_json_config_file_helper), os(_os), object(_object), type(_type), indent(_indent), first(true)
		{
		}

		~Visitor()
		{
			End();
		}

		void Next()
		{
			if(first)
			{
				if(object) os << std::endl;
				os << indent << '{';
				++indent;
				first = false;
			}
			else
			{
				os << ", ";
			}
		}

		void End()
		{
			if(first)
			{
				if(object) os << " ";
				os << "{}";
			}
			else
			{
				os << std::endl << --indent << '}';
			}
		}

		bool Visit(unisim::kernel::Object *child)
		{
			if(object || !child->GetParent())
			{
				Next();
				os << std::endl << indent;
				os << "\"" << unisim::util::json::Escape(child->GetObjectName()) << "\" :";
				json_config_file_helper.SaveVariables(os, child, type, indent);
			}
			return false;
		}

		bool Visit(unisim::kernel::VariableBase *variable)
		{
			if((object || !variable->GetOwner()) &&
			   !variable->GetContainer() &&
			   ((type == unisim::kernel::VariableBase::VAR_VOID) ||
			    ((variable->GetType() == unisim::kernel::VariableBase::VAR_ARRAY) || (variable->GetType() == type))))
			{
				Next();
				os << std::endl << indent;
				json_config_file_helper.SaveVariable(os, *variable, type, indent);
			}
			return false;
		}
	} visitor(*this, os, object, type, indent);

	if(object)
	{
		object->ScanVariables(visitor);
		object->ScanChildren(visitor);
	}
	else
	{
		simulator->ScanVariables(visitor);
		simulator->ScanObjects(visitor);
	}
}

void JSONConfigFileHelper::SaveVariableValue(std::ostream& os, unisim::kernel::VariableBase& variable, unisim::kernel::VariableBase::Type type, Indent& indent)
{
	if(variable.GetType() == unisim::kernel::VariableBase::VAR_ARRAY)
	{
		struct Visitor
		{
			JSONConfigFileHelper& json_config_file_helper;
			std::ostream& os;
			unisim::kernel::VariableBase::Type type;
			Indent& indent;
			bool first;

			Visitor(JSONConfigFileHelper& _json_config_file_helper, std::ostream& _os, unisim::kernel::VariableBase::Type _type, Indent& _indent)
				: json_config_file_helper(_json_config_file_helper), os(_os), type(_type), indent(_indent), first(true)
			{
			}

			~Visitor()
			{
				End();
			}

			void Next()
			{
				if(first)
				{
					os << std::endl << indent << '[';
					++indent;
					first = false;
				}
				else
				{
					os << ", ";
				}
			}

			void End()
			{
				if(first)
				{
					os << " []";
				}
				else
				{
					os << std::endl << --indent << ']';
				}
			}

			bool Visit(std::size_t index, unisim::kernel::VariableBase *variable)
			{
				if((type == unisim::kernel::VariableBase::VAR_VOID) ||
				   ((variable->GetType() == unisim::kernel::VariableBase::VAR_ARRAY) || (variable->GetType() == type)))
				{
					Next();
					os << std::endl << indent;
					json_config_file_helper.SaveVariableValue(os, *variable, type, indent);
				}
				return false;
			}
		} visitor(*this, os, type, indent);

		variable.AsArray().ScanVariables(visitor, type);
	}
	else
	{
		unisim::kernel::VariableBase::DataType dt = variable.GetDataType();
		if((dt != unisim::kernel::VariableBase::DT_USER) &&
		   (dt != unisim::kernel::VariableBase::DT_STRING) &&
		   ((dt == unisim::kernel::VariableBase::DT_BOOL) ||
		   (dt == unisim::kernel::VariableBase::DT_DOUBLE) ||
		   (dt == unisim::kernel::VariableBase::DT_FLOAT) ||
		   (variable.GetFormat() == unisim::kernel::VariableBase::FMT_DEC)))
		{
			os << (std::string) variable;
		}
		else
		{
			os << "\"" << unisim::util::json::Escape((std::string) variable) << "\"";
		}
	}
}

void JSONConfigFileHelper::SaveVariable(std::ostream& os, unisim::kernel::VariableBase& variable, unisim::kernel::VariableBase::Type type, Indent& indent)
{
	os << "\"" << unisim::util::json::Escape(variable.GetVarName()) << "\" : ";
	SaveVariableValue(os, variable, type, indent);
}

bool JSONConfigFileHelper::LoadVariables(const char *_filename, unisim::kernel::VariableBase::Type type)
{
	std::string filename = simulator->SearchSharedDataFile(_filename);

	std::ifstream file(filename.c_str());

	if(file.fail()) return false;

	return LoadVariables(file, type);
}

struct JSON_AST_Visitor
{
	JSON_AST_Visitor(unisim::kernel::Simulator *_simulator)
		: simulator(_simulator)
		, ctx_stack()
	{
	}

	template <typename T> void SetVariable(const T& value)
	{
		std::string variable_name = VariableName();
		simulator->SetVariable(variable_name.c_str(), value);
	}

	bool Visit(const unisim::util::json::JSON_Value& value)
	{
		throw std::runtime_error("Internal error!");
		return true;
	}

	bool Visit(const unisim::util::json::JSON_String& value)
	{
		SetVariable((const char *) value);
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Integer& value)
	{
		SetVariable((int64_t) value);
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Float& value)
	{
		SetVariable((double) value);
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Boolean& value)
	{
		SetVariable((bool) value);
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Null& value)
	{
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Object& object)
	{
		object.Scan(*this);
		if(InArray()) NextIndex();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Member& member)
	{
		PushMemberContext(member.GetName());
		member.Scan(*this);
		PopContext();
		return false;
	}

	bool Visit(const unisim::util::json::JSON_Array& array)
	{
		PushArrayContext();
		array.Scan(*this);
		PopContext();
		if(InArray()) NextIndex();
		return false;
	}

	void PushMemberContext(const std::string& name)
	{
		ctx_stack.push_back(Context(Context::CTX_MEMBER, name, 0));
	}

	void PushArrayContext()
	{
		ctx_stack.push_back(Context(Context::CTX_ARRAY, std::string(), 0));
	}

	void PopContext()
	{
		ctx_stack.pop_back();
	}

	bool InArray() const
	{
		return (ctx_stack.size() != 0) && (ctx_stack.back().type == Context::CTX_ARRAY);
	}

	void NextIndex()
	{
		if(ctx_stack.size() != 0)
		{
			++ctx_stack.back().index;
		}
	}

	std::string VariableName() const
	{
		std::string variable_name;
		unsigned int n = ctx_stack.size();
		for(unsigned int i = 0; i < n; ++i)
		{
			const Context& ctx = ctx_stack[i];

			switch(ctx.type)
			{
				case Context::CTX_MEMBER:
				{
					const std::string& name = ctx.name;
					if(name.length() != 0)
					{
						if(variable_name.length() != 0)
						{
							variable_name += '.';
						}
						variable_name += name;
					}
					break;
				}

				case Context::CTX_ARRAY:
				{
					std::ostringstream sstr;
					sstr << '[' << ctx.index << ']';
					variable_name += sstr.str();
					break;
				}
			}
		}
		return variable_name;
	}

	unsigned int GetIndex() const
	{
		return ctx_stack.back().index;
	}

	struct Context
	{
		enum Type
		{
			CTX_MEMBER,
			CTX_ARRAY
		};

		Context() : type(), name(), index(0) {}
		Context(Type _type, const std::string& _name, unsigned int _index) : type(_type), name(_name), index(_index) {}

		Type type;
		std::string name;
		unsigned int index;
	};

	unisim::kernel::Simulator *simulator;
	typedef std::vector<Context> ContextStack;
	ContextStack ctx_stack;

};

bool JSONConfigFileHelper::LoadVariables(std::istream& is, unisim::kernel::VariableBase::Type type)
{
	unisim::util::json::JSON_Parser<unisim::util::json::JSON_AST_Builder> json_parser;
	unisim::util::json::JSON_AST_Builder json_ast_builder;

	unisim::util::json::JSON_Value *root = json_parser.Parse(is, json_ast_builder);

	if(root)
	{
		JSON_AST_Visitor json_ast_visitor(simulator);
		root->Visit(json_ast_visitor);
		delete root;

		return true;
	}

	return false;
}

} // end of namespace json
} // end of namespace config
} // end of namespace kernel
} // end of namespace unisim
