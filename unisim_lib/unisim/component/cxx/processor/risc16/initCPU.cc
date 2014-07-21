#include <unisim/component/cxx/processor/risc16/initCPU.hh>

#include <unisim/component/cxx/processor/risc16/risc16_isa.cc>


uint16_t save_nia;
Decoder decoder;

CPU::CPU(const char* name, Object *_parent):
				Object(name, _parent, "")

, unisim::kernel::service::Client<DebugControl<PHYSICAL_ADDR> >(name, _parent)
, unisim::kernel::service::Client<MemoryAccessReporting<PHYSICAL_ADDR> >(name, _parent)
, Service<MemoryAccessReportingControl>(name, _parent)
, Service<Memory<PHYSICAL_ADDR> > (name, _parent)
, Client<Memory<PHYSICAL_ADDR> > (name, _parent)
, Service<Disassembly<PHYSICAL_ADDR> >(name, _parent)

, memory_export("memory-export", this)
,  memory_access_reporting_control_export("memory-access-reporting-control-export", this)

, debug_control_import("debug-control-import", this)
, memory_access_reporting_import("memory-access-reporting-import", this)
, memory_import("memory-import", this)
, trap_reporting_import("trap_reporting_import", this)
, disasm_export("disasm_export", this)
, registers_export("registers_export", this)

, requires_memory_access_reporting(false)
, param_requires_memory_access_reporting("requires-memory-access-reporting", this, requires_memory_access_reporting)
, requires_finished_instruction_reporting(false)
, param_requires_finished_instruction_reporting("requires-finished-instruction-reporting", this, requires_finished_instruction_reporting)

,cia(0)
,nia(0)
,load_address(0x800)
,param_load_address("load-address", this, load_address)

, entry_point(load_address + 4)
, param_entry_point ("entry-point", this, entry_point)

,received_vect(0)
,has_interrupt(false)

{
	logger = new unisim::kernel::logger::Logger(*this);

	for (int i= 0; i < 16; i++) gpr[i] = 0;

	tracefile.open("trace.txt", ios::out);

};

CPU::~CPU()
{
	unsigned int n = extended_registers_registry.size();
	for (unsigned int i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}

	tracefile.close();
};

int CPU::generateBinary(const char* fname)
{
	std::ofstream fos(fname, ios::out | ios::binary);

	OpLoad* Load = new OpLoad(0,0);
	OpStore* Store = new OpStore(0,0);
	OpShlx* shlx = new OpShlx(0, 0);
	OpOr* or1 = new OpOr(0 , 0);
	OpBne* bne = new OpBne(0,0);
	OpLoadi* Ldi = new OpLoadi(0,0);
	OpRti* RTI = new OpRti(0,0);

// ISR code
	Load->rd = 0;
	Load->base = load_address;
	Load->immed = 100; //ofsset quelquonc
	Load->binary(fos);

	RTI->binary(fos);

// main code
	Load->rd = 0;
	Load->base = 0;
	Load->immed = 1;
	Load->binary(fos);

	// -- programmation du vecteur d'interruption (ISR)
	// ---- address du vecteur
	Ldi->rd = 0;
	Ldi->val = 0xff;
	Ldi->binary(fos);

	shlx->rd = 0;
	shlx->rs = 0;
	shlx->val = 8;
	shlx->binary(fos);

	Ldi->rd = 2;
	Ldi->val = 0x20;
	Ldi->binary(fos);

	or1->rd = 0;
	or1->rs1 = 0;
	or1->rs2 = 2;
	or1->binary(fos);

	// ---- addresse de l'ISR
	Ldi->rd = 1;
	Ldi->val = 0x08;
	Ldi->binary(fos);

	shlx->rd = 1;
	shlx->rs = 1;
	shlx->val = 8;
	shlx->binary(fos);

	Ldi->rd = 2;
	Ldi->val = 0x00;
	Ldi->binary(fos);

	or1->rd = 1;
	or1->rs1 = 1;
	or1->rs2 = 2;
	or1->binary(fos);

	// ---- ecriture de l'addresse de l'ISR dans le vecteur
	Store->rd = 1;
	Store->base = 0;
	Store->immed = 0;
	Store->binary(fos);

	Ldi->rd = 15;
	Ldi->val = 1;
	Ldi->binary(fos);

	// -- boucle infinie
	bne->rs = 15;
	bne->offset = 0;
	bne->binary(fos);

	fos.close();


	return 0;
}
// ***************************************

void CPU::mem_loader(const char* fname,PHYSICAL_ADDR addr)
{
	FILE *f;
	int n;
	uint8_t mem[32];


	f = fopen(fname, "rb");

	n = fread(mem, 1, 32, f);

	if (memory_import) {
		memory_import->WriteMemory(addr, mem, n);
	}

	fclose(f);

	if (n < 0)
	{
		std::cout << "Error lors du chargement de pgm" << std::endl;
		exit(-1);
	}
	else
	{
		std::cout << "load success size=" << n << std::endl;
//		for (int i=0; i< n; i++) {
//			std::cout << (bitset<8>) mem[i] << std::endl;
//		}
	}

}

