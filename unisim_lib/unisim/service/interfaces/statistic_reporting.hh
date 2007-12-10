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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_INTERFACES_STATISTIC_REPORTING_HH__
#define __UNISIM_SERVICE_INTERFACES_STATISTIC_REPORTING_HH__

#include <inttypes.h>

namespace unisim {
namespace service {
namespace interfaces {

class StatisticReporting {
public:
	typedef uint32_t stat_handle_t;
	virtual stat_handle_t RegisterStat(const char *name) = 0;
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			uint64_t value) = 0;
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			int64_t value) = 0;
	virtual void ReportStat(stat_handle_t handle,
			uint64_t time,
			double value) = 0;
};

class StatisticReportingControl {
public:
	virtual void SetPreferredStatReportingPeriod(uint64_t time_hint) = 0;
	virtual void RequiresStatReporting(const char *name, bool required) = 0;
};

//class Statistics {
//public:
//	virtual bool AddSource(const char *name, uint32_t &id) = 0;
//	virtual bool AddStatistic(uint32_t source_id, const char *name, uint32_t *stat, const char *description = 0) = 0;
//	virtual bool AddStatistic(uint32_t source_id, const char *name, uint64_t *stat, const char *description = 0) = 0;
//	virtual bool AddStatistic(uint32_t source_id, const char *name, double *stat, const char *description = 0) = 0;
//	virtual bool AddStatistic(uint32_t source_id, const char *name, float *stat, const char *description = 0) = 0;
//	virtual bool Update(uint32_t id, double time) = 0; // time in seconds
//};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNSIIM_SERVICE_INTERFACES_STATISTIC_REPORTING_HH__
