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
 
#include <unisim/service/power/cache_power_estimator.hh>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace unisim {
namespace kernel {
namespace service {

template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::Parameter(const char *_name, Object *_object, unisim::service::power::CachePowerEstimator::AccessMode& _storage, const char *_description) :
ParameterBase(_name, _object, _description), storage(&_storage)
{
}

template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator bool () const { return false; }
template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator long long () const { return (long long)(*storage); }
template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator unsigned long long () const { return (unsigned long long)(*storage); }
template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator double () const { return (double)(unsigned int)(*storage); }
template <> Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator string () const
{
	switch(*storage)
	{
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL: return string("normal");
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL: return string("sequential");
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST: return string("fast");
	}
	return string("?");
}

template <> ParameterBase& Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (bool value) { return *this;}

template <> ParameterBase& Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (unsigned long long value)
{
	switch(value)
	{
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL:
			*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL;
			break;
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST:
			*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST;
			break;
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL:
		default:
			*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL;
			break;
	}
	return *this;
}

template <> ParameterBase& Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (long long value)
{
	return (*this) = (unsigned long long) value;
}

template <> ParameterBase& Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (double value)
{
	return (*this) = (unsigned long long) value;
}

template <> ParameterBase& Parameter<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (const char *value)
{
	if(strcmp(value, "normal") == 0) *storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL; else
	if(strcmp(value, "sequential") == 0) *storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL; else
	if(strcmp(value, "fast") == 0) *storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST;
	
	return *this;
}

template class Parameter<unisim::service::power::CachePowerEstimator::AccessMode>;

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim

namespace unisim {
namespace service {
namespace power {

using std::cerr;
using std::endl;
using std::string;

CacheProfile::CacheProfile(
	unsigned int _frequency,
	unsigned int _voltage,
	double _dyn_energy_per_read,
	double _dyn_energy_per_write,
	double _leak_power) :
	CacheProfileKey(_frequency, _voltage),
	dyn_energy_per_read(_dyn_energy_per_read),
	dyn_energy_per_write(_dyn_energy_per_write),
	leak_power(_leak_power),
	read_accesses(0),
	write_accesses(0),
	duration(0.0)
{
}


CachePowerEstimator::CachePowerEstimator(const char *name, Object *parent) :
	Object(name, parent),
	Service<unisim::service::interfaces::CachePowerEstimator>(name, parent),
	Service<unisim::service::interfaces::PowerMode>(name, parent),
	Client<unisim::service::interfaces::Time>(name, parent),
	power_estimator_export("power-estimator-export", this),
	power_mode_export("power-mode-export", this),
	time_import("time-import", this),
	current_profile(0),
	p_cache_size(0),
	p_line_size(0),
	p_associativity(0),
	p_rw_ports(0),
	p_excl_read_ports(0),
	p_excl_write_ports(0),
	p_single_ended_read_ports(0),
	p_banks(0),
	p_tech_node(0.0),
	p_output_width(0),
	p_tag_width(0),
	p_access_mode(ACCESS_MODE_NORMAL),
	param_cache_size("cache-size", this, p_cache_size),
	param_line_size("line-size", this, p_line_size),
	param_associativity("associativity", this, p_associativity),
	param_rw_ports("rw-ports", this, p_rw_ports),
	param_excl_read_ports("excl-read-ports", this, p_excl_read_ports),
	param_excl_write_ports("excl-write-ports", this, p_excl_write_ports),
	param_single_ended_read_ports("single-ended-read-ports", this, p_single_ended_read_ports),
	param_banks("banks", this, p_banks),
	param_tech_node("tech-node", this, p_tech_node),
	param_output_width("output-width", this, p_output_width),
	param_tag_width("tag-width", this, p_tag_width),
	param_access_mode("access-mode", this, p_access_mode),
	Ndwl(0),
	Ndbl(0),
	Ntwl(0),
	Ntbl(0),
	Ntspd(0),
	Nspd(0.0),
	max_frequency(0),
	time_stamp(0.0)
{
}

CachePowerEstimator::~CachePowerEstimator()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	double dyn_energy = 0.0;

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		delete prof;
	}
}

void CachePowerEstimator::SetPowerMode(unsigned int frequency, unsigned int voltage)
{
	double new_time_stamp = time_import->GetTime(); // in seconds
	
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	
	prof_iter = profiles.find(CacheProfileKey(frequency, voltage));
	
	if(prof_iter != profiles.end())
	{
		if(new_time_stamp > time_stamp)
		{
			// accumulate the time spent in the current profile
			current_profile->duration = current_profile->duration + (new_time_stamp - time_stamp);
			time_stamp = new_time_stamp;
		}
		// switch to the new profile
		current_profile = (*prof_iter).second;
		return;
	}
	
	Cacti4_2::total_result_type total_result;
	
	total_result = cacti.cacti_interface(
			p_cache_size,
			p_line_size,
			p_associativity,
			p_rw_ports,
			p_excl_read_ports,
			p_excl_write_ports,
			p_single_ended_read_ports,
			p_banks,
			p_tech_node / 1000.0,
			p_output_width,
			p_tag_width ? 1:0,
			p_tag_width,
			p_access_mode,
			0,
			Ndwl,
			Ndbl,
			Ntwl,
			Ntbl,
			Ntspd,
			Nspd,
			(double) voltage / 1e3 /* V */);
	
	//cacti.output_data(&total_result.result,&total_result.area,&total_result.params);
	
	if(frequency > max_frequency)
	{
		cerr << GetName() << ": WARNING! A frequency of " << frequency << " Mhz is too high for the simulated hardware !" << endl;
	}
	
	CacheProfile *prof = new CacheProfile(
		frequency,
		voltage,
		total_result.result.total_power_allbanks.readOp.dynamic,
		total_result.result.total_power_allbanks.writeOp.dynamic,
		total_result.result.total_power_allbanks.readOp.leakage);
	
	profiles[*prof] = prof;
	current_profile = prof;
	time_stamp = new_time_stamp;

// 	cerr << "(cycle-time=" << (1e3 / (double) prof->frequency)
// 			<<" ns, min-cycle-time=" << (total_result.result.cycle_time * 1e9)
// 			<<  " ns, frequency=" << prof->frequency
// 			<< " Mhz, voltage=" << ((double) prof->voltage / 1e3)
// 			<< " V, dyn-energy-per-read=" << (prof->dyn_energy_per_read * 1e9)
// 			<< " nJ, dyn-energy-per-write=" << (prof->dyn_energy_per_write * 1e9)
// 			<< " nJ, max-dyn-power=" << (prof->dyn_energy_per_read * prof->frequency * 1e6)
// 			<< " W, leak-power=" << prof->leak_power << " W)" << endl;

}

unsigned int CachePowerEstimator::GetMaxFrequency()
{
	return max_frequency;
}

unsigned int CachePowerEstimator::GetDefaultVoltage()
{
	return default_voltage;
}

void CachePowerEstimator::ReportReadAccess()
{
	current_profile->read_accesses++;
}

void CachePowerEstimator::ReportWriteAccess()
{
	current_profile->write_accesses++;
}

double CachePowerEstimator::GetDynamicEnergy()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	double dyn_energy = 0.0;

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		
/*		cerr << "read_accesses = " << prof->read_accesses << endl;
		cerr << "write_accesses = " << prof->write_accesses << endl;*/
		dyn_energy = dyn_energy + prof->dyn_energy_per_read * prof->read_accesses + prof->dyn_energy_per_write * prof->write_accesses;
	}
	return dyn_energy;
}

double CachePowerEstimator::GetDynamicPower()
{
	double dyn_energy = GetDynamicEnergy(); // in J
	double total_time = time_import->GetTime(); // in seconds
	return dyn_energy / total_time;
}

double CachePowerEstimator::GetLeakagePower()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	double leakage_power = 0.0;
	double total_time = time_import->GetTime(); // in seconds
	double t = 0.0;

