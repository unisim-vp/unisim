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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_STATISTIC_STATISTIC_FILE_SERVER_HH__
#define __UNISIM_SERVICE_STATISTIC_STATISTIC_FILE_SERVER_HH__

#include "unisim/service/interfaces/statistic_reporting.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/util/time/time.hh"
#include <boost/thread/mutex.hpp>
#include <vector>
#include <queue>
#include <string>

namespace unisim {
namespace service {
namespace statistic {

using std::vector;
using std::queue;
using std::string;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::StatisticReporting;
using unisim::service::interfaces::StatisticReportingControl;
using unisim::util::time::time_unit_type;

class StatisticClientIdentifier;
class Statistic;
class TimedStatistic;

class StatisticServer :
	public Object {
private:
	static const unsigned int MAX_SOURCES = 1024;
	typedef StatisticReporting::stat_handle_t stat_handle_t;
	
public:
	ServiceExport<StatisticReporting> 
		*statistic_reporting_export[MAX_SOURCES];
	ServiceImport<StatisticReportingControl> 
		*statistic_reporting_control_import[MAX_SOURCES];
	
	StatisticServer(const char *name, Object *parent = 0);
	~StatisticServer();
	
	virtual bool Setup();
	
	stat_handle_t RegisterStat(unsigned int source_id, const char *source_name, const char *name);
	void ReportStat(stat_handle_t handle,
		uint64_t time,
		uint64_t value);
	void ReportStat(stat_handle_t handle,
		uint64_t time,
		int64_t value);
	void ReportStat(stat_handle_t handle,
		uint64_t time,
		double value);

private:
	static void Display(StatisticServer *server);
	
	Parameter<uint32_t> param_refresh_time;
	uint32_t refresh_time;
	Parameter<double> param_report_frequency;
	double report_frequency;
	Parameter<time_unit_type> param_report_frequency_time_unit;
	time_unit_type report_frequency_time_unit;
	Parameter<string> param_server_name;
	string server_name;
	Parameter<uint32_t> param_server_port;
	uint32_t server_port;
	
	StatisticClientIdentifier *identifier[MAX_SOURCES];
	vector<Statistic> statistic;
	queue<TimedStatistic *> free_stat;
	boost::mutex mutex;
};

class TimedStatistic {
public:
	enum Type {UINT64, INT64, DOUBLE};
	uint64_t time;
	Type type;
	uint64_t u;
	int64_t s;
	double d;
};

class Statistic {
public:
	string source_name;
	string name;
	unsigned int id;
	queue<TimedStatistic *> stat;
};

class StatisticClientIdentifier :
	public Service<StatisticReporting> {
private:
	typedef StatisticReporting::stat_handle_t stat_handle_t;
	
public:
	ServiceExport<StatisticReporting> statistic_reporting_export;
	
	StatisticClientIdentifier(StatisticServer *server, unsigned int source_id, const char *name, Object *parent = 0);
	~StatisticClientIdentifier();
	
	virtual bool Setup();
	
	virtual stat_handle_t RegisterStat(const char *name);
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			uint64_t value);
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			int64_t value);
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			double value);
	
private:
	StatisticServer *server;
	unsigned int source_id;
	string source_name;
};

} // end of namespace statistics
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_STATISTIC_STATISTIC_FILE_SERVER_HH__
