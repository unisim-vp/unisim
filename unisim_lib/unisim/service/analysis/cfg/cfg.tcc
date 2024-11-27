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

#ifndef __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_TCC__
#define __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_TCC__

#include <unisim/service/analysis/cfg/cfg.hh>
#include <unisim/util/cfg/cfg.tcc>
#include <unisim/util/cfg/dominance/dominance.tcc>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <io.h>
#else
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace analysis {
namespace cfg {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename CONFIG>
Builder<CONFIG>::Builder(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "A dynamic control flow graph builder")
	, unisim::kernel::Service<unisim::service::interfaces::InstructionCollecting<ADDRESS> >(name, parent)
	, unisim::util::cfg::Builder<CONFIG>()
	, instruction_collecting_export("instruction-collecting-export", this)
	, disasm_import("disasm-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, logger(*this)
	, analysis_file()
	, graphviz_file()
	, graphviz_dir()
	, verbose(false)
	, enable_dominance_analysis(false)
	, enable_post_dominance_analysis(false)
	, enable_dominance_frontier_analysis(false)
	, enable_post_dominance_frontier_analysis(false)
	, param_analysis_file("analysis-file", this, analysis_file, "JSON file (input/output) to incrementally save the control flow graph analysis")
	, param_graphviz_file("graphviz-file", this, graphviz_file, "Graphviz .dot file (output) for rendering the control flow graph analysis")
	, param_graphviz_dir("graphviz-dir", this, graphviz_dir, "Directory (output) for Graphviz .dot files (one per CFG) for rendering the control flow graph analysis")
	, param_graphviz_color("graphviz-color", this, this->config.graphviz_color, "Default color")
	, param_graphviz_fillcolor("graphviz-fillcolor", this, this->config.graphviz_fillcolor, "Fill color for blocks and phantoms")
	, param_graphviz_branch_target_color("graphviz-branch-target-color", this, this->config.graphviz_branch_target_color, "Edge color for target branch outcome when rendering the control flow graph analysis")
	, param_graphviz_branch_fallthrough_color("graphviz-branch-fallthrough-color", this, this->config.graphviz_branch_fallthrough_color, "Edge color for fallthrough branch outcome when rendering the control flow graph analysis")
	, param_graphviz_call_color("graphviz-call-color", this, this->config.graphviz_call_color, "Edge color for calls when rendering the control flow graph analysis")
	, param_graphviz_fontname("graphviz-fontname", this, this->config.graphviz_fontname, "Font name when rendering the control flow graph analysis")
	, param_graphviz_block_fontname("graphviz-block-fontname", this, this->config.graphviz_block_fontname, "Font name for blocks when rendering the control flow graph analysis")
	, param_graphviz_phantom_fontname("graphviz-phantom-fontname", this, this->config.graphviz_phantom_fontname, "Font name for phantoms when rendering the control flow graph analysis")
	, param_graphviz_enable_call_edges("graphviz-enable-call-edges", this, this->config.graphviz_enable_call_edges, "Enable generating edges for calls when rendering the control flow graph analysis")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_enable_dominance_analysis("enable-dominance-analysis", this, enable_dominance_analysis, "Enable/Disable dominance analysis")
	, param_enable_post_dominance_analysis("enable-post-dominance-analysis", this, enable_post_dominance_analysis, "Enable/Disable post-dominance analysis")
	, param_enable_dominance_frontier_analysis("enable-dominance-frontier-analysis", this, enable_dominance_frontier_analysis, "Enable/Disable dominance frontier analysis")
	, param_enable_post_dominance_frontier_analysis("enable-post-dominance-frontier-analysis", this, enable_post_dominance_frontier_analysis, "Enable/Disable post-dominance frontier analysis")
	, write_output(false)
{
}

template <typename CONFIG>
Builder<CONFIG>::~Builder()
{
	this->End();
	
	if(enable_dominance_analysis) AnalyzeDominance<unisim::util::cfg::dominance::DOMINANCE>();
	if(enable_post_dominance_analysis) AnalyzeDominance<unisim::util::cfg::dominance::POST_DOMINANCE>();
	if(enable_dominance_frontier_analysis) AnalyzeDominanceFrontier<unisim::util::cfg::dominance::DOMINANCE>();
	if(enable_post_dominance_frontier_analysis) AnalyzeDominanceFrontier<unisim::util::cfg::dominance::POST_DOMINANCE>();
	
	if(write_output)
	{
		if(!analysis_file.empty())
		{
			std::ofstream file(analysis_file);
			if(file)
			{
				if(file << (*this))
				{
					if(verbose) logger << DebugInfo << "File \"" << analysis_file << "\" written" << EndDebugInfo;
				}
				else
				{
					logger << DebugError << "Saving into File \"" << analysis_file << "\" failed" << EndDebugError;
				}
			}
			else
			{
				logger << DebugError << "Can't open File \"" << analysis_file << "\" for writing" << EndDebugError;
			}
			
			if(!graphviz_file.empty())
			{
				std::ofstream file(graphviz_file);
				if(file)
				{
					this->OutputGraph(file);
					if(file.good())
					{
						if(verbose) logger << DebugInfo << "File \"" << graphviz_file << "\" written" << EndDebugInfo;
					}
					else
					{
						logger << DebugError << "Writting into File \"" << graphviz_file << "\" failed" << EndDebugError;
					}
				}
				else
				{
					logger << DebugError << "Can't open File \"" << graphviz_file << "\" for writing" << EndDebugError;
				}
			}
			
			if(!graphviz_dir.empty())
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				mkdir(graphviz_dir.c_str());
#else
				mkdir(graphviz_dir.c_str(), S_IRWXU);
#endif
				const unisim::util::cfg::Mapping<CONFIG>& mapping = this->GetMapping();
				
				struct CFGVisitor
				{
					unisim::kernel::logger::Logger& logger;
					const std::string& graphviz_dir;
					const unisim::util::cfg::Config<CONFIG>& config;
					bool verbose;
					
					CFGVisitor(
						unisim::kernel::logger::Logger& _logger,
						const std::string& _graphviz_dir,
						const unisim::util::cfg::Config<CONFIG>& _config,
						bool _verbose)
						: logger(_logger)
						, graphviz_dir(_graphviz_dir)
						, config(_config)
						, verbose(_verbose)
					{
					}
					
					bool Visit(unisim::util::cfg::CFG<CONFIG> *cfg)
					{
						std::string graphviz_file(graphviz_dir + '/' + cfg->GetGraphName() + ".dot");
						std::ofstream file(graphviz_file);
						if(file)
						{
							cfg->OutputGraph(file, std::string(), config);
							
							if(file.good())
							{
								if(verbose) logger << DebugInfo << "File \"" << graphviz_file << "\" written" << EndDebugInfo;
							}
							else
							{
								logger << DebugError << "Writting into File \"" << graphviz_file << "\" failed" << EndDebugError;
							}
						}
						else
						{
							logger << DebugError << "Can't open File \"" << graphviz_file << "\" for writing" << EndDebugError;
						}
						return false; // keep scanning CFGs
					}
				};
				
				CFGVisitor cfg_visitor(logger, graphviz_dir, this->config, verbose);
				mapping.ScanCFGs(cfg_visitor);
			}
		}
	}
}

template <typename CONFIG>
template <unisim::util::cfg::dominance::DominanceType DOMINANCE_TYPE>
void Builder<CONFIG>::AnalyzeDominance()
{
	if(verbose)
	{
		logger << DebugInfo << "Analyzing " << ((DOMINANCE_TYPE == unisim::util::cfg::dominance::POST_DOMINANCE) ? "dominance" : "post-dominance") << EndDebugInfo;
	}
	
	const unisim::util::cfg::Mapping<CONFIG>& mapping = this->GetMapping();
	
	struct CFGVisitor
	{
		bool Visit(unisim::util::cfg::CFG<CONFIG> *cfg)
		{
			unisim::util::cfg::dominance::BuildDominatorTree<CONFIG, DOMINANCE_TYPE>(*cfg);
			return false; // keep scanning CFGs
		}
	};
	
	CFGVisitor cfg_visitor;
	mapping.ScanCFGs(cfg_visitor);
}

template <typename CONFIG>
template <unisim::util::cfg::dominance::DominanceType DOMINANCE_TYPE>
void Builder<CONFIG>::AnalyzeDominanceFrontier()
{
	if(verbose)
	{
		logger << DebugInfo << "Analyzing " << ((DOMINANCE_TYPE == unisim::util::cfg::dominance::POST_DOMINANCE) ? "dominance" : "post-dominance") << " frontier" << EndDebugInfo;
	}

	const unisim::util::cfg::Mapping<CONFIG>& mapping = this->GetMapping();
	
	struct CFGVisitor
	{
		bool Visit(unisim::util::cfg::CFG<CONFIG> *cfg)
		{
			unisim::util::cfg::dominance::BuildDominanceFrontierSets<CONFIG, DOMINANCE_TYPE>(*cfg);
			return false; // keep scanning CFGs
		}
	};
	
	CFGVisitor cfg_visitor;
	mapping.ScanCFGs(cfg_visitor);
}

template <typename CONFIG>
bool Builder<CONFIG>::EndSetup()
{
	this->SetDisasmInterface(disasm_import);
	this->SetSymbolTableLookupInterface(symbol_table_lookup_import);
	
	if(!analysis_file.empty())
	{
		std::ifstream file(analysis_file);
    if(file)
		{
			unisim::util::cfg::dominance::DominanceExtensionFactory<CONFIG> dom_tree_extension_factory;
			this->Load(file, { &dom_tree_extension_factory });
			if(file)
			{
				if(verbose) logger << DebugInfo << "File \"" << analysis_file << "\" loaded" << EndDebugInfo;
			}
			else
			{
				logger << DebugError << "Loading from File \"" << analysis_file << "\" failed" << EndDebugError;
				return false;
			}
		}
	}
	
	if(verbose)
	{
		logger << DebugInfo << "Min opcode size: " << CONFIG::MIN_OPCODE_SIZE << EndDebugInfo;
		logger << DebugInfo << "Max opcode size: " << CONFIG::MAX_OPCODE_SIZE << EndDebugInfo;
		if(CONFIG::CACHE_SIZE > 0)
		{
			logger << DebugInfo << "Node cache size: " << CONFIG::CACHE_SIZE << EndDebugInfo;
		}
		
		logger << DebugInfo << "Debug: " << (CONFIG::DEBUG ? "enabled" : "disabled") << EndDebugInfo;
		logger << DebugInfo << "Runtime checking: " << (CONFIG::CHECK ? "enabled" : "disabled") << EndDebugInfo;
	}
	
	return write_output = true;
}

// unisim::service::interfaces::BasicBlockCollecting<ADDRESS>
template <typename CONFIG>
void Builder<CONFIG>::CollectInstruction(const unisim::service::interfaces::InstructionInfo<ADDRESS>& instr_info)
{
	this->Process(instr_info);
}

} // end of namespace cfg
} // end of namespace analysis
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_ANALYSIS_CFG_CFG_TCC__