void CPU::rti() {
	// restaure context saved
	nia = save_nia;
	std::cout << "return to normal execution" << std::endl;
}

void CPU::handleinterrupt() {
	// determiner le vecteu
	// sauve context
	// bascule vers isr



	save_nia = nia;

	tracefile << "received vect " << std::hex << received_vect << std::dec << std::endl;

	cia = mem_read16(received_vect);
	tracefile <<"handling the interruption by CPU." << std::endl;
	tracefile << "ISR address 0x " << std::hex << cia << std::endl;

	has_interrupt = false;

}

int CPU::Step()
{

	if(debug_control_import) {
		DebugControl<PHYSICAL_ADDR>::DebugCommand dbg_cmd;

		do {

			dbg_cmd = debug_control_import->FetchDebugCommand(cia);

			if(dbg_cmd == DebugControl<PHYSICAL_ADDR>::DBG_STEP) {
				break;
			}
			if(dbg_cmd == DebugControl<PHYSICAL_ADDR>::DBG_SYNC) {
				continue;
			}

			if(dbg_cmd == DebugControl<PHYSICAL_ADDR>::DBG_KILL) {
				Stop(0);
			}
			if(dbg_cmd == DebugControl<PHYSICAL_ADDR>::DBG_RESET) {

			}
		} while(1);
	}

	if (requires_memory_access_reporting) {
		if(memory_access_reporting_import) {
			memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, cia, MAX_INS_SIZE);
		}
	}

	uint16_t instruction;
	Operation *operation;
	char disasm_buffer[265];


	if (has_interrupt)
	{
		handleinterrupt();
	}

	instruction = fetch(cia);
	operation = decoder.Decode(cia, instruction);

	operation->disasm(cia, disasm_buffer);
	tracefile << "0x" << std::hex << cia << " : 0x" << std::hex << instruction << "\t\t" << disasm_buffer << std::endl;
//	printf("0x%04x: 0x%04x %s\n", cia, instruction, disasm_buffer);

	nia = cia+2;

//	std::cout << "*** GPR *** " << std::endl;
//	for (int i= 0; i < 16; i++) std::cout << "gpr_" << std::dec << i << "  " << std::hex << gpr[i] << std::dec <<  " ";
//	std::cout << std::endl;

	operation->execute(this);

	if(requires_finished_instruction_reporting) {
		if(memory_access_reporting_import) {
			memory_access_reporting_import->ReportFinishedInstruction(cia, nia);

		}
	}

	cia = nia;

	return 0;

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::BeginSetup() {
//	const char* fname = "tt.bin";
//
//	generateBinary(fname);
//	mem_loader(fname, load_address );

	cia = entry_point;

	stringstream strm;
	string desc = "General Purpose Register ";
	for (int i= 0; i < 16; i++) {
		strm << "gpr[" << i << "]";
		string name = strm.str();
		extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>(name.c_str(), this, gpr[i], desc.append(name).c_str()));
		strm.str(string());
	}


	return (true);
}

bool CPU::Setup(ServiceExportBase *srv_export) {
	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	return (true);

}

bool CPU::EndSetup() {
	return (true);
}

void CPU::OnDisconnect() {

}


//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

void CPU::RequiresMemoryAccessReporting(bool report) {
	requires_memory_access_reporting = report;
}

void CPU::RequiresFinishedInstructionReporting(bool report) {
	requires_finished_instruction_reporting = report;
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

void CPU::Reset() {

}

bool CPU::ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size) {
	if (memory_import) {
		return (memory_import->ReadMemory(addr, (uint8_t *) buffer, size));
	}

	return (false);
}

bool CPU::WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size) {
	if (memory_import) {
		return (memory_import->WriteMemory(addr, (uint8_t *) buffer, size));
	}

	return (false);
}

/**
 * Returns a string with the disassembling of the instruction found
 *   at address addr.
 *
 * @param addr The address of the instruction to disassemble.
 * @param next_addr The address following the requested instruction.
 * @return The disassembling of the requested instruction address.
 */
string CPU::Disasm(PHYSICAL_ADDR service_addr, PHYSICAL_ADDR &next_addr)
{
	uint16_t instruction;

	ReadMemory(service_addr, &instruction, MAX_INS_SIZE);

	Operation *operation = decoder.Decode(service_addr, instruction);

	char disasm_buffer[265];
	operation->disasm(service_addr, disasm_buffer);

	unsigned int insn_length = operation->GetLength();

	if (insn_length % 8) throw "InternalError";

	next_addr = service_addr + (insn_length/8);

	return disasm_buffer;
}








