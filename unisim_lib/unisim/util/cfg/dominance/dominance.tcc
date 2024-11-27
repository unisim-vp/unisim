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

#ifndef __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_TCC__
#define __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_TCC__

#include <unisim/util/cfg/dominance/dominance.hh>
#include <unisim/util/cfg/cfg.tcc>
#include <unisim/util/ostream/ostream.hh>

namespace unisim {
namespace util {
namespace cfg {
namespace dominance {

using unisim::util::ostream::FScope;

///////////////////////////////////////////////////////////////////////////////
//                               Definitions                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////// NodeDominatorTreeOrderExtension<> ///////////////////////

template <typename CONFIG>
const char *NodeDominatorTreeOrderExtension<CONFIG>::NAME = "dom_tree_orders";

template <typename CONFIG>
JSON_Value *NodeDominatorTreeOrderExtension<CONFIG>::Save() const
{
	JSON_Object *object = new JSON_Object();
	if(order[DOMINANCE]) object->Add(new JSON_Member("dom_tree_order", new JSON_Integer(order[DOMINANCE])));
	if(order[POST_DOMINANCE]) object->Add(new JSON_Member("pdom_tree_order", new JSON_Integer(order[POST_DOMINANCE])));
	return object;
}

template <typename CONFIG>
NodeDominatorTreeOrderExtension<CONFIG> *NodeDominatorTreeOrderExtension<CONFIG>::Load(const JSON_Value& value)
{
	const JSON_Object& object = value.AsObject();
	unsigned dom_tree_order = object.HasProperty("dom_tree_order") ? unsigned(object["dom_tree_order"].AsInteger()) : 0;
	unsigned pdom_tree_order = object.HasProperty("pdom_tree_order") ? unsigned(object["pdom_tree_order"].AsInteger()) : 0;
	return new NodeDominatorTreeOrderExtension<CONFIG>(dom_tree_order, pdom_tree_order);
}

template <typename CONFIG>
void NodeDominatorTreeOrderExtension<CONFIG>::Print(std::ostream& stream) const
{
	FScope<std::ostream> fscope(stream);
	if(order[DOMINANCE]) stream << "dom_tree_order: " << std::dec << order[DOMINANCE];
	if(order[POST_DOMINANCE]) stream << (order[DOMINANCE] ? ", ": "") << "pdom_tree_order: " << std::dec << order[POST_DOMINANCE];
}

///////////////////////////// NodeOrderAccessor<> /////////////////////////////

template <typename CONFIG>
NodeDominatorTreeOrderExtension<CONFIG> *NodeOrderAccessor<CONFIG>::GetExtension(Node<CONFIG> *node)
{
	NodeDominatorTreeOrderExtension<CONFIG> *node_ext = node->template GetExtension<NodeDominatorTreeOrderExtension<CONFIG> >();
	if(!node_ext)
	{
		node_ext = new NodeDominatorTreeOrderExtension<CONFIG>();
		node->SetExtension(node_ext);
	}
	return node_ext;
}

template <typename CONFIG>
unsigned NodeOrderAccessor<CONFIG>::Get(Node<CONFIG> *node, DominanceType dom_type)
{
	return GetExtension(node)->Order(dom_type);
}

template <typename CONFIG>
void NodeOrderAccessor<CONFIG>::Set(Node<CONFIG> *node, DominanceType dom_type, unsigned order)
{
	GetExtension(node)->Order(dom_type, order);
}

////////////////////////// NodeDominanceExtension<> ///////////////////////////

template <typename CONFIG>
const char *NodeDominanceExtension<CONFIG>::NAME = "dominances";

template <typename CONFIG>
JSON_Value *NodeDominanceExtension<CONFIG>::Save() const
{
	JSON_Object *object = new JSON_Object();
	if(dom[DOMINANCE]) object->Add(new JSON_Member("idom", new JSON_Integer(dom[DOMINANCE])));
	if(dom[POST_DOMINANCE]) object->Add(new JSON_Member("ipdom", new JSON_Integer(dom[POST_DOMINANCE])));
	return object;
}

template <typename CONFIG>
NodeDominanceExtension<CONFIG> *NodeDominanceExtension<CONFIG>::Load(const JSON_Value& value)
{
	const JSON_Object& object = value.AsObject();
	unsigned idom = object.HasProperty("idom") ? unsigned(object["idom"].AsInteger()) : 0;
	unsigned ipdom = object.HasProperty("ipdom") ? unsigned(object["ipdom"].AsInteger()) : 0;
	return new NodeDominanceExtension<CONFIG>(idom, ipdom);
}

template <typename CONFIG>
void NodeDominanceExtension<CONFIG>::Print(std::ostream& stream) const
{
	FScope<std::ostream> fscope(stream);
	if(dom[DOMINANCE]) stream << "idom: " << std::dec << dom[DOMINANCE];
	if(dom[POST_DOMINANCE]) stream << (dom[DOMINANCE] ? ", " : "") << "ipdom: " << std::dec << dom[POST_DOMINANCE];
}

/////////////////////////// NodeDominanceAccessor<> ///////////////////////////

template <typename CONFIG>
NodeDominanceExtension<CONFIG> *NodeDominanceAccessor<CONFIG>::GetExtension(Node<CONFIG> *node)
{
	NodeDominanceExtension<CONFIG> *node_ext = node->template GetExtension<NodeDominanceExtension<CONFIG> >();
	if(!node_ext)
	{
		node_ext = new NodeDominanceExtension<CONFIG>();
		node->SetExtension(node_ext);
	}
	return node_ext;
}

template <typename CONFIG>
unsigned NodeDominanceAccessor<CONFIG>::Get(Node<CONFIG> *node, DominanceType dom_type)
{
	return GetExtension(node)->Dom(dom_type);
}

template <typename CONFIG>
void NodeDominanceAccessor<CONFIG>::Set(Node<CONFIG> *node, DominanceType dom_type, unsigned dom)
{
	GetExtension(node)->Dom(dom_type, dom);
}

///////////////////// NodeDominanceFrontierSetExtension<> /////////////////////

template <typename CONFIG>
const char *NodeDominanceFrontierSetExtension<CONFIG>::NAME = "dom_frontier_sets";

template <typename CONFIG>
JSON_Value *NodeDominanceFrontierSetExtension<CONFIG>::Save() const
{
	JSON_Object *object = new JSON_Object();
	if(dirty[DOMINANCE]) object->Add(new JSON_Member("dom_frontier_set", dom_frontier_set.Save()));
	if(dirty[POST_DOMINANCE]) object->Add(new JSON_Member("pdom_frontier_set", pdom_frontier_set.Save()));
	return object;
}

template <typename CONFIG>
NodeDominanceFrontierSetExtension<CONFIG> *NodeDominanceFrontierSetExtension<CONFIG>::Load(const JSON_Value& value)
{
	const JSON_Object& object = value.AsObject();
	NodeDominanceFrontierSetExtension<CONFIG> *node_ext = new NodeDominanceFrontierSetExtension<CONFIG>();
	if(object.HasProperty("dom_frontier_set"))
	{
		node_ext->dom_frontier_set.Load(object["dom_frontier_set"].AsValue());
		node_ext->dirty[DOMINANCE] = true;
	}
	if(object.HasProperty("pdom_frontier_set"))
	{
		node_ext->pdom_frontier_set.Load(object["pdom_frontier_set"].AsValue());
		node_ext->dirty[POST_DOMINANCE] = true;
	}
	return node_ext;
}

template <typename CONFIG>
void NodeDominanceFrontierSetExtension<CONFIG>::Print(std::ostream& stream) const
{
	if(dirty[DOMINANCE]) stream << "dom_frontier_set: " << dom_frontier_set;
	if(dirty[POST_DOMINANCE]) stream << (dirty[DOMINANCE] ? ", " : "") << "pdom_frontier_set: " << pdom_frontier_set;
}

///////////////////// NodeDominanceFrontierSetAccessor<> //////////////////////

template <typename CONFIG>
NodeDominanceFrontierSetExtension<CONFIG> *NodeDominanceFrontierSetAccessor<CONFIG>::GetExtension(Node<CONFIG> *node)
{
	NodeDominanceFrontierSetExtension<CONFIG> *node_ext = node->template GetExtension<NodeDominanceFrontierSetExtension<CONFIG> >();
	if(!node_ext)
	{
		node_ext = new NodeDominanceFrontierSetExtension<CONFIG>();
		node->SetExtension(node_ext);
	}
	return node_ext;
}

template <typename CONFIG>
void NodeDominanceFrontierSetAccessor<CONFIG>::Insert(Node<CONFIG> *node, DominanceType dom_type, unsigned n)
{
	return GetExtension(node)->Insert(dom_type, n);
}

template <typename CONFIG>
void NodeDominanceFrontierSetAccessor<CONFIG>::Clear(Node<CONFIG> *node, DominanceType dom_type)
{
	return GetExtension(node)->Clear(dom_type);
}

//////////////////////// DominatorTreeCFGExtension<> //////////////////////////

template <typename CONFIG>
const char *DominatorTreeCFGExtension<CONFIG>::NAME = "dom_trees";

template <typename CONFIG>
JSON_Value *DominatorTreeCFGExtension<CONFIG>::Save() const
{
	JSON_Object *object = new JSON_Object();
	if(!dom_tree.Empty()) object->Add(new JSON_Member("dom_tree", dom_tree.Save()));
	if(!pdom_tree.Empty()) object->Add(new JSON_Member("pdom_tree", pdom_tree.Save()));
	return object;
}

template <typename CONFIG>
DominatorTreeCFGExtension<CONFIG> *DominatorTreeCFGExtension<CONFIG>::Load(const JSON_Value& value)
{
	const JSON_Object& object = value.AsObject();
	DominatorTreeCFGExtension<CONFIG> *cfg_ext = new DominatorTreeCFGExtension<CONFIG>();
	if(object.HasProperty("dom_tree")) cfg_ext->dom_tree.Load(object["dom_tree"].AsValue());
	if(object.HasProperty("pdom_tree")) cfg_ext->pdom_tree.Load(object["pdom_tree"].AsValue());
	return cfg_ext;
}

template <typename CONFIG>
void DominatorTreeCFGExtension<CONFIG>::Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const JSON_Value& value)
{
	struct NodeVisitor
	{
		DominatorTree<CONFIG>& dom_tree;
		
		NodeVisitor(DominatorTree<CONFIG>& _dom_tree) : dom_tree(_dom_tree) {}
		
		bool Visit(Node<CONFIG> *node)
		{
			unsigned order = NodeOrderAccessor<CONFIG>::Get(node, dom_tree.type);
			if(order)
			{
				if(order >= dom_tree.nodes.size()) dom_tree.nodes.resize(order + 1);
				dom_tree.nodes[order] = node;
			}
			return false;
		}
	};
	
	dom_tree.nodes.clear();
	NodeVisitor dom_tree_node_visitor(dom_tree);
	cfg.ScanNodes(dom_tree_node_visitor);
	assert(dom_tree.doms.size() == dom_tree.nodes.size());
	pdom_tree.nodes.clear();
	NodeVisitor pdom_tree_node_visitor(pdom_tree);
	cfg.ScanNodes(pdom_tree_node_visitor);
	assert(pdom_tree.doms.size() == pdom_tree.nodes.size());
}

template <typename CONFIG>
void DominatorTreeCFGExtension<CONFIG>::Print(std::ostream& stream) const
{
	if(!dom_tree.Empty()) stream << "dom_tree: " << dom_tree;
	if(!pdom_tree.Empty()) stream << (dom_tree.Empty() ? "" : ", ") << "pdom_tree: " << pdom_tree;
}

//////////////////////// DominanceExtensionFactory<> //////////////////////////

template <typename CONFIG>
ExtensionBase *DominanceExtensionFactory<CONFIG>::LoadExtension(ExtensionType ext_type, const std::string& ext_name, const JSON_Value& value) const
{
	if(ext_type == NODE_EXT)
	{
		if(ext_name == NodeDominatorTreeOrderExtension<CONFIG>::NAME)
		{
			return NodeDominatorTreeOrderExtension<CONFIG>::Load(value);
		}
		else if(ext_name == NodeDominanceExtension<CONFIG>::NAME)
		{
			return NodeDominanceExtension<CONFIG>::Load(value);
		}
		else if(ext_name == NodeDominanceFrontierSetExtension<CONFIG>::NAME)
		{
			return NodeDominanceFrontierSetExtension<CONFIG>::Load(value);
		}
	}
	else if(ext_type == CFG_EXT)
	{
		if(ext_name == DominatorTreeCFGExtension<CONFIG>::NAME)
		{
			return DominatorTreeCFGExtension<CONFIG>::Load(value);
		}
	}
	return 0;
}

//////////////////////////// BuildDominatorTree<> /////////////////////////////

template <typename CONFIG, DominanceType DOMINANCE_TYPE>
void BuildDominatorTree(CFG<CONFIG>& cfg)
{
	DominatorTreeCFGExtension<CONFIG> *dom_tree_cfg_ext = cfg.template GetExtension<DominatorTreeCFGExtension<CONFIG> >();
	if(!dom_tree_cfg_ext)
	{
		dom_tree_cfg_ext = new DominatorTreeCFGExtension<CONFIG>();
		cfg.SetExtension(dom_tree_cfg_ext);
	}
	
	typedef DominatorTree<CONFIG> DomTree;
	DomTree& dom_tree = dom_tree_cfg_ext->GetDominatorTree(DOMINANCE_TYPE);
	dom_tree.nodes.clear();
	dom_tree.doms.clear();
	
	struct NodeSorter
	{
		const CFG<CONFIG>& cfg;
		DomTree& dom_tree;
		unsigned order; // last node number
		std::set<Node<CONFIG> *> visited;
		unsigned pass;
		
		NodeSorter(const CFG<CONFIG>& _cfg, DomTree& _dom_tree, Node<CONFIG> *start_node) : cfg(_cfg), dom_tree(_dom_tree), order(0), visited(), pass(0)
		{
			// pass 0: reset order of all CFG nodes
			cfg.ScanNodes(*this);
			// pass 1: assign order for reachable nodes from start_node
			pass = 1;
			dom_tree.nodes.push_back(0); // node at position 0 is undefined because 0 in doms array means undefined
			// Depth first search starting with start node
			Visit(start_node);
			assert(dom_tree.nodes.size() == (order + 1));
			if(CONFIG::CHECK)
			{
				for(typename DomTree::Tree::size_type idx = 1, len = dom_tree.nodes.size(); idx < len; ++idx)
				{
					assert(NodeOrderAccessor<CONFIG>::Get(dom_tree.nodes[idx], DOMINANCE_TYPE) == idx);
				}
			}
		}
		
		bool Visit(Node<CONFIG> *node)
		{
			if(pass == 0)
			{
				// reset order
				NodeOrderAccessor<CONFIG>::Set(node, DOMINANCE_TYPE, 0);
				NodeDominanceAccessor<CONFIG>::Set(node, DOMINANCE_TYPE, 0);
			}
			else
			{
				// assign order
				if(!visited.count(node))
				{
					visited.insert(node);
					// Depth first search
					if(DOMINANCE_TYPE == POST_DOMINANCE)
					{
						if((node->GetCategory() == HALT) && cfg.HasExit())
						{
							// hack: for post-dominator tree, if exit node exists, halt node is implicitely a successor of exit node
							Visit(cfg.GetExit());
						}
						node->ScanPredecessors(*this);
					}
					else // if(DOMINANCE_TYPE == DOMINANCE)
					{
						node->ScanSuccessors(*this);
					}
					
					// Number the node
					if(CONFIG::CHECK) assert(NodeOrderAccessor<CONFIG>::Get(node, DOMINANCE_TYPE) == 0);
					NodeOrderAccessor<CONFIG>::Set(node, DOMINANCE_TYPE, ++order);
					
					// Keep nodes sorted by postorder
					dom_tree.nodes.push_back(node);
				}
			}
			return false; // keep scanning successors or predecessors or nodes
		}
	};
	
	// Sort nodes by postorder
	
	// define the (unique) start_node:
	//   - for a dominator tree, it is the entry node.
	//   - for a post-dominator tree, it is the halt node when it exists, otherwise it is the exit node.
	//     when both halt and exit nodes exist, the halt node is implicitely a successor of exit node.
	Node<CONFIG> *start_node = (DOMINANCE_TYPE == POST_DOMINANCE) ? (cfg.HasHalt() ? cfg.GetHalt()
	                                                                               : (cfg.HasExit() ? cfg.GetExit()
	                                                                                                : 0))
	                                                              : (cfg.HasEntry() ? cfg.GetEntry()
	                                                                                : 0);
	assert(start_node != 0);
	NodeSorter node_sorter(cfg, dom_tree, start_node);
	
	// Compute dominator (or post-dominator) tree
	
	// for all nodes, b: doms[b] <- undefined
	dom_tree.doms.resize(dom_tree.nodes.size());
	
	// doms[start_node] <- start_node
	NodeDominanceAccessor<CONFIG>::Set(start_node, DOMINANCE_TYPE, dom_tree.doms[node_sorter.order] = node_sorter.order);
	
	bool changed = true;
	while(changed)
	{
		changed = false;
		
		// for all nodes, b, in reverse postorder (except start_node, i.e. order >= 1)
		for(unsigned b = node_sorter.order - 1; b >= 1; --b)
		{
			// new_idom <- undefined
			unsigned new_idom = 0;
			
			struct NodeVisitor
			{
				DomTree& dom_tree;
				unsigned& new_idom;
				
				NodeVisitor(DomTree& _dom_tree, unsigned& _new_idom) : dom_tree(_dom_tree), new_idom(_new_idom) {}
				
				unsigned Intersect(unsigned finger1, unsigned finger2) const
				{
					while(finger1 != finger2)
					{
						while(finger1 < finger2)
						{
							finger1 = dom_tree.doms[finger1];
						}
						while(finger2 < finger1)
						{
							finger2 = dom_tree.doms[finger2];
						}
					}
					return finger1;
				}
				
				bool Visit(Node<CONFIG> *node)
				{
					unsigned p = NodeOrderAccessor<CONFIG>::Get(node, DOMINANCE_TYPE);
					// if node is reachable from start_node, i.e., order is defined (!= 0)
					if(p != 0)
					{
						// if doms[p] != undefined, i.e., if doms[p] already calculated
						if(dom_tree.doms[p] != 0)
						{
							// if new_idom != undefined
							if(new_idom != 0)
							{
								// other predecessors (or successors), p, of b
								new_idom = Intersect(p, new_idom);
							}
							else
							{
								// first (processed) predecessor (or successor) of b (pick one)
								new_idom = p;
							}
						}
					}
						
					return false; // keep scanning successors (or predecessors)
				}
			};
			
			Node<CONFIG> *node = dom_tree.nodes[b];
			NodeVisitor node_visitor(dom_tree, new_idom);
			
			// Scan either successors (for post-dominator tree) or predecessors (for dominator tree)
			if(DOMINANCE_TYPE == POST_DOMINANCE)
			{
				if((node->GetCategory() == EXIT) && cfg.HasHalt())
				{
					// hack: for post-dominator tree, if halt node exists, it is implicitely a successor of exit node
					node_visitor.Visit(cfg.GetHalt());
				}
				node->ScanSuccessors(node_visitor);
			}
			else // if(DOMINANCE_TYPE == DOMINANCE)
				node->ScanPredecessors(node_visitor);
			
			if(dom_tree.doms[b] != new_idom)
			{
				NodeDominanceAccessor<CONFIG>::Set(node, DOMINANCE_TYPE, dom_tree.doms[b] = new_idom);
				changed = true;
			}
		}
	}
}

///////////////////////// BuildDominanceFrontierSet<> /////////////////////////

template <typename CONFIG, DominanceType DOMINANCE_TYPE>
void BuildDominanceFrontierSets(CFG<CONFIG>& cfg)
{
	DominatorTreeCFGExtension<CONFIG> *dom_tree_cfg_ext = cfg.template GetExtension<DominatorTreeCFGExtension<CONFIG> >();
	if(!dom_tree_cfg_ext)
	{
		BuildDominatorTree<CONFIG, DOMINANCE_TYPE>(cfg);
		dom_tree_cfg_ext = cfg.template GetExtension<DominatorTreeCFGExtension<CONFIG> >();
	}
	assert(dom_tree_cfg_ext != 0);
	
	typedef DominatorTree<CONFIG> DomTree;
	const DomTree& dom_tree = dom_tree_cfg_ext->GetDominatorTree(DOMINANCE_TYPE);
	
	struct NodeVisitor
	{
		const CFG<CONFIG>& cfg;
		const DomTree& dom_tree;
		unsigned pass;
		
		NodeVisitor(const CFG<CONFIG>& _cfg, const DomTree& _dom_tree) : cfg(_cfg), dom_tree(_dom_tree), pass(0)
		{
			cfg.ScanNodes(*this);
			pass = 1;
		}
		
		bool Visit(Node<CONFIG> *node)
		{
			if(pass == 0)
			{
				// clear dominance frontier set
				NodeDominanceFrontierSetAccessor<CONFIG>::Clear(node, DOMINANCE_TYPE);
			}
			else
			{
				// compute dominance frontier set
				unsigned b = NodeOrderAccessor<CONFIG>::Get(node, DOMINANCE_TYPE);
				
				// if reachable from start_node
				if(b)
				{
					struct NeighbourNodeVisitor
					{
						const DomTree& dom_tree;
						unsigned b;
						
						NeighbourNodeVisitor(const DomTree& _dom_tree, unsigned _b) : dom_tree(_dom_tree), b(_b) {}
						
						bool Visit(Node<CONFIG> *node)
						{
							unsigned p = NodeOrderAccessor<CONFIG>::Get(node, DOMINANCE_TYPE);
							
							// if reachable from start_node
							if(p)
							{
								// runner <- p
								unsigned runner = p;
								
								// while runner != doms[b]
								while(runner != dom_tree.doms[b])
								{
									// add b to runner's dominance frontier set
									NodeDominanceFrontierSetAccessor<CONFIG>::Insert(dom_tree.nodes[runner], DOMINANCE_TYPE, b);
									
									// runner = doms[runner]
									if(dom_tree.doms[runner] == 0)
									{
										std::cerr << *dom_tree.nodes[runner] << std::endl;
									}
									runner = dom_tree.doms[runner];
									assert(runner != 0);
								}
							}
							return false; // keep scanning predecessors (or successors)
						}
					};
					
					NeighbourNodeVisitor neighbour_node_visitor(dom_tree, b);
					
					// if the number of predecessors (or successors) of b >= 2
					// for all predecessors (or successors), p, of b
					if(DOMINANCE_TYPE == POST_DOMINANCE)
					{
						unsigned successor_count = node->SuccessorCount();
						if((node->GetCategory() == EXIT) && cfg.HasHalt())
						{
							// hack: for post-dominator tree, if halt node exists, it is implicitely a successor of exit node
							++successor_count;
							if(successor_count >= 2)
							{
								neighbour_node_visitor.Visit(cfg.GetHalt());
							}
						}
						if(successor_count >= 2)
						{
							node->ScanSuccessors(neighbour_node_visitor);
						}
					}
					else // if(DOMINANCE_TYPE == DOMINANCE)
					{
						if(node->PredecessorCount() >= 2) node->ScanPredecessors(neighbour_node_visitor);
					}
				}
			}
			
			return false; // keep scanning nodes of CFG
		}
	};
	
	// for all nodes, b
	NodeVisitor node_visitor(cfg, dom_tree);
	cfg.ScanNodes(node_visitor);
}

} // end of namespace dominance
} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_TCC__

