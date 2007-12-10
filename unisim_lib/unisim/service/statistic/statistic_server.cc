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

#include "unisim/service/statistic/statistic_server.hh"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>

namespace unisim {
namespace service {
namespace statistic {

using std::cout;
using std::endl;
using std::stringstream;

//class StatisticServer :
//	public Client<StatisticControlReporting> {
//private:
//	static const unsigned int MAX_SOURCES = 1024;
//	
//public:
//	ServiceExport<StatisticReporting> 
//		*statistic_reporting_export[MAX_SOURCES];
//	ServiceImport<StatisticReportingControl> 
//		*statistic_import[MAX_SOURCES];
//	

StatisticServer::
StatisticServer(const char *name, Object *parent) :
	Object(name, parent),
	refresh_time(10),
	param_refresh_time("refresh-time", this, refresh_time, "host time statistics refreshing period"),
	statistic() {
	for(unsigned int i = 0; i < MAX_SOURCES; i++) {
		stringstream ss;
		ss << "statistic-client-identifier[" << i << "]";
		identifier[i] = new StatisticClientIdentifier(this, i, ss.str().c_str(), this);
	}
	for(unsigned int i = 0; i < MAX_SOURCES; i++) {
		stringstream ss;
		ss << "statistic_reporting_export[" << i << "]";
		statistic_reporting_export[i] =
			new ServiceExport<StatisticReporting>(ss.str().c_str(), this);
		stringstream sc;
		sc << "statistic_reporting_control_import[" << i << "]";
		statistic_reporting_control_import[i] =
			new ServiceImport<StatisticReportingControl>(sc.str().c_str(), this);
	}
	for(unsigned int i = 0; i < MAX_SOURCES; i++) {
		*statistic_reporting_export[i] >> identifier[i]->statistic_reporting_export;
	}
}

StatisticServer::
~StatisticServer() {
}

bool 
StatisticServer::
Setup() {
	uint64_t time = refresh_time;
	for(unsigned int i = 0; i < 5; i++) time = time * 1000;
	time = time / 2;
	boost::thread thrd(boost::bind(StatisticServer::Display, this));
	cout << "Preferred Stat Reporting Period = " << time 
		<< " (" << ((double)time) << ")" << endl;
	for(unsigned int i = 0; i < MAX_SOURCES; i++) {
		if(*statistic_reporting_control_import[i]) {
			(*statistic_reporting_control_import[i])->SetPreferredStatReportingPeriod(time);
		}
	}
	return true;
}

StatisticReporting::stat_handle_t 
StatisticServer::
RegisterStat(unsigned int source_id, const char *source_name, const char *name) {
	boost::mutex::scoped_lock lock(mutex);
	unsigned int ret_id = 0;
	vector<Statistic>::iterator it;
	for(it = statistic.begin(); it != statistic.end(); it++) {
		if(it->source_name.compare(source_name) == 0)
			if(it->name.compare(name) == 0)
				return it->id;
	}
	Statistic *new_stat = new Statistic();
	new_stat->source_name = source_name;
	new_stat->name = name;
	ret_id = statistic.size();
	new_stat->id = ret_id;
	statistic.push_back(*new_stat);
	delete new_stat;
	
	return ret_id;
}

void 
StatisticServer::
ReportStat(stat_handle_t handle,
		uint64_t time,
		uint64_t value) {
	boost::mutex::scoped_lock lock(mutex);
	TimedStatistic *stat;
	if(free_stat.empty())
		stat = new TimedStatistic();
	else {
		stat = free_stat.front();
		free_stat.pop();
	}
	stat->time = time;
	stat->type = TimedStatistic::UINT64;
	stat->u = value;
	statistic[handle].stat.push(stat);
}

void 
StatisticServer::
ReportStat(stat_handle_t handle,
		uint64_t time,
		int64_t value) {
	boost::mutex::scoped_lock lock(mutex);
	TimedStatistic *stat;
	if(free_stat.empty())
		stat = new TimedStatistic();
	else {
		stat = free_stat.front();
		free_stat.pop();
	}
	stat->time = time;
	stat->type = TimedStatistic::INT64;
	stat->s = value;
	statistic[handle].stat.push(stat);
}
	
void 
StatisticServer::
ReportStat(stat_handle_t handle,
		uint64_t time,
		double value) {
	boost::mutex::scoped_lock lock(mutex);
	TimedStatistic *stat;
	if(free_stat.empty())
		stat = new TimedStatistic();
	else {
		stat = free_stat.front();
		free_stat.pop();
	}
	stat->time = time;
	stat->type = TimedStatistic::DOUBLE;
	stat->d = value;
	statistic[handle].stat.push(stat);
}

void
StatisticServer::
Display(StatisticServer *server) {
	boost::mutex::scoped_lock *lock;
	while(1) {
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += server->refresh_time; // change xt to next second
		boost::thread::sleep(xt);
		lock = new boost::mutex::scoped_lock(server->mutex);
		vector<Statistic>::iterator it;
		for(it = server->statistic.begin(); it != server->statistic.end(); it++) {
			cout << "Statistic: " << it->source_name << "." << it->name << endl;
			cout << "   <" << it->stat.size() << " elements>" << endl;
			if(it->stat.empty()) {
				cout << "   <no statistic reported>" << endl;
			} else {
				while(!(it->stat.empty())) {
					TimedStatistic *cur = it->stat.front();
					if(cur->type == TimedStatistic::UINT64)
						cout << "   " << cur->u;
					else if(cur->type == TimedStatistic::INT64)
						cout << "   " << cur->s;
					else if(cur->type == TimedStatistic::DOUBLE)
						cout << "   " << cur->d;
					cout << " (" << cur->time << ")" << endl;
					it->stat.pop();
					server->free_stat.push(cur);
				}
			}
		}
		delete lock;
	}
}

StatisticClientIdentifier::
StatisticClientIdentifier(StatisticServer *_server, unsigned int _source_id, const char *name, Object *parent) :
Object(name, parent),
Service<StatisticReporting>(name,parent),
statistic_reporting_export("statistic_reporting_export", this),
server(_server),
source_id(_source_id)
{
	
}

StatisticClientIdentifier::
~StatisticClientIdentifier() {
	
}


bool
StatisticClientIdentifier::
Setup() {
	if(statistic_reporting_export.GetClient() != 0) 
		source_name = statistic_reporting_export.GetClient()->GetName();
	else
		source_name = "(unknown source name)";
}

StatisticReporting::stat_handle_t
StatisticClientIdentifier::
RegisterStat(const char *name) {
	server->RegisterStat(source_id, source_name.c_str(), name);
}

void 
StatisticClientIdentifier::
ReportStat(stat_handle_t handle,
		uint64_t time,
		uint64_t value) {
	server->ReportStat(handle, time, value);
}

void 
StatisticClientIdentifier::
ReportStat(stat_handle_t handle,
		uint64_t time,
		int64_t value) {
	server->ReportStat(handle, time, value);
}

void 
StatisticClientIdentifier::
ReportStat(stat_handle_t handle,
		uint64_t time,
		double value) {
	server->ReportStat(handle,time, value);
}

//Statistics::
//Statistics(const char *name, Object *parent) :
//	Object(name, parent),
//	Service<StatisticsIf>(name, parent),
//	statistics_export("statistics_export", this),
//	refresh_time(1),
//	param_refresh_time("refresh-time", this, refresh_time),
//	stats(0)
//	{
//}
//
//Statistics::
//~Statistics() {
//}
//
//bool
//Statistics::
//Setup() {
//	cerr << "Setting up Statistics" << endl;
//	boost::thread thrd(boost::bind(Statistics::Display, this));
////	boost::thread thrd(boost::bind(&Statistics::Display, this));
//	cerr << "Statistics set up" << endl;
//	return true;
//}
//
//bool 
//Statistics::
//AddSource(const char *name, uint32_t &source_id) {
//	// check that name is not already used
//	vector<SourceStatistics *>::iterator it;
//	for(it = sources.begin(); it != sources.end(); it++) {
//		if(((SourceStatistics *)*it)->HasName(name) == 0)
//			return false;
//	}
//	sources.push_back(new SourceStatistics(name));
//	source_id = sources.size() - 1;
//	return true;
//}
//
//bool 
//Statistics::
//AddStatistic(uint32_t source_id, const char *name, uint32_t *stat, const char *description) {
//	if(source_id >= sources.size()) return false;
//	SourceStatistics *source = sources[source_id];
//	return source->AddStatistic(name, stat, description);
//}
//
//bool 
//Statistics::
//AddStatistic(uint32_t source_id, const char *name, uint64_t *stat, const char *description) {
//	if(source_id >= sources.size()) return false;
//	SourceStatistics *source = sources[source_id];
//	return source->AddStatistic(name, stat, description);
//}
//
//bool 
//Statistics::
//AddStatistic(uint32_t source_id, const char *name, double *stat, const char *description) {
//	if(source_id >= sources.size()) return false;
//	SourceStatistics *source = sources[source_id];
//	return source->AddStatistic(name, stat, description);
//}
//
//bool 
//Statistics::
//AddStatistic(uint32_t source_id, const char *name, float *stat, const char *description) {
//	if(source_id >= sources.size()) return false;
//	SourceStatistics *source = sources[source_id];
//	return source->AddStatistic(name, stat, description);
//}
//
//bool 
//Statistics::
//Update(uint32_t source_id, double time) {
//	if(source_id >= sources.size()) return false;
//	return sources[source_id]->Update(time);
//}
//
//void
//Statistics::
//Display(Statistics *stats) {
//	while(1) {
//		boost::xtime xt;
//		boost::xtime_get(&xt, boost::TIME_UTC);
//		xt.sec += stats->refresh_time; // change xt to next second
//		boost::thread::sleep(xt);
//		vector<SourceStatistics *>::iterator it;
//		for(it = stats->sources.begin(); it != stats->sources.end(); it++) {
//			vector<SingleStatistic<uint64_t> *>::iterator is;
//			for(is = (*it)->stats64.begin(); is != (*it)->stats64.end(); is++) {
//				cerr << (*is)->GetName() << " = "
//					<< (*is)->GetValue() << endl;
//			}
//		}
////		vector<SingleStatistic<uint64_t> *>::iterator it;
////		for(it = sources[0]->stats64.begin(); it !=  sources[0]->stats64.end(); it++) {
////			cerr << ((SingleStatistic<uint64_t> *)*it)->GetName() << " = "
////				<< ((SingleStatistic<uint64_t> *)*it)->GetValue() << endl;
////			if(((SingleStatistic<uint64_t> *)*it)->HasName(name) == 0)
////				return false;
////		}
//	}
//}
//
//SourceStatistics::
//SourceStatistics(const char *_name) :
//	name(_name) {
//}
//
//SourceStatistics::
//~SourceStatistics() {
//}
//
//const char *
//SourceStatistics::
//GetName() const {
//	return name.c_str();
//}
//
//bool
//SourceStatistics::
//HasName(const char *name) const {
//	return this->name.compare(name) == 0; 
//}
//
//bool 
//SourceStatistics::
//AddStatistic(const char *name, uint32_t *stat, const char *description) {
//	// check that name is not already used
//	vector<SingleStatistic<uint32_t> *>::iterator it;
//	for(it = stats32.begin(); it != stats32.end(); it++) {
//		if(((SingleStatistic<uint32_t> *)*it)->HasName(name) == 0)
//			return false;
//	}
//	stats32.push_back(new SingleStatistic<uint32_t>(name, stat, description));
//	return true;
//}
//
//bool 
//SourceStatistics::
//AddStatistic(const char *name, uint64_t *stat, const char *description) {
//	// check that name is not already used
//	vector<SingleStatistic<uint64_t> *>::iterator it;
//	for(it = stats64.begin(); it != stats64.end(); it++) {
//		if(((SingleStatistic<uint64_t> *)*it)->HasName(name) == 0)
//			return false;
//	}
//	stats64.push_back(new SingleStatistic<uint64_t>(name, stat, description));
//	return true;
//}
//
//bool 
//SourceStatistics::
//AddStatistic(const char *name, double *stat, const char *description) {
//	// check that name is not already used
//	vector<SingleStatistic<double> *>::iterator it;
//	for(it = statsd.begin(); it != statsd.end(); it++) {
//		if(((SingleStatistic<double> *)*it)->HasName(name) == 0)
//			return false;
//	}
//	statsd.push_back(new SingleStatistic<double>(name, stat, description));
//	return true;
//}
//
//bool 
//SourceStatistics::
//AddStatistic(const char *name, float *stat, const char *description) {
//	// check that name is not already used
//	vector<SingleStatistic<float> *>::iterator it;
//	for(it = statsf.begin(); it != statsf.end(); it++) {
//		if(((SingleStatistic<float> *)*it)->HasName(name) == 0)
//			return false;
//	}
//	statsf.push_back(new SingleStatistic<float>(name, stat, description));
//	return true;
//}
//
//bool
//SourceStatistics::
//Update(double time) {
//	// lock the times mutex
//	vector<SingleStatistic<uint32_t> *>::iterator it32;
//	for(it32 = stats32.begin(); it32 != stats32.end(); it32++) {
//		if(!((SingleStatistic<uint32_t> *)*it32)->Update()) return false;
//	}
//	vector<SingleStatistic<uint64_t> *>::iterator it64;
//	for(it64 = stats64.begin(); it64 != stats64.end(); it64++) {
//		if(!((SingleStatistic<uint64_t> *)*it64)->Update()) return false;
//	}
//	vector<SingleStatistic<double> *>::iterator itd;
//	for(itd = statsd.begin(); itd != statsd.end(); itd++) {
//		if(!((SingleStatistic<double> *)*itd)->Update()) return false;
//	}
//	vector<SingleStatistic<float> *>::iterator itf;
//	for(itf = statsf.begin(); itf != statsf.end(); itf++) {
//		if(!((SingleStatistic<float> *)*itf)->Update()) return false;
//	}
//	// unlock the times mutex
//	return true;
//}

} // end of namespace statistics
} // end of namespace service
} // end of namespace unisim

