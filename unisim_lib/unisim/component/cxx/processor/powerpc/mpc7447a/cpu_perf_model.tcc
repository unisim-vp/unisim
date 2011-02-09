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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_PERF_MODEL_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_PERF_MODEL_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

template <class CONFIG>
Instruction<CONFIG>::Instruction()
{
}

template <class CONFIG>
Instruction<CONFIG>::~Instruction()
{
}

inline uint32_t Max(uint32_t a, uint32_t b)
{
	return a > b ? a : b;
}

inline uint32_t Min(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
}

template <class CONFIG>
Event<CONFIG> *CPU<CONFIG>::AllocateEvent()
{
	if(!event_free_list)
	{
		event_free_list = new Event<CONFIG>();
		event_free_list->next_free = 0;
	}

	Event<CONFIG> *ev = event_free_list;
	event_free_list = ev->next_free;
	ev->type = Event<CONFIG>::EV_NULL;
	ev->object.null = 0;
	ev->next_free = 0;
	return ev;
}

template <class CONFIG>
Instruction<CONFIG> *CPU<CONFIG>::AllocateInstruction()
{
	if(!insn_free_list)
	{
		insn_free_list = new Instruction<CONFIG>();
		insn_free_list->next_free = 0;
	}

	Instruction<CONFIG> *instruction = insn_free_list;
	insn_free_list = instruction->next_free;
	instruction->next_free = 0;
	return instruction;
}

template <class CONFIG>
Operand<CONFIG> *CPU<CONFIG>::AllocateOperand()
{
	if(!operand_free_list)
	{
		operand_free_list = new Operand<CONFIG>();
		operand_free_list->next_free = 0;
	}

	Operand<CONFIG> *operand = operand_free_list;
	operand_free_list = operand->next_free;
	operand->ref_count = 1;
	operand->next_free = 0;
	return operand;
}

template <class CONFIG>
void CPU<CONFIG>::AcquireOperand(Operand<CONFIG> *operand)
{
	operand->ref_count++;
}

template <class CONFIG>
LoadStoreAccess<CONFIG> *CPU<CONFIG>::AllocateLoadStoreAccess()
{
	if(!load_store_access_free_list)
	{
		load_store_access_free_list = new LoadStoreAccess<CONFIG>();
		load_store_access_free_list->next_free = 0;
	}

	LoadStoreAccess<CONFIG> *load_store_access = load_store_access_free_list;
	load_store_access_free_list = load_store_access->next_free;
	load_store_access->next_free = 0;	
	return load_store_access;
}

template <class CONFIG>
void CPU<CONFIG>::GenLoadStoreAccess(typename LoadStoreAccess<CONFIG>::Type type, unsigned int reg_num, typename CONFIG::address_t munged_ea, uint32_t size, Instruction<CONFIG> *instruction)
{
	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(size_to_fsb_boundary >= size)
	{
		// Memory load does not cross a FSB boundary
		LoadStoreAccess<CONFIG> *load_store_access = AllocateLoadStoreAccess();
		load_store_access->type = type;
		load_store_access->reg_num = reg_num;
		load_store_access->munged_ea = munged_ea;
		load_store_access->offset = 0;
		load_store_access->size = size;
		load_store_access->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access);
	}
	else
	{
		// Memory load crosses a FSB boundary
		LoadStoreAccess<CONFIG> *load_store_access1 = AllocateLoadStoreAccess();
		load_store_access1->type = type;
		load_store_access1->reg_num = reg_num;
		load_store_access1->munged_ea = munged_ea;
		load_store_access1->offset = 0;
		load_store_access1->size = size_to_fsb_boundary;
		load_store_access1->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access1);

		LoadStoreAccess<CONFIG> *load_store_access2 = AllocateLoadStoreAccess();
		load_store_access2->type = type;
		load_store_access2->reg_num = reg_num;
		load_store_access2->munged_ea = munged_ea + size_to_fsb_boundary;
		load_store_access2->offset = size_to_fsb_boundary;
		load_store_access2->size = size - size_to_fsb_boundary;
		load_store_access2->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access2);
	}
}

template <class CONFIG>
BusAccess<CONFIG> *CPU<CONFIG>::AllocateBusAccess()
{
	if(!bus_access_free_list)
	{
		bus_access_free_list = new BusAccess<CONFIG>();
		bus_access_free_list->next_free = 0;
	}

	BusAccess<CONFIG> *bus_access = bus_access_free_list;
	bus_access_free_list = bus_access->next_free;
	bus_access->next_free = 0;	
	return bus_access;
}

template <class CONFIG>
FetchAccess<CONFIG> *CPU<CONFIG>::AllocateFetchAccess()
{
	if(!fetch_access_free_list)
	{
		fetch_access_free_list = new FetchAccess<CONFIG>();
		fetch_access_free_list->next_free = 0;
	}

	FetchAccess<CONFIG> *fetch_access = fetch_access_free_list;
	fetch_access_free_list = fetch_access->next_free;
	fetch_access->next_free = 0;	
	return fetch_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeEvent(Event<CONFIG> *ev)
{
	ev->next_free = event_free_list;
	event_free_list = ev;
}

template <class CONFIG>
void CPU<CONFIG>::FreeInstruction(Instruction<CONFIG> *instruction)
{
	abort();
	unsigned int num_operands = instruction->input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_operands; i++)
	{
		Operand<CONFIG> *operand = instruction->input_operands[i];
		FreeOperand(operand);
	}

	instruction->next_free = insn_free_list;
	insn_free_list = instruction;
}

template <class CONFIG>
void CPU<CONFIG>::FreeOperand(Operand<CONFIG> *operand)
{
	if(--operand->ref_count == 0)
	{
		operand->next_free = operand_free_list;
		operand_free_list = operand;
	}
}

