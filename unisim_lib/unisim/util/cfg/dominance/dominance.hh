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

// This is an implementation of algorithms from the following paper:
//
//   Cooper, Keith & Harvey, Timothy & Kennedy, Ken. (2006).
//   A Simple, Fast Dominance Algorithm.
//   Rice University, CS Technical Report 06-33870.

#ifndef __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_HH__
#define __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_HH__

#include <unisim/util/cfg/cfg.hh>

namespace unisim {
namespace util {
namespace cfg {
namespace dominance {

///////////////////////////////////////////////////////////////////////////////
//                            Enumeration types                              //
///////////////////////////////////////////////////////////////////////////////

// Dominance type
enum DominanceType
{
	DOMINANCE      = 0, // dominance
	POST_DOMINANCE = 1  // post-dominance
};

///////////////////////////////////////////////////////////////////////////////
//                          Forward declarations                             //
///////////////////////////////////////////////////////////////////////////////

struct DominatorTreeBase;
template <typename CONFIG> struct DominatorTree;
template <typename CONFIG> struct NodeDominatorTreeOrderExtension;
template <typename CONFIG> struct NodeOrderAccessor;
template <typename CONFIG> struct DominatorTreeCFGExtension;
template <typename CONFIG> struct DominanceExtensionFactory;

using unisim::util::json::JSON_Value;
using unisim::util::json::JSON_String;
using unisim::util::json::JSON_Number;
using unisim::util::json::JSON_Integer;
using unisim::util::json::JSON_Float;
using unisim::util::json::JSON_Boolean;
using unisim::util::json::JSON_Null;
using unisim::util::json::JSON_Object;
using unisim::util::json::JSON_Member;
using unisim::util::json::JSON_Array;

///////////////////////////////////////////////////////////////////////////////
//                               Declarations                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// DominatorTreeBase ///////////////////////////////

struct DominatorTreeBase
{
	DominanceType type;
	typedef std::vector<unsigned> Tree;
	Tree doms;   // doms[n] == IDom(n)
	             // Note: position 0 is unused
	
	DominatorTreeBase(DominanceType _type) : type(_type), doms() {}
	bool Empty() const { return doms.empty(); }
	JSON_Value *Save() const;
	void Load(const JSON_Value& value);
	void Print(std::ostream& stream) const;
};

inline std::ostream& operator << (std::ostream& stream, const DominatorTreeBase& dom_tree) { dom_tree.Print(stream); return stream; }

/////////////////////////////// DominatorTree<> ///////////////////////////////

template <typename CONFIG>
struct DominatorTree : DominatorTreeBase
{
	typedef std::vector<Node<CONFIG> *> Nodes;
	Nodes nodes; // Nodes sorted by postorder
	             // Note: position 0 is undefined
	
	DominatorTree(DominanceType _type) : DominatorTreeBase(_type), nodes() {}
};

//////////////////////////// DominanceFrontierSet /////////////////////////////

struct DominanceFrontierSet
{
	DominanceType type;
	typedef std::set<unsigned> Set;
	Set set; 
	
	DominanceFrontierSet(DominanceType _type) : type(_type), set() {}
	JSON_Value *Save() const;
	void Load(const JSON_Value& value);
	void Insert(unsigned n) { set.insert(n); }
	void Clear() { set.clear(); }
	bool Empty() const { return set.empty(); }
	void Print(std::ostream& stream) const;
};

inline std::ostream& operator << (std::ostream& stream, const DominanceFrontierSet& dom_frontier_set) { dom_frontier_set.Print(stream); return stream; }

///////////////////// NodeDominatorTreeOrderExtension<> ///////////////////////

template <typename CONFIG>
struct NodeDominatorTreeOrderExtension : NodeExtension<CONFIG, NodeDominatorTreeOrderExtension<CONFIG> >
{
	static const char *NAME;
	NodeDominatorTreeOrderExtension() : order() {}
	NodeDominatorTreeOrderExtension(unsigned dom_tree_order, unsigned pdom_tree_order) : order()
	{
		order[DOMINANCE] = dom_tree_order;
		order[POST_DOMINANCE] = pdom_tree_order;
	}
	
	virtual JSON_Value *Save() const;
	static NodeDominatorTreeOrderExtension<CONFIG> *Load(const JSON_Value& value);
	
	void Order(DominanceType dom_type, unsigned _order) { order[dom_type] = _order; }
	unsigned Order(DominanceType dom_type) const { return order[dom_type]; }
	
	virtual void Print(std::ostream& stream) const;
	
private:
	unsigned order[2];
};

///////////////////////////// NodeOrderAccessor<> /////////////////////////////

template <typename CONFIG>
struct NodeOrderAccessor
{
	static unsigned Get(Node<CONFIG> *node, DominanceType dom_type);
	static void Set(Node<CONFIG> *node, DominanceType dom_type, unsigned order);
private:
	static NodeDominatorTreeOrderExtension<CONFIG> *GetExtension(Node<CONFIG> *node);
};

////////////////////////// NodeDominanceExtension<> ///////////////////////////

template <typename CONFIG>
struct NodeDominanceExtension : NodeExtension<CONFIG, NodeDominanceExtension<CONFIG> >
{
	static const char *NAME;
	NodeDominanceExtension() : dom() {}
	NodeDominanceExtension(unsigned idom, unsigned ipdom) : dom()
	{
		dom[DOMINANCE] = idom;
		dom[POST_DOMINANCE] = ipdom;
	}
	
