/*
 *  Copyright (c) 2018,
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

#include <unisim/kernel/service/ini_config_file_helper.hh>
#include <string>
#include <iostream>
#include <fstream>

namespace unisim {
namespace kernel {
namespace service {

INIConfigFileHelper::INIConfigFileHelper(Simulator *_simulator)
	: simulator(_simulator)
{
	simulator->Register(this);
}

INIConfigFileHelper::~INIConfigFileHelper()
{
}

const char *INIConfigFileHelper::GetName() const
{
	return "INI";
}

bool INIConfigFileHelper::SaveVariables(const char *filename, VariableBase::Type type)
{
	std::ofstream file(filename);
	
	if(file.fail()) return false;
		
	std::list<VariableBase *> variables;
	std::list<VariableBase *>::iterator variable_iter;
	simulator->GetVariables(variables, type);
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = *variable_iter;
		
		if(!variable->GetOwner())
		{
			file << variable->GetVarName() << "=" << (std::string)(*variable) << std::endl;
		}
	}
	
	if(!variables.empty())
	{
		file << std::endl;
	}
	
	std::list<Object *> objects;
	std::list<Object *>::iterator object_iter;
	simulator->GetObjects(objects);
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		Object *object = *object_iter;
		
		SaveVariables(file, object, type);
	}
	
	return true;
}

void INIConfigFileHelper::SaveVariables(std::ostream& os, Object *object, VariableBase::Type type)
{
	std::list<VariableBase *> variables;
	object->GetVariables(variables, type);
	
	if(!variables.empty())
	{
		os << "[" << object->GetName() << "]" << std::endl;

		std::list<VariableBase *>::iterator variable_iter;
		for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
		{
			VariableBase *variable = *variable_iter;
			
			os << variable->GetVarName() << "=" << (std::string)(*variable) << std::endl;
		}
		
		os << std::endl;
	}
}

bool INIConfigFileHelper::LoadVariables(const char *_filename, VariableBase::Type type)
{
	std::string filename = simulator->SearchSharedDataFile(_filename);

	std::ifstream file(filename.c_str());
	
	if(file.fail()) return false;
	
	std::size_t lineno = 1;
	std::string line;
	std::string section;
	std::string key;
	std::string value;
	int state = 0;
	
	while(std::getline(file, line)) // note: line delimiter is not stored
	{
		std::size_t pos = 0;
		std::size_t len = line.length();
		std::size_t eol_pos = len ? (len - 1) : 0;
		
		while(pos < len)
		{
			char c = line[pos];

			switch(state)
			{
				case 0:
					if(c == '[')
					{
						section.clear();
						state = 1;
					}
					else if(c == ';')
					{
						state = 2;
					}
					else if(c == '=')
					{
						std::cerr << "In File \"" << filename << "\", Line #" << lineno << ", missing key before assignment operator ('=')" << std::endl;
						return false;
					}
					else
					{
						key.clear();
						key.append(1, c);
						state = 3;
					}
					break;
					
				case 1:
					if(c == ']')
					{
						state = 0;
					}
					else
					{
						section.append(1, c);
					}
					break;
				case 2:
					if(pos == eol_pos)
					{
						state = 0;
					}
					break;
					
				case 3:
					if(c == '=')
					{
						value.clear();
						if(pos == eol_pos)
						{
							state = 0;
							Assign(section, key, value);
							state = 0;
						}
						else
						{
							state = 4;
						}
					}
					else
					{
						key.append(1, c);
						if(pos == eol_pos)
						{
							std::cerr << "In File \"" << filename << "\", Line #" << lineno << ", expecting assignment operator ('=') after key \"" << key << "\"" << std::endl;
							return false;
						}
					}
					break;
					
				case 4:
					value.append(1, c);
					if(pos == eol_pos) // end of value?
					{
						Assign(section, key, value);
						state = 0;
					}
					break;
			}
			
			pos++;
		}
		
		lineno++;
	}
	
	return file.eof();
}

void INIConfigFileHelper::Assign(const std::string& section, const std::string& key, const std::string& value)
{
	if(section.empty())
	{
		simulator->SetVariable(key.c_str(), value.c_str());
	}
	else
	{
		std::stringstream variable_name_sstr;
		variable_name_sstr << section << "." << key;
		simulator->SetVariable(variable_name_sstr.str().c_str(), value.c_str());
	}
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
