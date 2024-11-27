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

#ifndef __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_HH__
#define __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/instruction_collecting.hh>
#include <unisim/util/cfg/cfg.hh>
#include <unisim/util/cfg/dominance/dominance.hh>
#include <string>

namespace unisim {
namespace service {
namespace analysis {
namespace cfg {

template <typename CONFIG>
struct Builder
	: unisim::kernel::Service<unisim::service::interfaces::InstructionCollecting<typename CONFIG::ADDRESS> >
	, unisim::util::cfg::Builder<CONFIG>
{
	typedef typename CONFIG::ADDRESS ADDRESS;
	
	// Exports
	unisim::kernel::ServiceExport<unisim::service::interfaces::InstructionCollecting<ADDRESS> > instruction_collecting_export;
	
	// Imports
	unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> > disasm_import;
	unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;
	
	Builder(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Builder();
	
	virtual bool EndSetup();
	
	// unisim::service::interfaces::InstructionCollecting<ADDRESS>
	virtual void CollectInstruction(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info);
private:
	unisim::kernel::logger::Logger logger;
	
	std::string analysis_file;
	std::string graphviz_file;
	std::string graphviz_dir;
	bool verbose;
	bool enable_dominance_analysis;
	bool enable_post_dominance_analysis;
	bool enable_dominance_frontier_analysis;
	bool enable_post_dominance_frontier_analysis;
	
	unisim::kernel::variable::Parameter<std::string> param_analysis_file;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_file;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_dir;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_color;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_fillcolor;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_branch_target_color;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_branch_fallthrough_color;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_call_color;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_fontname;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_block_fontname;
	unisim::kernel::variable::Parameter<std::string> param_graphviz_phantom_fontname;
	unisim::kernel::variable::Parameter<bool>        param_graphviz_enable_call_edges;
	unisim::kernel::variable::Parameter<bool>        param_verbose;
	unisim::kernel::variable::Parameter<bool>        param_enable_dominance_analysis;
	unisim::kernel::variable::Parameter<bool>        param_enable_post_dominance_analysis;
	unisim::kernel::variable::Parameter<bool>        param_enable_dominance_frontier_analysis;
	unisim::kernel::variable::Parameter<bool>        param_enable_post_dominance_frontier_analysis;
	
	bool write_output;

	template <unisim::util::cfg::dominance::DominanceType DOMINANCE_TYPE>
	void AnalyzeDominance();
	
	template <unisim::util::cfg::dominance::DominanceType DOMINANCE_TYPE>
	void AnalyzeDominanceFrontier();
};

} // end of namespace cfg
} // end of namespace analysis
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_HH__