	if(total_time > time_stamp)
	{
		// accumulate the time spent in the current profile
		current_profile->duration = current_profile->duration + (total_time - time_stamp);
		time_stamp = total_time;
	}

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		
		leakage_power = leakage_power + prof->leak_power * (prof->duration / total_time);
	}
	return leakage_power;
}

bool CachePowerEstimator::Setup()
{
	if(!time_import)
	{
		cerr << GetName() << ": ERROR! no time service is connected." << endl;
		return false;
	}
	int i, j;
	cerr << GetName() << ": " << ((double) p_cache_size / 1024.0) << " KB cache" << endl;
	cerr << GetName() << ": " << p_line_size << " bytes per line" << endl;
	cerr << GetName() << ": " << p_associativity << " way/set associative" << endl;
	cerr << GetName() << ": " << p_rw_ports << " read/write ports" << endl;
	cerr << GetName() << ": " << p_excl_read_ports << " read-only ports" << endl;
	cerr << GetName() << ": " << p_excl_write_ports << " write-only ports" << endl;
	cerr << GetName() << ": " << p_single_ended_read_ports << " single-ended-read-ports" << endl;
	cerr << GetName() << ": " << p_banks << " banks" << endl;
	cerr << GetName() << ": " << p_tech_node << " nm tech-node" << endl;
	cerr << GetName() << ": " << p_output_width << "-bit output" << endl;
	if(p_tag_width) cerr << GetName() << ": " << p_tag_width << "-bit tag" << endl;
	cerr << GetName() << ": ";
	switch(p_access_mode)
	{
		case ACCESS_MODE_NORMAL: cerr << "normal"; break;
		case ACCESS_MODE_SEQUENTIAL: cerr << "sequential"; break;
		case ACCESS_MODE_FAST: cerr << "fast"; break;
		default: cerr << "?";
	}
	cerr << " access mode" << endl;

	Cacti4_2::total_result_type total_result;
	double VddPow;

	total_result = cacti.cacti_interface(
			p_cache_size,
	p_line_size,
	p_associativity,
	p_rw_ports,
	p_excl_read_ports,
	p_excl_write_ports,
	p_single_ended_read_ports,
	p_banks,
	p_tech_node / 1000.0,
	p_output_width,
	p_tag_width ? 1:0,
	p_tag_width,
	p_access_mode,
	0,
	&VddPow);

	//cacti.output_data(&total_result.result,&total_result.area,&total_result.params);

	Ndwl = total_result.result.best_Ndwl;
	Ndbl = total_result.result.best_Ndbl;
	Ntwl = total_result.result.best_Ntwl;
	Ntbl = total_result.result.best_Ntbl;
	Ntspd = total_result.result.best_Ntspd;
	Nspd = total_result.result.best_Nspd;
	
	max_frequency = (unsigned int)(1.0 / (total_result.result.cycle_time * 1e6));
	default_voltage = (unsigned int) floor(VddPow * 1000.0);
	
	cerr << GetName() << ": Ndwl=" << Ndwl << ", Ndbl=" << Ndbl
			<< ", Ntwl=" << Ntbl << ", Ntspd=" << Ntspd
			<< ", Nspd=" << Nspd << ", max frequency=" << max_frequency << " Mhz, default VddPow=" << VddPow << " V" << endl;

	
	SetPowerMode(max_frequency, default_voltage);
	return true;
}

} // end of namespace power
} // end of namespace service
} // end of namespace unisim