	virtual JSON_Value *Save() const;
	static NodeDominanceExtension<CONFIG> *Load(const JSON_Value& value);
	
	void Dom(DominanceType dom_type, unsigned _dom) { dom[dom_type] = _dom; }
	unsigned Dom(DominanceType dom_type) const { return dom[dom_type]; }
	
	virtual void Print(std::ostream& stream) const;
	
private:
	unsigned dom[2];
};

/////////////////////////// NodeDominanceAccessor<> ///////////////////////////

template <typename CONFIG>
struct NodeDominanceAccessor
{
	static unsigned Get(Node<CONFIG> *node, DominanceType dom_type);
	static void Set(Node<CONFIG> *node, DominanceType dom_type, unsigned dom);
private:
	static NodeDominanceExtension<CONFIG> *GetExtension(Node<CONFIG> *node);
};

///////////////////// NodeDominanceFrontierSetExtension<> /////////////////////

template <typename CONFIG>
struct NodeDominanceFrontierSetExtension : NodeExtension<CONFIG, NodeDominanceFrontierSetExtension<CONFIG> >
{
	static const char *NAME;
	
	NodeDominanceFrontierSetExtension() : dirty(), dom_frontier_set(DOMINANCE), pdom_frontier_set(POST_DOMINANCE) {}
	
	virtual JSON_Value *Save() const;
	static NodeDominanceFrontierSetExtension<CONFIG> *Load(const JSON_Value& value);
	
	void Insert(DominanceType dom_type, unsigned n) { (dom_type == POST_DOMINANCE) ? pdom_frontier_set.Insert(n) : dom_frontier_set.Insert(n); dirty[dom_type] = true; }
	void Clear(DominanceType dom_type) { (dom_type == POST_DOMINANCE) ? pdom_frontier_set.Clear() : dom_frontier_set.Clear();  dirty[dom_type] = true; }
	const DominanceFrontierSet& GetDominanceFrontierSet(DominanceType dom_type) const { return (dom_type == POST_DOMINANCE) ? pdom_frontier_set : dom_frontier_set; }
	
	virtual void Print(std::ostream& stream) const;
	
private:
	bool dirty[2];
	DominanceFrontierSet dom_frontier_set;
	DominanceFrontierSet pdom_frontier_set;
};

///////////////////// NodeDominanceFrontierSetAccessor<> //////////////////////

template <typename CONFIG>
struct NodeDominanceFrontierSetAccessor
{
	static void Insert(Node<CONFIG> *node, DominanceType dom_type, unsigned n);
	static void Clear(Node<CONFIG> *node, DominanceType dom_type);
private:
	static NodeDominanceFrontierSetExtension<CONFIG> *GetExtension(Node<CONFIG> *node);
};

//////////////////////// DominatorTreeCFGExtension<> //////////////////////////

template <typename CONFIG>
struct DominatorTreeCFGExtension : CFGExtension<CONFIG, DominatorTreeCFGExtension<CONFIG> >
{
	static const char *NAME;
	DominatorTreeCFGExtension() : dom_tree(DOMINANCE), pdom_tree(POST_DOMINANCE) {}
	DominatorTree<CONFIG>& GetDominatorTree(DominanceType dom_type) { return (dom_type == POST_DOMINANCE) ? pdom_tree : dom_tree; }
	const DominatorTree<CONFIG>& GetDominatorTree(DominanceType dom_type) const { return (dom_type == POST_DOMINANCE) ? pdom_tree : dom_tree; }

	virtual JSON_Value *Save() const;
	static DominatorTreeCFGExtension<CONFIG> *Load(const JSON_Value& value);
	virtual void Fix(const Mapping<CONFIG>& mapping, const CFG<CONFIG>& cfg, const JSON_Value& value);
	
	virtual void Print(std::ostream& stream) const;
private:
	DominatorTree<CONFIG> dom_tree;
	DominatorTree<CONFIG> pdom_tree;
};

//////////////////////// DominanceExtensionFactory<> //////////////////////////

template <typename CONFIG>
struct DominanceExtensionFactory : ExtensionFactory
{
	virtual ExtensionBase *LoadExtension(ExtensionType ext_type, const std::string& ext_name, const JSON_Value& value) const;
};

//////////////////////////// BuildDominatorTree<> /////////////////////////////

// See Figure 3 of paper
template <typename CONFIG, DominanceType DOMINANCE_TYPE>
void BuildDominatorTree(CFG<CONFIG>& cfg);

//////////////////////// BuildDominanceFrontierSets<> /////////////////////////

// See Figure 5 of paper
template <typename CONFIG, DominanceType DOMINANCE_TYPE>
void BuildDominanceFrontierSets(CFG<CONFIG>& cfg);

} // end of namespace dominance
} // end of namespace cfg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CFG_DOMINANCE_DOMINANCE_HH__

