/*
 *  Copyright (c) 2008,
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
 * Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr) 
 */

#include <systemc.h>
#include "bus_test.hh"
#include "unisim/kernel/service/service.hh"

using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;
using unisim::kernel::logger::LoggerServer;

int sc_main(int argv, char **argc) {
	Top<true> top("top");


	VariableBase *var = ServiceManager::GetParameter("top.bus.bus_cycle_time");
    *var = 10.0;
	var = ServiceManager::GetParameter("top.bus.verbose_all");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_out");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_out_color");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_err");
	*var = false;
	var = ServiceManager::GetParameter("kernel_logger.std_err_color");
	*var = false;
	var = ServiceManager::GetParameter("kernel_logger.xml_file");
	*var = true;
	if(!ServiceManager::Setup()) {
		cerr << "Setup error" << endl;
		return 0;
	}

	sc_start();

	cerr << "Bye, bye" << endl;

}