template <class CONFIG>
void CPU<CONFIG>::FreeLoadStoreAccess(LoadStoreAccess<CONFIG> *load_store_access)
{
	load_store_access->next_free = load_store_access_free_list;
	load_store_access_free_list = load_store_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeBusAccess(BusAccess<CONFIG> *bus_access)
{
	bus_access->next_free = bus_access_free_list;
	bus_access_free_list = bus_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeFetchAccess(FetchAccess<CONFIG> *fetch_access)
{
	fetch_access->next_free = fetch_access_free_list;
	fetch_access_free_list = fetch_access;
}


template <class CONFIG>
void CPU<CONFIG>::Fetch()
{
	// Stall if IQ is full
	if(iq.Full()) return;
	
	FetchAccess<CONFIG> *fetch_access = AllocateFetchAccess();
	unsigned int lat = 0;
	
	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), CONFIG::FETCH_WIDTH);
	fetch_access->cia = seq_cia;
	fetch_access->size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	fetch_access->mmu_access.addr = seq_cia;
	fetch_access->mmu_access.privilege_level = GetPrivilegeLevel();
	fetch_access->mmu_access.memory_type = CONFIG::MT_INSN;
	fetch_access->mmu_access.memory_access_type = CONFIG::MAT_READ;

	// Check wether IMMU is enabled
	if(GetMSR_IR())
	{
		// BATs and TLB Lookup
		LookupBAT<false>(fetch_access->mmu_access);
		if(!fetch_access->mmu_access.bat_hit)
		{
			AccessTLB<false>(fetch_access->mmu_access);
				
			if(!fetch_access->mmu_access.tlb_hit)
			{
				// BAT/TLB Miss
				abort(); // Hardware page table search should start at commit
			}
		}
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		fetch_access->mmu_access.wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		fetch_access->mmu_access.physical_addr = fetch_access->mmu_access.addr;
	}

	logger << DebugInfo << *fetch_access << EndDebugInfo;

	if(!(fetch_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsInsnCacheEnabled())
	{
		ClearAccessIL1(fetch_access->l1_access); // TO BE REMOVED
		// IL1 Access
		fetch_access->l1_access.addr = fetch_access->mmu_access.physical_addr;
		LookupIL1(fetch_access->l1_access);
		lat += CONFIG::IL1_LATENCY;

		if(fetch_access->l1_access.block)
		{
			// Fetch hit in IL1
			memcpy(fetch_access->data, &fetch_access->l1_access.block[fetch_access->l1_access.offset], fetch_access->size);
			UpdateReplacementPolicyIL1(fetch_access->l1_access);
			NotifyFinishedFetch(fetch_access, lat);
		}
		else
		{
			// Fetch miss in IL1
			if(num_outstanding_l1_fetch_miss >= CONFIG::MAX_OUTSTANDING_L1_FETCH_MISS) return;
			num_outstanding_l1_fetch_miss++;

			if(IsL2CacheEnabled())
			{
				ClearAccessL2(fetch_access->l2_access); // TO BE REMOVED
				// DL1 block fill from L2
				fetch_access->l2_access.addr = fetch_access->l1_access.block_base_addr;
			
				LookupL2(fetch_access->l2_access);
				lat += CONFIG::L2_LATENCY;

				if(fetch_access->l2_access.block)
				{
					// IL1 block fill hit in L2
					memcpy(&(*fetch_access->l1_access.block)[0], &(*fetch_access->l2_access.block)[fetch_access->l2_access.offset], CacheBlock<class CONFIG::IL1_CONFIG>::SIZE);
					UpdateReplacementPolicyL2(fetch_access->l2_access);
					memcpy(fetch_access->data, &(*fetch_access->l1_access.block)[fetch_access->l1_access.offset], fetch_access->size);
					UpdateReplacementPolicyIL1(fetch_access->l1_access);
					NotifyFinishedFetch(fetch_access, lat);
				}
				else
				{
					// Load miss in L2
					if(num_outstanding_l2_load_miss >= CONFIG::MAX_OUTSTANDING_L2_LOAD_MISS) return;

					// L2 block fill from bus
					fetch_access->l2_access.block = &(*fetch_access->l2_access.line)[fetch_access->l2_access.sector];

					BusAccess<CONFIG> *bus_access = AllocateBusAccess();
					bus_access->issued = false;
					bus_access->type = BusAccess<CONFIG>::REFILL;
					bus_access->addr = fetch_access->l2_access.block_base_addr;
					bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
					bus_access->load_store_access = 0;
					bus_access->fetch_access = fetch_access;
					bus_access->wimg = fetch_access->mmu_access.wimg;
					bus_access->rwitm = false;
					NotifyBusAccess(bus_access, lat);
				}
			}
			else
			{
				// DL1 block fill from bus
				BusAccess<CONFIG> *bus_access = AllocateBusAccess();
				bus_access->issued = false;
				bus_access->type = BusAccess<CONFIG>::REFILL;
				bus_access->addr = fetch_access->l1_access.block_base_addr;
				bus_access->size = CacheBlock<class CONFIG::IL1_CONFIG>::SIZE;
				bus_access->load_store_access = 0;
				bus_access->fetch_access = fetch_access;
				bus_access->wimg = fetch_access->mmu_access.wimg;
				bus_access->rwitm = false;
				NotifyBusAccess(bus_access, lat);
			}
		}
	}
	else
	{
		// Caching is either disabled or inhibited
		BusAccess<CONFIG> *bus_access = AllocateBusAccess();
		bus_access->issued = false;
		bus_access->type = BusAccess<CONFIG>::LOAD;
		bus_access->addr = fetch_access->mmu_access.physical_addr;
		bus_access->size = fetch_access->size;
		bus_access->load_store_access = 0;
		bus_access->fetch_access = fetch_access;
		bus_access->wimg = fetch_access->mmu_access.wimg;
		bus_access->rwitm = false;
		NotifyBusAccess(bus_access, lat);
	}

	seq_cia += fetch_access->size;
	// Send a request to IMMU
/*	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), FETCH_WIDTH);
	uint32_t fetch_size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	uint8_t prefetch_buffer[fetch_size];
	uint32_t read_size = 0; //ReadInsnMemory(cia, prefetch_buffer, sizeof(prefetch_buffer) > size_to_block_boundary ? size_to_block_boundary : sizeof(prefetch_buffer));
	uint32_t offset;
	for(offset = 0; offset < read_size; offset += sizeof(uint32_t), cia += sizeof(uint32_t))
	{
		uint32_t insn = prefetch_buffer[offset];
		Instruction<CONFIG> *instruction = AllocateInstruction();
		instruction->Initialize(this, cia, insn);
		iq.Push(instruction);
	}*/
}

template <class CONFIG>
void CPU<CONFIG>::DecodeDispatch()
{
	if(iq.Empty()) return;

	unsigned int num_load_stores = 0;
	unsigned int i, j;
	for(i = 0; i < CONFIG::DECODE_WIDTH; i++)
	{
		// Check for completion queue availability
		if(cq.Full()) return;

		// Lookup the last instruction from the instruction queue
		Instruction<CONFIG> *instruction = iq.Front();

		const typename CONFIG::execution_unit_type_t execution_unit = instruction->GetExecutionUnit();

		// Check for specific dispatch constraints
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
				if(num_load_stores >= CONFIG::MAX_DISPATCHED_LOAD_STORES_PER_CYCLE) return;
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				break;
		}

		// Check for issue queue availability
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				if(giq.Full()) return;
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				if(fiq.Full()) return;
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				if(viq.Full()) return;
				break;
		}

		// Check for rename registers availability
		unsigned int num_output_fpr = instruction->GetNumOutputFPR();
		if(num_output_fpr && num_output_fpr > fpr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_gpr = instruction->GetNumOutputGPR();
		if(num_output_gpr && num_output_gpr > gpr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_cr = instruction->GetNumOutputCR();
		if(num_output_cr && num_output_cr > cr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_lr = instruction->GetNumOutputLR();
		if(num_output_lr && num_output_lr > lr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_ctr = instruction->GetNumOutputCTR();
		if(num_output_ctr && num_output_ctr > ctr_mapping_table.GetNumFreeRenameRegisters()) return;

		// Locate input operands
		unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation = instruction->operation;
		unsigned int num_operands = operation->num_insn_operands;

		for(j = 0; j < num_operands; j++)
		{
			typename CONFIG::operand_t *insn_operand = &operation->insn_operands[i];
	
			if(insn_operand->dir == CONFIG::INPUT_T)
			{
				Operand<CONFIG> *operand = 0;
				bool forwarding = true;

				switch(insn_operand->type)
				{
					case CONFIG::GPR_T:
						{
							unsigned int gpr_reg_num = insn_operand->reg_num;
							int gpr_tag = gpr_mapping_table.Lookup(gpr_reg_num);
							operand = (gpr_tag >= 0) ? arch_gpr[gpr_reg_num] : rename_gpr[~gpr_tag];
						}
						break;

					case CONFIG::GPR_ST_T:
						{
							unsigned int gpr_reg_num = insn_operand->reg_num;
							int gpr_tag = gpr_mapping_table.Lookup(gpr_reg_num);
							operand = (gpr_tag >= 0) ? arch_gpr[gpr_reg_num] : rename_gpr[~gpr_tag];
							forwarding = false; // store value is not forwarded to L/S reservation station
						}
						break;

					case CONFIG::FPR_T:
						{
							unsigned int fpr_reg_num = insn_operand->reg_num;
							int fpr_tag = fpr_mapping_table.Lookup(fpr_reg_num);
							operand = (fpr_tag >= 0) ? arch_fpr[fpr_reg_num] : rename_fpr[~fpr_tag];
						}
						break;
		
					case CONFIG::CR_T:
						{
							int cr_tag = cr_mapping_table.Lookup(0);
							operand = (cr_tag >= 0) ? arch_cr : rename_cr[~cr_tag];
						}
						break;
		
					case CONFIG::LR_T:
						{
							int lr_tag = lr_mapping_table.Lookup(0);
							operand = (lr_tag >= 0) ? arch_lr : rename_lr[~lr_tag];
						}
						break;
		
					case CONFIG::CTR_T:
						{
							int ctr_tag = ctr_mapping_table.Lookup(0);
							operand = (ctr_tag >= 0) ? arch_ctr : rename_ctr[~ctr_tag];
						}
						break;

					default:
						logger << DebugError << "Unhandled operand type" << EndDebugError;
						Stop(-1);
				}

				AcquireOperand(operand);
				instruction->input_operands.Push(operand);
				instruction->forwarding.Push(forwarding);
			}
		}

		// Allocate rename registers for output operands
		for(j = 0; j < num_operands; j++)
		{
			typename CONFIG::operand_t *insn_operand = &operation->insn_operands[i];
	
			if(insn_operand->dir == CONFIG::OUTPUT_T)
			{
				Operand<CONFIG> *operand = AllocateOperand();
				operand->valid = false;

				switch(insn_operand->type)
				{
					case CONFIG::GPR_T:
						{
							operand->type = Operand<CONFIG>::GPR;
							operand->reg_num = insn_operand->reg_num;
							gpr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_gpr[~operand->tag] = operand;
						}
						break;

					case CONFIG::FPR_T:
						{
							operand->type = Operand<CONFIG>::FPR;
							operand->reg_num = insn_operand->reg_num;
							fpr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_fpr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::CR_T:
						{
							operand->type = Operand<CONFIG>::CR;
							operand->reg_num = 0;
							cr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_cr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::LR_T:
						{
							operand->type = Operand<CONFIG>::LR;
							operand->reg_num = 0;
							lr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_lr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::CTR_T:
						{
							operand->type = Operand<CONFIG>::CTR;
							operand->reg_num = 0;
							ctr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_ctr[~operand->tag] = operand;
						}
						break;

					case CONFIG::XER_T:
						{
							operand->type = Operand<CONFIG>::XER;
							operand->reg_num = 0;
						}
						break;

					case CONFIG::FPSCR_T:
						{
							operand->type = Operand<CONFIG>::FPSCR;
							operand->reg_num = 0;
						}
						break;

					default:
						logger << DebugError << "Unhandled operand type" << EndDebugError;
						Stop(-1);
				}

				instruction->output_operands.Push(operand);
			}
		}

		// Move the instruction from the instruction queue to completion queue and an issue queue
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
				num_load_stores++;
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				giq.Push(instruction);
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				fiq.Push(instruction);
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				viq.Push(instruction);
				break;
		}

		cq.Push(instruction);

		if(++dispatch_uop_num < instruction->GetNumUOps())
		{
			iq[0] = AllocateInstruction();
			iq[0]->Initialize(this, *instruction, dispatch_uop_num);
		}
		else
		{
			dispatch_uop_num = 0;
			iq.Pop();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::GPRIssue()
{
	// Issue is Out-of-order
	unsigned int i, j, k;
	for(i = k = 0; i < CONFIG::GPR_ISSUE_WIDTH; i++)
	{
		if(k >= giq.Size()) return;
		Instruction<CONFIG> *instruction = giq[k];

		const typename CONFIG::execution_unit_type_t execution_unit = instruction->GetExecutionUnit();

		switch(execution_unit)
		{
			case CONFIG::LSU_T:     // Load/Store Unit
				if(!lsu_reservation_station.Full())
				{
					lsu_reservation_station.Push(instruction);
					giq.Remove(k);
				}
				else
				{
					k++;
				}
				break;
			case CONFIG::IU1_T:     // Simple Integer Unit
				{
					bool found_iu1 = false;
					for(j = 0; j < CONFIG::NUM_IU1; j++)
					{
						if(!iu1_reservation_station[j].Full())
						{
							iu1_reservation_station[j].Push(instruction);
							found_iu1 = true;
							break;
						}
					}

					if(found_iu1)
					{
						giq.Remove(k);
					}
					else
					{
						k++;
					}
				}
				break;
			case CONFIG::IU2_T:     // Complex Integer Unit
				{
					bool found_iu2 = false;
					for(j = 0; j < CONFIG::NUM_IU2; j++)
					{
						if(!iu2_reservation_station[j].Full())
						{
							iu2_reservation_station[j].Push(instruction);
							found_iu2 = true;
							break;
						}
					}

					if(found_iu2)
					{
						giq.Remove(k);
					}
					else
					{
						k++;
					}
				}
				break;

			default:
				logger << DebugError << "Unhandled execution unit type" << EndDebugError;
				Stop(-1);

		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::FPRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::VRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::IU1Execute()
{
	unsigned int i;
	for(i = 0; i < CONFIG::NUM_IU1; i++)
	{
		if(!iu1_reservation_station[i].Empty())
		{
			Instruction<CONFIG> *instruction = iu1_reservation_station[i].Front();

			if(instruction->IsAllowedToExecute() && instruction->IsReadyToExecute())
			{
				instruction->Execute(this);

				NotifyFinishedInstruction(instruction, instruction->GetLatency());
				iu1_reservation_station[i].Pop();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::IU2Execute()
{
	unsigned int i;
	for(i = 0; i < CONFIG::NUM_IU2; i++)
	{
		if(!iu2_reservation_station[i].Empty())
		{
			Instruction<CONFIG> *instruction = iu2_reservation_station[i].Front();

			if(instruction->IsAllowedToExecute() && instruction->IsReadyToExecute())
			{
				instruction->Execute(this);
				NotifyFinishedInstruction(instruction, instruction->GetLatency());
				iu2_reservation_station[i].Pop();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::FPUExecute()
{
}

template <class CONFIG>
void CPU<CONFIG>::LSUExecute1()
{
	if(!lsu_reservation_station.Empty())
	{
		Instruction<CONFIG> *instruction = lsu_reservation_station.Front();

		if(!instruction->IsAllowedToExecute()) return;
		if(!instruction->IsReadyToExecute()) return;
		if(required_ea.Full()) return;

		instruction->Execute(this);
		required_ea.Push(instruction);
		lsu_reservation_station.Pop();
	}
}

// 		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
// 		{
// 			unsigned int num_load_store_accesses = instruction->load_store_access_queue.Size();
// 			NotifyFinishedInstruction(instruction, num_load_store_accesses);
// 		}
// 		else
// 		{


template <class CONFIG>
void CPU<CONFIG>::LSUExecute2()
{
	unsigned int lat = 0;

	if(!required_ea.Empty())
	{
		Instruction<CONFIG> *instruction = required_ea.Front();

		LoadStoreAccess<CONFIG> *load_store_access = instruction->load_store_access_queue[instruction->load_store_access_index];
	
		load_store_access->mmu_access.addr = load_store_access->munged_ea;
		load_store_access->mmu_access.privilege_level = GetPrivilegeLevel();
		load_store_access->mmu_access.memory_type = CONFIG::MT_DATA;
		
		if(GetMSR_DR())
		{
			// BATs and TLB Lookup
			load_store_access->mmu_access.memory_access_type = (load_store_access->type & LoadStoreAccess<CONFIG>::STORE) ? CONFIG::MAT_WRITE : CONFIG::MAT_READ;
	
			LookupBAT<false>(load_store_access->mmu_access);
			if(!load_store_access->mmu_access.bat_hit)
			{
				AccessTLB<false>(load_store_access->mmu_access);
					
				if(!load_store_access->mmu_access.tlb_hit || load_store_access->mmu_access.force_page_table_walk)
				{
					// BAT/TLB Miss
					abort(); // Hardware page table search should start at commit
				}
			}
		}
		else
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			load_store_access->mmu_access.wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			load_store_access->mmu_access.physical_addr = load_store_access->munged_ea;
		}

		if(load_store_access->type & LoadStoreAccess<CONFIG>::LOAD)
		{
			// Load
			if(!(load_store_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
			{
				// DL1 Access
				load_store_access->l1_access.addr = load_store_access->mmu_access.physical_addr;
				LookupDL1(load_store_access->l1_access);
				lat += CONFIG::DL1_LATENCY;
	
				if(load_store_access->l1_access.block)
				{
					// Load hit in DL1
					memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
					UpdateReplacementPolicyDL1(load_store_access->l1_access);
					NotifyLoadResultAvailability(load_store_access, lat);
				}
				else
				{
					// Load miss in DL1
					if(num_outstanding_l1_load_miss >= CONFIG::MAX_OUTSTANDING_L1_LOAD_MISS) return;
		
					if(IsL2CacheEnabled())
					{
						// DL1 block fill from L2
						load_store_access->l2_access.addr = load_store_access->l1_access.block_base_addr;
					
						LookupL2(load_store_access->l2_access);
						lat += CONFIG::L2_LATENCY;
	
						if(load_store_access->l2_access.block)
						{
							// DL1 block fill hit in L2
							memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
							UpdateReplacementPolicyL2(load_store_access->l2_access);
							memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
							UpdateReplacementPolicyDL1(load_store_access->l1_access);
							NotifyLoadResultAvailability(load_store_access, lat);
						}
						else
						{
							// Load miss in L2
							if(num_outstanding_l2_load_miss >= CONFIG::MAX_OUTSTANDING_L2_LOAD_MISS) return;
	
							// L2 block fill from bus
							load_store_access->l2_access.block = &(*load_store_access->l2_access.line)[load_store_access->l2_access.sector];
		
							BusAccess<CONFIG> *bus_access = AllocateBusAccess();
							bus_access->issued = false;
							bus_access->type = BusAccess<CONFIG>::REFILL;
							bus_access->addr = load_store_access->l2_access.block_base_addr;
							bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
							bus_access->load_store_access = load_store_access;
							bus_access->fetch_access = 0;
							bus_access->wimg = load_store_access->mmu_access.wimg;
							bus_access->rwitm = false;
							NotifyBusAccess(bus_access, lat);
						}
					}
					else
					{
						// DL1 block fill from bus
						BusAccess<CONFIG> *bus_access = AllocateBusAccess();
						bus_access->issued = false;
						bus_access->type = BusAccess<CONFIG>::REFILL;
						bus_access->addr = load_store_access->l1_access.block_base_addr;
						bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
						bus_access->load_store_access = load_store_access;
						bus_access->fetch_access = 0;
						bus_access->wimg = load_store_access->mmu_access.wimg;
						bus_access->rwitm = false;
						NotifyBusAccess(bus_access, lat);
					}
				}
			}
			else
			{
				// Caching is either disabled or inhibited
				BusAccess<CONFIG> *bus_access = AllocateBusAccess();
				bus_access->issued = false;
				bus_access->type = BusAccess<CONFIG>::LOAD;
				bus_access->addr = load_store_access->mmu_access.physical_addr;
				bus_access->size = load_store_access->size;
				bus_access->load_store_access = load_store_access;
				bus_access->fetch_access = 0;
				bus_access->wimg = load_store_access->mmu_access.wimg;
				bus_access->rwitm = false;
				NotifyBusAccess(bus_access, lat);
			}
		}

		if(++instruction->load_store_access_index >= instruction->load_store_access_queue.Size())
		{
			required_ea.Pop();

			if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
			{
				instruction->load_store_access_index = 0;
				NotifyFinishedInstruction(instruction, 1);
			}
			else
			{
				return;
			}
		}
	}

	if(!csq.Empty())
	{
		Instruction<CONFIG> *instruction = csq.Front();

		LoadStoreAccess<CONFIG> *load_store_access = instruction->load_store_access_queue[instruction->load_store_access_index];

		switch(load_store_access->type)
		{
			case LoadStoreAccess<CONFIG>::INT8_STORE:
				{
					uint8_t value = instruction->GetGPR(load_store_access->reg_num);
					memcpy(load_store_access->data, &value, sizeof(value));
				}
				break;
			case LoadStoreAccess<CONFIG>::INT16_STORE:
				{
					uint16_t value = Host2BigEndian((uint16_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT32_STORE:
				{
					uint32_t value = Host2BigEndian((uint32_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP32_STORE:
				{
					Flags flags;
					flags.setRoundingMode((fpscr & CONFIG::FPSCR_RN_MASK) >> CONFIG::FPSCR_RN_OFFSET);
					uint32_t value = Host2BigEndian(SoftFloat(instruction->GetFPR(load_store_access->reg_num), flags).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP64_STORE:
				{
					uint64_t value = Host2BigEndian(instruction->GetFPR(load_store_access->reg_num).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP_STORE_LSW:
				{
					uint32_t value = Host2BigEndian((uint32_t) instruction->GetFPR(load_store_access->reg_num).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE:
				{
					uint16_t value = Host2LittleEndian((uint16_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE:
				{
					uint32_t value = Host2LittleEndian((uint32_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT_STORE_MSB:
				switch(load_store_access->size)
				{
					case 1:
						{
							uint8_t value = instruction->GetGPR(load_store_access->reg_num) >> 24;
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
						}
						break;
					case 2:
						{
							uint16_t value = Host2BigEndian((uint16_t)(instruction->GetGPR(load_store_access->reg_num) >> 16));
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
							break;
						}
			
					case 3:
						{
							uint32_t value = instruction->GetGPR(load_store_access->reg_num);
							uint8_t buffer[3];
							buffer[0] = value >> 24;
							buffer[1] = value >> 16;
							buffer[2] = value >> 8;
							memcpy(load_store_access->data, buffer + load_store_access->offset, load_store_access->size);
							break;
						}
			
					case 4:
						{
							uint32_t value = Host2BigEndian(instruction->GetGPR(load_store_access->reg_num));
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
							break;
						}
				}
				break;

			default:
				logger << DebugError << "Unhandled load/store access type" << EndDebugError;
				Stop(-1);
		}

		if(!(load_store_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
		{
			// DL1 Access
			load_store_access->l1_access.addr = load_store_access->mmu_access.physical_addr;
			LookupDL1(load_store_access->l1_access);
			lat += CONFIG::DL1_LATENCY;

			if(load_store_access->l1_access.block)
			{
				// Store hit in DL1
				memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
				load_store_access->l1_access.block->status.dirty = true;
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
			}
			else
			{
				// Store miss in DL1
				if(num_outstanding_l1_store_miss >= CONFIG::MAX_OUTSTANDING_L1_STORE_MISS) return;
	
				if(IsL2CacheEnabled())
				{
					// DL1 block fill from L2
					load_store_access->l2_access.addr = load_store_access->l1_access.block_base_addr;
				
					LookupL2(load_store_access->l2_access);
					lat += CONFIG::L2_LATENCY;
	
					if(load_store_access->l2_access.block)
					{
						// DL1 block fill hit in L2
						memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
						UpdateReplacementPolicyL2(load_store_access->l2_access);
						memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
						UpdateReplacementPolicyDL1(load_store_access->l1_access);
					}
					else
					{
						// Store miss in L2
						if(num_outstanding_l2_store_miss >= CONFIG::MAX_OUTSTANDING_L2_STORE_MISS) return;
	
						// L2 block fill from bus
						load_store_access->l2_access.block = &(*load_store_access->l2_access.line)[load_store_access->l2_access.sector];
	
						BusAccess<CONFIG> *bus_access = AllocateBusAccess();
						bus_access->issued = false;
						bus_access->type = BusAccess<CONFIG>::REFILLX;
						bus_access->addr = load_store_access->l2_access.block_base_addr;
						bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
						bus_access->load_store_access = load_store_access;
						bus_access->fetch_access = 0;
						bus_access->wimg = load_store_access->mmu_access.wimg;
						bus_access->rwitm = false;
						NotifyBusAccess(bus_access, lat);
					}
				}
				else
				{
					// DL1 block fill from bus
					BusAccess<CONFIG> *bus_access = AllocateBusAccess();
					bus_access->issued = false;
					bus_access->type = BusAccess<CONFIG>::REFILLX;
					bus_access->addr = load_store_access->l1_access.block_base_addr;
					bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
					bus_access->load_store_access = load_store_access;
					bus_access->fetch_access = 0;
					bus_access->wimg = load_store_access->mmu_access.wimg;
					bus_access->rwitm = false;
					NotifyBusAccess(bus_access, lat);
				}
			}
		}
		else
		{
			// Caching is either disabled or inhibited
			BusAccess<CONFIG> *bus_access = AllocateBusAccess();
			bus_access->issued = false;
			bus_access->type = BusAccess<CONFIG>::STORE;
			bus_access->addr = load_store_access->mmu_access.physical_addr;
			bus_access->size = load_store_access->size;
			bus_access->load_store_access = load_store_access;
			bus_access->fetch_access = 0;
			bus_access->wimg = load_store_access->mmu_access.wimg;
			bus_access->rwitm = false;
			NotifyBusAccess(bus_access, lat);
		}
		csq.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::ScheduleEvents()
{
	while(!schedule.empty())
	{
		typename std::multimap<uint64_t, Event<CONFIG> *>::iterator it = schedule.begin();
		if(it->first > cpu_cycle) return;

		Event<CONFIG> *ev = it->second;

		switch(ev->type)
		{
			case Event<CONFIG>::EV_NULL:
				break;
			case Event<CONFIG>::EV_FINISHED_INSN:     // an instruction is finished
				{
					Instruction<CONFIG> *instruction = ev->object.instruction;

					instruction->Finish();
					if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
					{
						if(!fsq.Full())
						{
							fsq.Push(instruction);
							required_ea.Pop();
						}
					}
				}
				break;
			case Event<CONFIG>::EV_AVAILABLE_OPERAND: // an operand is available
				{
					Operand<CONFIG> *operand = ev->object.operand;

					operand->valid = true;
					
					unsigned int num_pending_instructions = operand->pending_instructions.Size();
					unsigned int i, j;
			
					for(i = j = 0; i < num_pending_instructions; i++)
					{
						Instruction<CONFIG> *instruction = operand->pending_instructions[j];
						
						if(instruction->CheckInputOperandsAvailability())
						{
							operand->pending_instructions.Remove(j);
						}
						else
						{
							j++;
						}
					}
				}
				break;
			case Event<CONFIG>::EV_BUS_ACCESS:        // a cache miss causes a bus access
				{
					BusAccess<CONFIG> *bus_access = ev->object.bus_access;

					switch(bus_access->type)
					{
						case BusAccess<CONFIG>::LOAD:     // cache inhibited load
						case BusAccess<CONFIG>::REFILL:   // cache refill
						case BusAccess<CONFIG>::REFILLX:  // cache refill with intent to modify
							if(!blq.Full())
								blq.Push(bus_access);
							else
								NotifyEvent(ev, 1); // Reschedule event on next cycle
							break;
						case BusAccess<CONFIG>::STORE:    // write through/cache inhibited store
						case BusAccess<CONFIG>::EVICTION: // block eviction
							if(!bsq.Full())
								bsq.Push(bus_access);
							else
								NotifyEvent(ev, 1); // Reschedule event on next cycle
							break;
					}
				}
				break;
				
			case Event<CONFIG>::EV_FINISHED_FETCH:     // a fetch has finished
				{
					FetchAccess<CONFIG> *fetch_access = ev->object.fetch_access;
					unsigned int offset;
					unsigned int i;
					
					for(offset = 0, i = 0; !iq.Full() && offset < fetch_access->size; offset += 4, i++)
					{
						uint32_t insn = Host2BigEndian(*(uint32_t *) &fetch_access->data[offset]);
						Instruction<CONFIG> *instruction = AllocateInstruction();
						instruction->Initialize(this, fetch_access->cia + i, insn);
						iq.Push(instruction);
					}
					FreeFetchAccess(fetch_access);
				}
				break;
		}

		schedule.erase(it);
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpSchedule()
{
	stringstream sstr;
	typename std::multimap<uint64_t, Event<CONFIG> *>::iterator it;
	
	for(it = schedule.begin(); it != schedule.end(); it++)
	{
		Event<CONFIG> *ev = it->second;
		
		sstr << "EVENT(time=" << it->first << ",";
		
		switch(ev->type)
		{
			case Event<CONFIG>::EV_NULL:
				sstr << "null";
				break;
			case Event<CONFIG>::EV_FINISHED_INSN:     // an instruction is finished
				{
					Instruction<CONFIG> *instruction = ev->object.instruction;

					sstr << *instruction;
				}
				break;
			case Event<CONFIG>::EV_AVAILABLE_OPERAND: // an operand is available
				{
					Operand<CONFIG> *operand = ev->object.operand;

					sstr << *operand;
				}
				break;
			case Event<CONFIG>::EV_BUS_ACCESS:        // a cache miss causes a bus access
				{
					BusAccess<CONFIG> *bus_access = ev->object.bus_access;
					sstr << *bus_access;
				}
				break;
				
			case Event<CONFIG>::EV_FINISHED_FETCH:     // a fetch has finished
				{
					FetchAccess<CONFIG> *fetch_access = ev->object.fetch_access;
					sstr << *fetch_access;
				}
				break;
		}
		sstr << ")" << endl;
	}
	
	logger << sstr.str();
}

template <class CONFIG>
bool CPU<CONFIG>::HasPipelineCollision(BusAccess<CONFIG> *bus_load_access)
{
	unsigned int i;
	unsigned int bsq_size = bsq.Size();
	for(i = 0; i < bsq_size; i++)
	{
		BusAccess<CONFIG> *bus_store_access = bsq[i];
		if(bus_load_access->addr == bus_store_access->addr) return true;
	}

	return false;
}

template <class CONFIG>
void CPU<CONFIG>::BIU()
{
	if(!blq.Empty())
	{
		BusAccess<CONFIG> *bus_access = blq.Front();
		if(!bus_access->issued && !HasPipelineCollision(bus_access))
		{
			bus_access->issued = true;
			DoBusAccess(bus_access);
			return;
		}
	}

	if(!bsq.Empty())
	{
		BusAccess<CONFIG> *bus_access = bsq.Front();
		if(!bus_access->issued)
		{
			bus_access->issued = true;
			DoBusAccess(bus_access);
		}
	}
}


template <class CONFIG>
void CPU<CONFIG>::OnFinishedBusAccess(BusAccess<CONFIG> *bus_access)
{
	if(bus_access->fetch_access)
	{
		FetchAccess<CONFIG> *fetch_access = bus_access->fetch_access;
		unsigned int lat = 0;
		
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsInsnCacheEnabled()) && !fetch_access->l1_access.line)
				{
					// IL1 Line miss
					ChooseLineToEvictIL1(fetch_access->l1_access);
					
					uint32_t l1_sector;
					
					for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::IL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
					{
						CacheBlock<class CONFIG::IL1_CONFIG>& l1_block_to_evict = (*fetch_access->l1_access.line_to_evict)[l1_sector];
						l1_block_to_evict.status.valid = false;
					}
					
					fetch_access->l1_access.line_to_evict->status.valid = false;
					fetch_access->l1_access.line = fetch_access->l1_access.line_to_evict;
					fetch_access->l1_access.line_to_evict = 0;
					fetch_access->l1_access.block = &(*fetch_access->l1_access.line)[fetch_access->l1_access.sector];				
				}

				if(likely(IsL2CacheEnabled()) && !fetch_access->l2_access.line)
				{
					// IL1 Line miss
					ChooseLineToEvictL2(fetch_access->l2_access);
					// L2 Line eviction
					uint32_t l2_sector;
				
					for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
					{
						CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*fetch_access->l2_access.line_to_evict)[l2_sector];
				
						l2_block_to_evict.status.valid = false;
						l2_block_to_evict.status.dirty = false;
					}
					
					fetch_access->l2_access.line_to_evict->status.valid = false;
					fetch_access->l2_access.line = fetch_access->l2_access.line_to_evict;
					fetch_access->l2_access.line_to_evict = 0;
					fetch_access->l2_access.block = &(*fetch_access->l2_access.line)[fetch_access->l2_access.sector];				
				}
				
				num_outstanding_l1_fetch_miss--;
				break;

			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize cache refills
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsL2CacheEnabled()) && fetch_access->l2_access.line)
				{
					if(!fetch_access->l2_access.block)
					{
						logger << DebugError << "PANIC ! null block" << EndDebugError;
						Stop(-1);
					}
					logger << DebugInfo << "block=0x" << std::hex << fetch_access->l2_access.block << std::dec << EndDebugInfo;
					// Finalize L2 Refill
					memcpy(&(*fetch_access->l2_access.block)[0], bus_access->storage, CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
					fetch_access->l2_access.line->status.valid = true;
					fetch_access->l2_access.line->SetBaseAddr(fetch_access->l2_access.line_base_addr);
					fetch_access->l2_access.block->status.valid = true;
					fetch_access->l2_access.block->status.dirty = false;
				}

				if(likely(IsInsnCacheEnabled()) && fetch_access->l1_access.line)
				{
					// Finalize IL1 refill
					memcpy(&(*fetch_access->l1_access.block)[0], &(*fetch_access->l2_access.block)[fetch_access->l2_access.offset], CacheBlock<class CONFIG::IL1_CONFIG>::SIZE);
					UpdateReplacementPolicyL2(fetch_access->l2_access);
					fetch_access->l1_access.line->status.valid = true;
					fetch_access->l1_access.line->SetBaseAddr(fetch_access->l1_access.line_base_addr);
					fetch_access->l1_access.block->status.valid = true;
				}
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize Fetch access
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::LOAD:
				// Finalize fetch from bus and forward raw data to IQ
				memcpy(fetch_access->data, bus_access->storage, fetch_access->size);
				NotifyFinishedFetch(fetch_access, lat);
				break;
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsInsnCacheEnabled()) && fetch_access->l1_access.line)
				{
					// Finalize fetch from IL1 and forward raw data to IQ
					memcpy(fetch_access->data, &(*fetch_access->l1_access.block)[fetch_access->l1_access.offset], fetch_access->size);
					UpdateReplacementPolicyIL1(fetch_access->l1_access);
					NotifyFinishedFetch(fetch_access, lat);
				}
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}
	}
	
	if(bus_access->load_store_access)
	{
		LoadStoreAccess<CONFIG> *load_store_access = bus_access->load_store_access;
		unsigned int lat = 0;

		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
			case BusAccess<CONFIG>::REFILLX:
				if(!load_store_access->l1_access.line)
				{
					// DL1 Line miss
					ChooseLineToEvictDL1(load_store_access->l1_access);
					
					if(load_store_access->l1_access.line_to_evict->status.valid)
					{
						// DL1 Line eviction
						uint32_t l1_sector;
					
						for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
						{
							CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_evict = (*load_store_access->l1_access.line_to_evict)[l1_sector];
					
							if(l1_block_to_evict.status.valid && l1_block_to_evict.status.dirty)
							{
								// dirty DL1 block eviction
								if(IsL2CacheEnabled())
								{
									// DL1 block eviction into L2
									CacheAccess<class CONFIG::L2_CONFIG> l2_access;
									l2_access.addr = l1_block_to_evict.GetBaseAddr();
									LookupL2(l2_access);
									lat += CONFIG::L2_LATENCY;
				
									if(l2_access.block)
									{
										// DL1 block eviction hit in L2
										memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_evict[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
										l2_access.block->status.dirty = true;
					
										UpdateReplacementPolicyL2(l2_access);
									}
									else
									{
										// dirty DL1 block eviction miss in L2
										// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
										BusAccess<CONFIG> *bus_access = AllocateBusAccess();
										bus_access->issued = false;
										bus_access->type = BusAccess<CONFIG>::EVICTION;
										bus_access->addr = l1_block_to_evict.GetBaseAddr();
										bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
										memcpy(bus_access->storage, &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
										bus_access->load_store_access = load_store_access;
										bus_access->fetch_access = 0;
										NotifyBusAccess(bus_access, lat);
									}
								}
								else
								{
									// dirty DL1 block eviction into memory
									// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
									BusAccess<CONFIG> *bus_access = AllocateBusAccess();
									bus_access->issued = false;
									bus_access->type = BusAccess<CONFIG>::EVICTION;
									bus_access->addr = l1_block_to_evict.GetBaseAddr();
									bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
									memcpy(bus_access->storage, &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
									bus_access->load_store_access = load_store_access;
									bus_access->fetch_access = 0;
									NotifyBusAccess(bus_access, lat);
								}
							}
							l1_block_to_evict.status.valid = false;
							l1_block_to_evict.status.dirty = false;
						}
						load_store_access->l1_access.line_to_evict->status.valid = false;
					}
					load_store_access->l1_access.line = load_store_access->l1_access.line_to_evict;
					load_store_access->l1_access.line_to_evict = 0;
				}

				if(!load_store_access->l2_access.line)
				{
					if(load_store_access->l2_access.line_to_evict->status.valid)
					{
						// L2 Line eviction
						uint32_t l2_sector;
					
						for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
						{
							CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*load_store_access->l2_access.line_to_evict)[l2_sector];
					
							if(l2_block_to_evict.status.valid && l2_block_to_evict.status.dirty)
							{
								// dirty L2 block eviction into memory
								// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
								BusAccess<CONFIG> *bus_access = AllocateBusAccess();
								bus_access->issued = false;
								bus_access->type = BusAccess<CONFIG>::EVICTION;
								bus_access->addr = l2_block_to_evict.GetBaseAddr();
								bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
								memcpy(bus_access->storage, &l2_block_to_evict[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
								bus_access->load_store_access = load_store_access;
								bus_access->fetch_access = 0;
								NotifyBusAccess(bus_access, lat);
							}
							l2_block_to_evict.status.valid = false;
							l2_block_to_evict.status.dirty = false;
						}
					
						load_store_access->l2_access.line_to_evict->status.valid = false;
					}
					load_store_access->l2_access.line = load_store_access->l2_access.line_to_evict;
					load_store_access->l2_access.line_to_evict = 0;
				}

				break;
			case BusAccess<CONFIG>::STORE:
			case BusAccess<CONFIG>::EVICTION:
				FreeInstruction(load_store_access->instruction);
				bsq.Pop();
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize cache refills
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
			case BusAccess<CONFIG>::REFILLX:
				// Finalize L2 Refill
				memcpy(&(*load_store_access->l2_access.block)[0], bus_access->storage, CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
				load_store_access->l2_access.line->status.valid = true;
				load_store_access->l2_access.line->SetBaseAddr(load_store_access->l2_access.line_base_addr);
				load_store_access->l2_access.block->status.valid = true;
				load_store_access->l2_access.block->status.dirty = false;

				// Finalize DL1 refill
				memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				UpdateReplacementPolicyL2(load_store_access->l2_access);
				load_store_access->l1_access.line->status.valid = true;
				load_store_access->l1_access.line->SetBaseAddr(load_store_access->l1_access.line_base_addr);
				load_store_access->l1_access.block->status.valid = true;
				load_store_access->l1_access.block->status.dirty = false;
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize Load/Store access
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::LOAD:
			case BusAccess<CONFIG>::REFILL:
				// Finalize load in DL1 and forward load result
				memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
				NotifyLoadResultAvailability(load_store_access, lat);
				NotifyFinishedInstruction(load_store_access->instruction, lat);
				blq.Pop();
				break;
			case BusAccess<CONFIG>::REFILLX:
			case BusAccess<CONFIG>::STORE:
				// Finalize store in DL1
				memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
				bsq.Pop();
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::NotifyEvent(Event<CONFIG> *ev, uint64_t latency)
{
	schedule.insert(std::pair<uint64_t, Event<CONFIG> *>(cpu_cycle + latency, ev));
}

template <class CONFIG>
void CPU<CONFIG>::NotifyBusAccess(BusAccess<CONFIG> *bus_access, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_BUS_ACCESS;
	ev->object.bus_access = bus_access;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyFinishedInstruction(Instruction<CONFIG> *instruction, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_FINISHED_INSN;
	ev->object.instruction = instruction;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyOperandAvailability(Operand<CONFIG> *operand, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_AVAILABLE_OPERAND;
	ev->object.operand = operand;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyFinishedFetch(FetchAccess<CONFIG> *fetch_access, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_FINISHED_FETCH;
	ev->object.fetch_access = fetch_access;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyLoadResultAvailability(LoadStoreAccess<CONFIG> *load_store_access, uint64_t latency)
{
	Instruction<CONFIG> *instruction = load_store_access->instruction;

	unsigned int i;
	unsigned int num_load_store_accesses = instruction->load_store_access_queue.Size();
	uint8_t data[8];

	for(i = 0; i < num_load_store_accesses; i++)
	{
		if(!load_store_access->valid) return;
		memcpy(data + load_store_access->offset, load_store_access->data, load_store_access->size);
	}

	switch(load_store_access->type)
	{
		case LoadStoreAccess<CONFIG>::INT8_LOAD:
			{
				uint8_t value = *(uint8_t *) data;
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) value, latency); // 8-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) BigEndian2Host(value), latency); // 16-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::SINT16_LOAD:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) (int16_t) BigEndian2Host(value), latency); // 16-bit to 32-bit sign extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, BigEndian2Host(value), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::FP32_LOAD:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				Flags flags;
				flags.setRoundingMode(RN_ZERO);
				load_store_access->instruction->SetFPR(load_store_access->reg_num, SoftDouble(SoftFloat(BigEndian2Host(value)), flags), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::FP64_LOAD:
			{
				uint64_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetFPR(load_store_access->reg_num, SoftDouble(BigEndian2Host(value)), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) LittleEndian2Host(value), latency); // reverse bytes and 16-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) LittleEndian2Host(value), latency); // reverse bytes
			}
			break;
		case LoadStoreAccess<CONFIG>::INT_LOAD_MSB:
			switch(load_store_access->size)
			{
				case 1:
				{
					uint8_t value = *data;
					load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) value << 24, latency);
					break;
				}
		
				case 2:
				{
					uint16_t value;
					memcpy(&value, data, sizeof(value));
					load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) BigEndian2Host(value) << 16, latency);
					break;
				}
		
				case 3:
				{
					uint8_t *buffer = data;
					load_store_access->instruction->SetGPR(load_store_access->reg_num, ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8), latency);
					break;
				}
		
				case 4:
				{
					uint32_t value;
					memcpy(&value, data, sizeof(value));
					load_store_access->instruction->SetGPR(load_store_access->reg_num, BigEndian2Host(value), latency);
					break;
				}
			}
			break;
		default:
			logger << DebugError << "Unhandled load/store access type" << EndDebugError;
			Stop(-1);
	}

	NotifyFinishedInstruction(load_store_access->instruction, 1);
}

template <class CONFIG>
void CPU<CONFIG>::Complete()
{
	if(cq.Empty()) return;

	unsigned int i;
	unsigned int num_gpr_wb_ports = 0;
	unsigned int num_fpr_wb_ports = 0;
	unsigned int num_cr_wb_ports = 0;
	unsigned int num_lr_wb_ports = 0;
	unsigned int num_ctr_wb_ports = 0;
	for(i = 0; i < CONFIG::COMPLETE_WIDTH; i++)
	{
		// Lookup the last instruction from the instruction queue
		Instruction<CONFIG> *instruction = cq.Front();

		if(!instruction->IsFinished())
		{
			if(!instruction->IsAllowedToExecute() && (instruction->GetSerialization() & CONFIG::EXECUTION_SERIALIZATION))
			{
				instruction->AllowExecution();
			}
			return;
		}

		// Check exception flags

		// check for store
		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			
		}

		// Check write back constraints
		if(wb0.Full()) return;

		unsigned int num_output_gpr = instruction->GetNumOutputGPR();
		num_gpr_wb_ports += num_output_gpr;
		if(num_gpr_wb_ports > CONFIG::MAX_GPR_WB_PER_CYCLE) return;

		unsigned int num_output_fpr = instruction->GetNumOutputFPR();
		num_fpr_wb_ports += num_output_fpr;
		if(num_fpr_wb_ports > CONFIG::MAX_FPR_WB_PER_CYCLE) return;

		unsigned int num_output_cr = instruction->GetNumOutputCR();
		num_cr_wb_ports += num_output_cr;
		if(num_cr_wb_ports > CONFIG::MAX_CR_WB_PER_CYCLE) return;

		unsigned int num_output_lr = instruction->GetNumOutputLR();
		num_lr_wb_ports += num_output_lr;
		if(num_lr_wb_ports > CONFIG::MAX_LR_WB_PER_CYCLE) return;

		unsigned int num_output_ctr = instruction->GetNumOutputCTR();
		num_ctr_wb_ports += num_output_ctr;
		if(num_ctr_wb_ports > CONFIG::MAX_CTR_WB_PER_CYCLE) return;
	
		// Instruction Retire
		wb0.Push(instruction);
		cq.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::WriteBack0()
{
	while(!wb0.Empty())
	{
		Instruction<CONFIG> *instruction = wb0.Front();

		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			if(wb1.Full()) return;
		}

		unsigned int num_output_operands = instruction->output_operands.Size();
		unsigned int j;

		for(j = 0; j < num_output_operands; j++)
		{
			Operand<CONFIG> *operand = instruction->output_operands[j];

			switch(operand->type)
			{
				case Operand<CONFIG>::GPR:
					FreeOperand(arch_gpr[operand->reg_num]);
					arch_gpr[operand->reg_num] = operand;
					rename_gpr[~operand->tag] = 0;
					gpr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = operand->reg_num;
					break;
				case Operand<CONFIG>::FPR:
					FreeOperand(arch_fpr[operand->reg_num]);
					arch_fpr[operand->reg_num] = operand;
					rename_fpr[~operand->tag] = 0;
					fpr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = operand->reg_num;
					break;
				case Operand<CONFIG>::CR:
					FreeOperand(arch_cr);
					arch_cr = operand;
					rename_cr[~operand->tag] = 0;
					cr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::LR:
					FreeOperand(arch_lr);
					arch_lr = operand;
					rename_lr[~operand->tag] = 0;
					lr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::CTR:
					FreeOperand(arch_ctr);
					arch_ctr = operand;
					rename_ctr[~operand->tag] = 0;
					ctr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::XER:
					FreeOperand(arch_xer);
					arch_xer = operand;
					break;
				case Operand<CONFIG>::FPSCR:
					FreeOperand(arch_fpscr);
					arch_fpscr = operand;
					break;
			}
		}
		
		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			wb1.Push(instruction);
		}
		else
		{
			FreeInstruction(instruction);
		}
		wb0.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::WriteBack1()
{
	while(!wb1.Empty())
	{
		Instruction<CONFIG> *instruction = wb1.Front();

		if(csq.Full()) return;

		csq.Push(instruction);
		fsq.Pop();
		wb1.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Flush()
{
	unsigned int i;

	if(!iq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = iq.Front();
			FreeInstruction(instruction);
			iq.Pop();
		} while(!iq.Empty());
	}

	if(!giq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = giq.Front();
			FreeInstruction(instruction);
			giq.Pop();
		} while(!giq.Empty());
	}

	if(!fiq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = fiq.Front();
			FreeInstruction(instruction);
			fiq.Pop();
		} while(!fiq.Empty());
	}

	if(!viq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = viq.Front();
			FreeInstruction(instruction);
			viq.Pop();
		} while(!viq.Empty());
	}

	for(i = 0; i < CONFIG::NUM_IU1; i++)
	{
		if(!iu1_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = iu1_reservation_station[i].Front();
				FreeInstruction(instruction);
				iu1_reservation_station[i].Pop();
			} while(!iu1_reservation_station[i].Empty());
		}
	}

	for(i = 0; i < CONFIG::NUM_IU2; i++)
	{
		if(!iu2_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = iu2_reservation_station[i].Front();
				FreeInstruction(instruction);
				iu2_reservation_station[i].Pop();
			} while(!iu2_reservation_station[i].Empty());
		}
	}

	for(i = 0; i < CONFIG::NUM_FPU; i++)
	{
		if(!fpu_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = fpu_reservation_station[i].Front();
				FreeInstruction(instruction);
				fpu_reservation_station[i].Pop();
			} while(!fpu_reservation_station[i].Empty());
		}
	}

	if(!lsu_reservation_station.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = lsu_reservation_station.Front();
			FreeInstruction(instruction);
			lsu_reservation_station.Pop();
		} while(!lsu_reservation_station.Empty());
	}

	if(!cq.Empty())
	{
		do
		{
			//Instruction<CONFIG> *instruction = cq.Front();

			cq.Pop();
		} while(!cq.Empty());
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpPipeline()
{
	stringstream sstr;
	
	sstr << iq;
	
	logger << DebugInfo;
	logger << sstr.str();
	logger << EndDebugInfo;
}

template <class CONFIG>
void CPU<CONFIG>::StepOneCycle()
{
	DumpPipeline();
	DumpSchedule();
	//logger << DebugInfo << "Cycle #" << cpu_cycle << EndDebugInfo;
	ScheduleEvents();

	// Complete
	Complete();
	// Execute
	IU1Execute();
	IU2Execute();
	FPUExecute();
	BIU();
	LSUExecute2();
	LSUExecute1();
	// Issue
	GPRIssue();
	VRIssue();
	FPRIssue();
	// Decode/Dispatch
	DecodeDispatch();
	// Fetch
	Fetch();
	cpu_cycle++;
}

template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, typename CONFIG::address_t cia, uint32_t encoding)
{
	this->operation = cpu->Decode(cia, encoding);
	operation->resolve(cpu, 0);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = 0;
	//Instruction<CONFIG> *macro_op;
	num_input_gpr = 0;
	num_input_fpr = 0;
	num_input_cr = 0;
	num_input_lr = 0;
	num_input_ctr = 0;
	input_operands.Clear();
	forwarding.Clear();
	output_operands.Clear();
}

template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, const Instruction<CONFIG>& instruction, unsigned int uop_num)
{
	this->operation = instruction.operation;
	operation->resolve(cpu, uop_num);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = uop_num;
	input_operands.Clear();
	forwarding.Clear();
	output_operands.Clear();
}

template <class CONFIG>
bool Instruction<CONFIG>::CheckInputOperandsAvailability()
{
	if(ready_to_execute) return true;

	unsigned int num_input_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_input_operands; i++)
	{
		if(forwarding[i] && !input_operands[i]->valid) return false;
	}

	ready_to_execute = true;
	return true;
}

/*
template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, unsigned int uop_num)
{
	unsigned int i;

	operation->resolve(cpu, uop_num);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = uop_num;
	num_input_operands = 0;
	num_ready_input_operands = 0;

	for(i = 0; i < operation->num_insn_operands; i++)
	{
		typename CONFIG::operand_t *operand = &operation->insn_operands[i];

		switch(operand->dir)
		{
			case CONFIG::INPUT_T:
				switch(operand->type)
				{
					case CONFIG::GPR_T:
						input_gpr[num_input_gpr].reg_num = operand->extra.reg_num;
						num_input_gpr++;
						num_input_operands++;
						break;

					case CONFIG::FPR_T:
						input_fpr[num_input_fpr].reg_num = operand->extra.reg_num;
						num_input_fpr++;
						num_input_operands++;
						break;
		
					case CONFIG::CRF_T:
						has_input_cr = true;
						num_input_operands++;
						break;
		
					case CONFIG::LR_T:
						has_input_lr = true;
						num_input_operands++;
						break;
		
					case CONFIG::CTR_T:
						has_input_ctr = true;
						num_input_operands++;
						break;
				}
				break;

			case CONFIG::OUTPUT_T:
				switch(operand->type)
				{
					case CONFIG::GPR_T:
						output_gpr[num_output_gpr].reg_num = operand->extra.reg_num;
						num_output_gpr++;
						break;
		
					case CONFIG::FPR_T:
						output_fpr[num_output_fpr].reg_num = operand->extra.reg_num;
						num_output_fpr++;
						break;
		
					case CONFIG::CRF_T:
						has_output_cr = true;
						break;
		
					case CONFIG::LR_T:
						has_output_lr = true;
						break;
		
					case CONFIG::CTR_T:
						has_output_ctr = true;
						break;
				}
				break;
		}
	}

	cycle = 0;
	stage = FETCH;
	finished = false;
	allowed_to_execute = (serialization == CONFIG::EXECUTION_SERIALIZATION) ? false : true;
	ready_to_execute = false;
}

template <class CONFIG>
void Instruction<CONFIG>::SetInputGPRValue(unsigned int i, uint32_t value)
{
	input_gpr[i].value = value;
	input_gpr[i].ready = true;
	num_ready_input_operands++;
	ready_to_execute = (num_ready_input_operands >= num_input_operands);
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetGPR(unsigned int n)
{
	unsigned int i;
	for(i = 0; i < num_input_gpr; i++)
	{
		if(input_gpr[i].reg_num == n) return input_gpr[i].value;
	}
	assert(false);
}

*/

template <class CONFIG>
const Operand<CONFIG> *Instruction<CONFIG>::SearchInputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num) const
{
	unsigned int num_input_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_input_operands; i++)
	{
		const Operand<CONFIG> *operand = input_operands[i];
		if(operand->type == type && operand->reg_num == reg_num)
		{
			return operand;
		}
	}
	return 0;
}

template <class CONFIG>
Operand<CONFIG> *Instruction<CONFIG>::SearchOutputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num)
{
	unsigned int num_output_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_output_operands; i++)
	{
		Operand<CONFIG> *operand = input_operands[i];
		if(operand->type == type && operand->reg_num == reg_num)
		{
			return operand;
		}
	}
	return 0;
}

template <class CONFIG>
void Instruction<CONFIG>::SetGPR(unsigned int n, uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::GPR, n);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetFPR(unsigned int n, const SoftDouble& value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::FPR, n);
	assert(operand != 0);
	operand->float_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetCR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::CR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetLR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::LR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetCTR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::CTR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetXER(uint32_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
	{
		Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::XER);
		assert(operand != 0);
		operand->int_value = value;
	}
	else
	{
		cpu->SetXER(value);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::SetFPSCR(uint32_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
	{
		Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::FPSCR);
		assert(operand != 0);
		operand->int_value = value;
	}
	else
	{
		cpu->SetFPSCR(value);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::SetNIA(typename CONFIG::address_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		nia = value;
	else
		cpu->SetNIA(value);
}


template <class CONFIG>
uint32_t Instruction<CONFIG>::GetGPR(unsigned int n) const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::GPR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
const SoftDouble& Instruction<CONFIG>::GetFPR(unsigned int n) const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::FPR);
	assert(operand != 0);
	return operand->float_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetCR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::CR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetLR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::LR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetCTR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::CTR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
void Instruction<CONFIG>::Int8Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenSocLibLoadStore(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, ea, 1);
#else
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, ea, 1, this);
	else
		cpu->Int8Load(rd, ea);
#endif
}

template <class CONFIG>
void Instruction<CONFIG>::Int16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_LOAD, rd, ea, 2, this);
	else
		cpu->Int16Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::SInt16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::SINT16_LOAD, rd, ea, 2, this);
	else
		cpu->SInt16Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_LOAD, rd, ea, 4, this);
	else
		cpu->Int32Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp32Load(unsigned int fd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP32_LOAD, fd, ea, 4, this);
	else
		cpu->Fp32Load(fd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp64Load(unsigned int fd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP64_LOAD, fd, ea, 8, this);
	else
		cpu->Fp64Load(fd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE, rd, ea, 2, this);
	else
		cpu->Int16LoadByteReverse(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE, rd, ea, 4, this);
	else
		cpu->Int32LoadByteReverse(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT_LOAD_MSB, rd, ea, size, this);
	else
		cpu->IntLoadMSBFirst(rd, ea, size);
}

template <class CONFIG>
void Instruction<CONFIG>::Int8Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT8_STORE, rs, ea, 1, this);
	else
		cpu->Int8Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_STORE, rs, ea, 2, this);
	else
		cpu->Int16Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_STORE, rs, ea, 4, this);
	else
		cpu->Int32Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp32Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP32_STORE, fs, ea, 4, this);
	else
		cpu->Fp32Store(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp64Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP64_STORE, fs, ea, 8, this);
	else
		cpu->Fp64Store(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP_STORE_LSW, fs, ea, 4, this);
	else
		cpu->FpStoreLSW(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE, rs, ea, 2, this);
	else
		cpu->Int16StoreByteReverse(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE, rs, ea, 4, this);
	else
		cpu->Int32StoreByteReverse(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT_STORE_MSB, rs, ea, size, this);
	else
		cpu->IntStoreMSBFirst(rs, ea, size);
}

template <class CONFIG>
void Instruction<CONFIG>::Execute(CPU<CONFIG> *cpu)
{
	operation->uop_execute(cpu, uop_num);
}

template <class CONFIG>
MappingTable<CONFIG>::MappingTable()
{
	unsigned int reg_num;

	for(reg_num = 0; reg_num < CONFIG::NUM_LOGICAL_REGISTERS; reg_num++)
	{
		mapping_table[reg_num] = reg_num;
	}

	int tag;

	// WARNING! tags are negative integers (2's complement)
	// WARNING! tag 0 is unused as it refers to logical register 0
	// WARNING! 'tag' refers to rename register number '~tag'
	for(tag = -1; tag >= -((int) CONFIG::NUM_RENAME_REGISTERS); tag--)
	{
		free_list.Push(tag);
	}
}

template <class CONFIG>
MappingTable<CONFIG>::~MappingTable()
{
}

template <class CONFIG>
bool MappingTable<CONFIG>::Allocate(unsigned int reg_num, int& tag)
{
	if(free_list.Empty()) return false;

	tag = mapping_table[reg_num] = free_list.Front();
	free_list.Pop();

	return true;
}

template <class CONFIG>
int MappingTable<CONFIG>::Lookup(unsigned int reg_num)
{
	return mapping_table[reg_num];
}

template <class CONFIG>
void MappingTable<CONFIG>::Free(unsigned int reg_num, int tag)
{
	if(mapping_table[reg_num] == tag)
	{
		mapping_table[reg_num] = reg_num;
	}

	free_list.Push(tag);
}

template <class CONFIG>
unsigned int MappingTable<CONFIG>::GetNumFreeRenameRegisters() const
{
	return free_list.Size();
}

template <class CONFIG>
ostream& operator << (ostream& os, const FetchAccess<CONFIG>& fetch_access)
{
	os << "FETCH_ACCESS(";
	os << "cia=0x" << std::hex << fetch_access.cia << std::dec;
	os << ", size=" << fetch_access.size;
	os << ", " << fetch_access.mmu_access;
	os << ", L1_" << fetch_access.l1_access;
	os << ", L2_" << fetch_access.l2_access;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const MMUAccess<CONFIG>& mmu_access)
{
	os << "MMU_ACCESS(";
	os << "addr=0x" << std::hex << mmu_access.addr << std::dec;
	os << ", privilege_level=";
	switch(mmu_access.privilege_level)
	{
		case CONFIG::PR_SUPERVISOR:
			os << "supervisor";
			break;
		case CONFIG::PR_USER:
			os << "user";
			break;
	}
	os << ", memory_access_type=";
	switch(mmu_access.memory_access_type)
	{
		case CONFIG::MAT_READ:
			os << "read";
			break;
		case CONFIG::MAT_WRITE:
			os << "write";
			break;
	}
	os << ", memory_type=";
	switch(mmu_access.memory_type)
	{
		case CONFIG::MT_INSN:
			os << "insn";
			break;
		case CONFIG::MT_DATA:
			os << "data";
			break;
	}
	os << ", bepi=" << std::hex << mmu_access.bepi << std::dec;
	os << ", sr_num=" << mmu_access.sr_num;
	os << ", virtual_segment_id=" << mmu_access.virtual_segment_id;
	os << ", sr_ks=" << mmu_access.sr_ks;
	os << ", sr_kp=" << mmu_access.sr_kp;
	os << ", sr_noexecute=" << mmu_access.sr_noexecute;
	os << ", virtual_addr=0x" << std::hex << mmu_access.virtual_addr << std::dec;
	os << ", base_virtual_addr=0x" << std::hex << mmu_access.base_virtual_addr << std::dec;
	os << ", tlb_index=" << mmu_access.tlb_index;
	os << ", tlb_way=" << mmu_access.tlb_way;
	os << ", key=" << mmu_access.key;
	os << ", force_page_table_walk=" << mmu_access.force_page_table_walk;
	os << ", page_index=0x" << std::hex << mmu_access.page_index << std::dec;
	os << ", api=0x" << std::hex << mmu_access.api << std::dec;
	os << ", bat_hit=" << mmu_access.bat_hit;
	os << ", tlb_hit=" << mmu_access.tlb_hit;
	os << ", physical_addr=0x" << std::hex << mmu_access.physical_addr << std::dec;
	os << ", protection_boundary=0x" << std::hex << mmu_access.protection_boundary << std::dec;
	os << ", wimg=";
	os << ((mmu_access.wimg & CONFIG::WIMG_WRITE_THROUGH) ? "W" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) ? "I" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? "M" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_GUARDED_MEMORY) ? "G" : "x");
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const CacheAccess<CONFIG>& cache_access)
{
	os << "CACHE_ACCESS(";
	os << "addr=0x" << std::hex << cache_access.addr << std::dec;
	os << ",line_base_addr=0x" << std::hex << cache_access.line_base_addr << std::dec;
	os << ",block_base_addr=0x" << std::hex << cache_access.block_base_addr << std::dec;
	os << ", way=" << cache_access.way;
	os << ", sector=" << cache_access.sector;
	os << ", offset=" << cache_access.offset;
	os << ", size_to_block_boundary=" << cache_access.size_to_block_boundary;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const LoadStoreAccess<CONFIG>& load_store_access)
{
	os << "LOAD_STORE_ACCESS(";
	os << "type=";
	switch(load_store_access.type)
	{
		case LoadStoreAccess<CONFIG>::INT8_LOAD: os << "int8_load"; break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD: os << "int16_load"; break;
		case LoadStoreAccess<CONFIG>::SINT16_LOAD: os << "sint16_load"; break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD: os << "int32_load"; break;
		case LoadStoreAccess<CONFIG>::FP32_LOAD: os << "fp32_load"; break;
		case LoadStoreAccess<CONFIG>::FP64_LOAD: os << "fp64_load"; break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE: os << "int16_load_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE: os << "int32_load_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT_LOAD_MSB: os << "int_load_msb"; break;
		case LoadStoreAccess<CONFIG>::INT8_STORE: os << "int8_store"; break;
		case LoadStoreAccess<CONFIG>::INT16_STORE: os << "int16_store"; break;
		case LoadStoreAccess<CONFIG>::INT32_STORE: os << "int32_store"; break;
		case LoadStoreAccess<CONFIG>::FP32_STORE: os << "fp32_store"; break;
		case LoadStoreAccess<CONFIG>::FP64_STORE: os << "fp64_store"; break;
		case LoadStoreAccess<CONFIG>::FP_STORE_LSW: os << "fp_store_lsw"; break;
		case LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE: os << "int16_store_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE: os << "int32_store_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT_STORE_MSB: os << "int_store_msb"; break;
	}
	os << ", reg_num=" << load_store_access.reg_num;
	os << ", munged_ea=0x" << std::hex << load_store_access.munged_ea << std::dec;
	os << ", offset=" << load_store_access.offset;
	os << ", size=" << load_store_access.size;
	os << ", valid=" << load_store_access.valid;
	os << ", data=[" << std::hex;
	int i;
	for(i = 0; i < sizeof(load_store_access.data); i++)
	{
		if(i != 0) os << " ";
		os << "0x" << (unsigned int) load_store_access.data[i];
	}
	os << "]" << std::dec;
	os << ", " << load_store_access.mmu_access;
	os << ", L1_" << load_store_access.l1_access;
	os << ", L2_" << load_store_access.l2_access;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const BusAccess<CONFIG>& bus_access)
{
	os << "BUS_ACCESS(";
	os << "issued=" << bus_access.issued;
	os << ", type=";
	switch(bus_access.type)
	{
		case BusAccess<CONFIG>::LOAD: os << "load"; break;
		case BusAccess<CONFIG>::REFILL: os << "refill"; break;
		case BusAccess<CONFIG>::REFILLX: os << "refillx"; break;
		case BusAccess<CONFIG>::STORE: os << "store"; break;
		case BusAccess<CONFIG>::EVICTION: os << "eviction"; break;
	}
	os << ", addr=0x" << std::hex << bus_access.addr << std::dec;
	os << ", size=" << bus_access.size;
	os << ", storage=[";
	unsigned int i;
	for(i = 0; i < bus_access.size; i++)
	{
		if(i != 0) os << " ";
		os << "0x" << std::hex << (unsigned int) bus_access.storage[i] << std::dec;
	}
	os << "]";
	os << ", wimg=";
	os << ((bus_access.wimg & CONFIG::WIMG_WRITE_THROUGH) ? "W" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) ? "I" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? "M" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_GUARDED_MEMORY) ? "G" : "x");
	os << ", rwitm=" << bus_access.rwitm;
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const Operand<CONFIG>& operand)
{
	os << "OPERAND(";
	os << "type=";
	switch(operand.type)
	{
		case Operand<CONFIG>::GPR: os << "GPR"; break;
		case Operand<CONFIG>::FPR: os << "FPR"; break;
		case Operand<CONFIG>::CR: os << "CR"; break;
		case Operand<CONFIG>::LR: os << "LR"; break;
		case Operand<CONFIG>::CTR: os << "CTR"; break;
		case Operand<CONFIG>::XER: os << "XER"; break;
		case Operand<CONFIG>::FPSCR: os << "FPSCR"; break;
	}
	os << ", valid=" << operand.valid;
	os << ", reg_num=" << operand.reg_num;
	os << ", tag=" << operand.tag;
	os << ", int_value=0x" << std::hex << operand.int_value << std::dec;
	os << ", float_value=0x" << operand.float_value;
	os << ", ref_count=" << operand.ref_count;
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const Instruction<CONFIG>& instruction)
{
	os << "INSTRUCTION(";
	os << ")";
	return os;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
