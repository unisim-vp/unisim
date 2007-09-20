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
 
#ifndef __UNISIM_SERVICE_POWER_CACHE_POWER_ESTIMATOR_HH__
#define __UNISIM_SERVICE_POWER_CACHE_POWER_ESTIMATOR_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/service/power/cacti4_2.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <unisim/service/interfaces/time.hh>
#include <map>
#include <inttypes.h>

namespace unisim {
namespace service {
namespace power {

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ParameterArray;
using std::map;

class CacheProfileKey
{
public:
	unsigned int frequency; // Mhz
	unsigned int voltage; // mV
	
	CacheProfileKey(unsigned int f, unsigned int v) : frequency(f), voltage(v)
	{
	}
	
	inline bool operator < (CacheProfileKey rhs) const
	{
		if(frequency < rhs.frequency) return true;
		if(frequency > rhs.frequency) return false;
		return voltage < rhs.voltage;
	}
};

class CacheProfile : public CacheProfileKey
{
public:
	CacheProfile(
		unsigned int frequency,
		unsigned int voltage,
		double dyn_energy_per_read,
		double dyn_energy_per_write,
		double leak_power);
	
	double dyn_energy_per_read; // in J
	double dyn_energy_per_write; // in J
	double leak_power; // in W
	uint64_t read_accesses;
	uint64_t write_accesses;
	double duration; // in seconds
	
};

class CachePowerEstimator :
	public Service<unisim::service::interfaces::CachePowerEstimator>,
	public Service<unisim::service::interfaces::PowerMode>,
	public Client<unisim::service::interfaces::Time>
{
public:
	typedef enum { ACCESS_MODE_NORMAL, ACCESS_MODE_SEQUENTIAL, ACCESS_MODE_FAST } AccessMode;
	
	ServiceExport<unisim::service::interfaces::CachePowerEstimator> power_estimator_export;
	ServiceExport<unisim::service::interfaces::PowerMode> power_mode_export;
	ServiceImport<unisim::service::interfaces::Time> time_import;
	
 	CachePowerEstimator(const char *name, Object *parent = 0);
 	virtual ~CachePowerEstimator();

 	virtual bool Setup();

	// Cache power estimator interface
	virtual void ReportReadAccess();
	virtual void ReportWriteAccess();
	virtual double GetDynamicEnergy();
	virtual double GetDynamicPower();
	virtual double GetLeakagePower();
	
	// Power mode interface
	virtual void SetPowerMode(unsigned int frequency, unsigned int voltage);
	virtual unsigned int GetMaxFrequency();
	virtual unsigned int GetDefaultVoltage();

private:
	
	CacheProfile *current_profile;
	map<CacheProfileKey, CacheProfile *> profiles;
	
	// Cache Power estimator service parameters
	unsigned int p_cache_size; // cache size in bytes
	unsigned int p_line_size;
	unsigned int p_associativity;
	unsigned int p_rw_ports;
	unsigned int p_excl_read_ports;
	unsigned int p_excl_write_ports;
	unsigned int p_single_ended_read_ports;
	unsigned int p_banks;
	double p_tech_node;
	unsigned int p_output_width;
	unsigned int p_tag_width;
	AccessMode p_access_mode;
	
	Parameter<unsigned int> param_cache_size;
	Parameter<unsigned int> param_line_size;
	Parameter<unsigned int> param_associativity;
	Parameter<unsigned int> param_rw_ports;
	Parameter<unsigned int> param_excl_read_ports;
	Parameter<unsigned int> param_excl_write_ports;
	Parameter<unsigned int> param_single_ended_read_ports;
	Parameter<unsigned int> param_banks;
	Parameter<double> param_tech_node;
	Parameter<unsigned int> param_output_width;
	Parameter<unsigned int> param_tag_width;
	Parameter<AccessMode> param_access_mode;
	
	int Ndwl;
	int Ndbl;
	int Ntwl;
	int Ntbl;
	int Ntspd;
	double Nspd;
	
	unsigned int max_frequency;
	unsigned int default_voltage;
	
	double time_stamp; // in seconds
	
	/* total results */
	Cacti4_2 cacti;
};

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#endif
