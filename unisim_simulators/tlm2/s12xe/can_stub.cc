/*
 *  Copyright (c) 2008, 2015
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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */



#include "can_stub.hh"

CAN_STUB::CAN_STUB(const sc_module_name& name, Object *parent) :
	Object(name)
	, sc_module(name)
	, can_rx_sock("can_rx_sock")
	, can_tx_sock("can_tx_sock")

	, can_rx_stimulus_period(80000000)
	, can_rx_stimulus_period_sc(NULL)

	, can_tx_fetch_period(1e9) // 1 ms
	, can_tx_fetch_period_sc(0)

	, trace_enable(false)
	, param_trace_enable("trace-enabled", this, trace_enable)

	, cosim_enabled(false)
	, param_cosim_enabled("cosim-enabled", this, cosim_enabled)

	, can_rx_stub_enabled(false)
	, param_can_rx_stub_enabled("can-rx-stub-enabled", this, can_rx_stub_enabled)

	, input_payload_queue("input_payload_queue")

	, param_can_rx_stimulus_period("can-rx-stimulus-period", this, can_rx_stimulus_period)
	, param_can_tx_fetch_period("can-tx-fetch-period", this, can_tx_fetch_period)

	, can_rx_stimulus_file("")
	, param_can_rx_stimulus_file("can-rx-stimulus-file", this, can_rx_stimulus_file)

	, terminated(false)


{
	can_rx_sock(*this);
	can_tx_sock(*this);

	SC_HAS_PROCESS(CAN_STUB);

	SC_THREAD(processCANRX);
	SC_THREAD(processCANTX);

}

CAN_STUB::~CAN_STUB() {

	if (trace_enable) {
		can_rx_output_file.close();
		can_tx_output_file.close();
	}

	if (can_rx_stimulus_period_sc) { delete can_rx_stimulus_period_sc; can_rx_stimulus_period_sc = NULL; }
	if (can_tx_fetch_period_sc) { delete can_tx_fetch_period_sc; can_tx_fetch_period_sc = NULL; }
}

bool CAN_STUB::BeginSetup() {

	if (cosim_enabled) {
		CAN_DATATYPE* data0  = new CAN_DATATYPE();
		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
			data0->ID[j] = 0;
		}
		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
			data0->Data[j] = 0;
		}
		data0->Length = 0;
		data0->Priority = 0;
		data0->Extended = 0;
		data0->Error = 0;
		data0->Remote = 0;
		data0->Timestamp[0] = 0;
		data0->Timestamp[1] = 0;
		can_rx_vect.push_back(data0);

	}
	else {
		if (can_rx_stub_enabled) {
			LoadXmlData(can_rx_stimulus_file.c_str(), can_rx_vect);
		} else {
			RandomizeData(can_rx_vect);
		}

	}

	if (trace_enable) {
//		can_rx_output_file.open ("can_rx_output.txt");
//		can_tx_output_file.open ("can_tx_output.txt");

		stringstream strm;
		strm << sc_object::name() << "_rx_output.txt";
		can_rx_output_file.open (strm.str().c_str());
		strm.str(string());
		strm << sc_object::name() << "_tx_output.txt";
		can_tx_output_file.open (strm.str().c_str());
		strm.str(string());

	}

	tlm_quantumkeeper::set_global_quantum(sc_time(0.0, SC_MS));

	can_rx_stimulus_period_sc = new sc_time(can_rx_stimulus_period, SC_PS);
	can_tx_fetch_period_sc = new sc_time(can_tx_fetch_period, SC_PS);

	return (true);
}

bool CAN_STUB::Setup(ServiceExportBase *srv_export) {
	return (true);
}

bool CAN_STUB::EndSetup() {
	return (true);
}

void CAN_STUB::Stop(int exit_status) {
	terminated = true;
}

// Slave methods
bool CAN_STUB::get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data)
{
	// Leave this empty as it is designed for memory mapped buses
	return (false);
}

unsigned int CAN_STUB::transport_dbg( CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum CAN_STUB::nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_REQ)
	{
		phase = END_REQ; // update the phase
		payload.acquire();

		input_payload_queue.notify(payload, t); // queue the payload and the associative time
		return (TLM_UPDATED);
	}

	// we received an unexpected phase, so we return TLM_ACCEPTED
	return (TLM_ACCEPTED);
}

void CAN_STUB::b_transport( CAN_Payload& payload, sc_core::sc_time& t)
{
	payload.acquire();
	input_payload_queue.notify(payload, t);
}

// Master methods
tlm_sync_enum CAN_STUB::nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		//payload.release();
		can_bw_event.notify();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

void CAN_STUB::invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

// Implementation
void CAN_STUB::observe(CAN_DATATYPE &msg)
{

	CAN_Payload *payload = NULL;

	wait(input_payload_queue.get_event());

	payload = input_payload_queue.get_next_transaction();

	if (trace_enable) {
		can_tx_output_file << (can_tx_quantumkeeper.get_current_time().to_seconds() * 1000) << " ms \t\t" << *payload <<  std::endl;
	}

	payload->unpack(msg);

	payload->release();

	tlm_phase phase = BEGIN_RESP;
	sc_time local_time = SC_ZERO_TIME;
	can_tx_sock->nb_transport_bw( *payload, phase, local_time);

}

void CAN_STUB::inject(CAN_DATATYPE msg)
{

	tlm_phase phase = BEGIN_REQ;

	can_rx_payload = can_rx_payload_fabric.allocate();

//	atd1_payload->acquire();

	can_rx_payload->pack(msg);


//	sc_time local_time = atd1_quantumkeeper.get_local_time();
	sc_time local_time = SC_ZERO_TIME;

	if (trace_enable) {
		can_rx_output_file << (can_rx_quantumkeeper.get_current_time().to_seconds() * 1000) << " ms \t\t" << *can_rx_payload << std::endl;
	}

	tlm_sync_enum ret = can_rx_sock->nb_transport_fw(*can_rx_payload, phase, local_time);

	can_rx_payload->release();
	
	wait(can_bw_event);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			can_rx_quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			can_rx_quantumkeeper.set(local_time); // increase the time
			if(can_rx_quantumkeeper.need_sync()) can_rx_quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			can_rx_quantumkeeper.set(local_time); // increase the time
			if(can_rx_quantumkeeper.need_sync()) can_rx_quantumkeeper.sync(); // synchronize if needed
			break;
	}

	can_rx_quantumkeeper.inc(*can_rx_stimulus_period_sc);
	if(can_rx_quantumkeeper.need_sync()) can_rx_quantumkeeper.sync();

}

int CAN_STUB::RandomizeData(std::vector<CAN_DATATYPE* > &vect) {


	const int SET_SIZE = 1024;

	srand(12345);

	CAN_DATATYPE* data;
	for (int i=0; i < SET_SIZE; i++) {
		data = new CAN_DATATYPE();

		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
			data->ID[j] = rand();
		}
		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
			data->Data[j] = rand();
		}
		data->Length = rand() % 9;
		data->Priority = 0;
		data->Extended = rand() % 2;
		data->Remote = rand() % 2;
		if (data->Extended == 1) {
			data->ID[1] = (data->ID[1] | CAN_Payload::IDE_MASK);
			if (data->Remote) {
				data->ID[3] = data->ID[3] | CAN_Payload::EXT_RTR_MASK;
			} else {
				data->ID[3] = data->ID[3] & ~CAN_Payload::EXT_RTR_MASK;
			}

		} else {
			data->ID[1] = (data->ID[1] & ~CAN_Payload::IDE_MASK);
			if (data->Remote) {
				data->ID[1] = data->ID[1] | CAN_Payload::STD_RTR_MASK;
			} else {
				data->ID[1] = data->ID[1] & ~CAN_Payload::STD_RTR_MASK;
			}
		}
		data->Error = 0;
		data->Timestamp[0] = 0;
		data->Timestamp[1] = 0;

		vect.push_back(data);
	}

	return (SET_SIZE);
}

void CAN_STUB::parseRow (xmlDocPtr doc, xmlNodePtr cur, CAN_DATATYPE &data) {

//	xmlChar *key;
//	vector<xmlNodePtr> rowCells;
//
//	if ((xmlStrcmp(cur->name, (const xmlChar *)"Row"))) {
//		cerr << "Error: Can't parse " << cur->name << endl;
//		return;
//	}
//
//	cur = cur->xmlChildrenNode;
//	while (cur != NULL) {
//		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Cell"))) {
//			rowCells.push_back(cur);
//		}
//		cur = cur->next;
//	}
//
//	if ((rowCells.size() < 2) || (rowCells.size() > (SIZE+1))) {
//		cerr << "Error: Wrong row size. The minimum is 2 and the maximum is " << std::dec << (SIZE+1) << " cells by row." << endl;
//		return;
//	}
//
//	for (int i=0; i < SIZE; i++) {
//		data.volte[i] = 0;
//	}
//	data.time = 0;
//
//	/**
//	 * First cells of row are ATD voltage
//	 * The last cell is time of sampling
//	 */
//	for (unsigned int i=0; (i < rowCells.size()) && (i < SIZE); i++) {
//		cur = rowCells.at(i);
//
//		xmlNodePtr node = cur->children;
//		while (xmlStrcmp(node->name, (const xmlChar *)"Data")) {
//			node = node->next;
//		}
//
//		key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
//		if (i < (rowCells.size()-1)) {
//			data.volte[i] = std::atof((const char *) key);
//		} else {
//			data.time = std::atof((const char *) key);
//		}
//
//		xmlFree(key);
//
//	}
//

	return;
}

int CAN_STUB::LoadXmlData(const char *filename, std::vector<CAN_DATATYPE* > &vect) {

//	const char *path = "//Row";
//
//	xmlDocPtr doc = NULL;
//	xmlXPathContextPtr context = NULL;
//	xmlXPathObjectPtr xmlobject;
//	int result = 0;
//
//	cout << sc_object::name() << " Parsing " << filename << endl;
//
//	doc = xmlParseFile (GetSimulator()->SearchSharedDataFile(filename).c_str());
//	if (!doc)
//	{
//		cerr << __FILE__ << ":" << __LINE__ << " Could not parse the document" << endl;
//		return (0);
//	}
//
//	xmlXPathInit ();
//	context = xmlXPathNewContext (doc);
//
//	xmlobject = xmlXPathEval ((xmlChar *) path, context);
//	xmlXPathFreeContext (context);
//	if ((xmlobject->type == XPATH_NODESET) && (xmlobject->nodesetval))
//	{
//		if (xmlobject->nodesetval->nodeNr)
//		{
//			result = xmlobject->nodesetval->nodeNr;
//
//			xmlNodePtr node;
//			data_t<SIZE> *data;
//			for (int i=0; i<xmlobject->nodesetval->nodeNr; i++) {
//				node = xmlobject->nodesetval->nodeTab[i];
//				data = new data_t<SIZE>();
//				parseRow<SIZE> (doc, node, *data);
//				vect.push_back(data);
//			}
//		}
//	}
//
//	xmlXPathFreeObject (xmlobject);
//	xmlFreeDoc(doc);
//
//	return (result);

	return 0;
}

void CAN_STUB::Inject_CAN(CAN_DATATYPE msg)
{
	CAN_DATATYPE* data  = *(can_rx_vect.begin());

	for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
		data->ID[j] = msg.ID[j];
	}
	for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
		data->Data[j] = msg.Data[j];
	}
	data->Length = msg.Length;
	data->Priority = msg.Priority;
	data->Extended = msg.Extended;
	data->Error = msg.Error;
	data->Remote = msg.Remote;
	data->Timestamp[0] = msg.Timestamp[0];
	data->Timestamp[1] = msg.Timestamp[1];

}

void CAN_STUB::Get_CAN(CAN_DATATYPE *msg)
{

	for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
		msg->ID[j] = can_tx_buffer.ID[j];
	}
	for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
		msg->Data[j] = can_tx_buffer.Data[j];
	}
	msg->Length = can_tx_buffer.Length;
	msg->Priority = can_tx_buffer.Priority;
	msg->Extended = can_tx_buffer.Extended;
	msg->Error = can_tx_buffer.Error;
	msg->Remote = can_tx_buffer.Remote;
	msg->Timestamp[0] = can_tx_buffer.Timestamp[0];
	msg->Timestamp[1] = can_tx_buffer.Timestamp[1];

}

void CAN_STUB::processCANRX()
{

	can_rx_quantumkeeper.set(sc_time(20, SC_MS));
	if (can_rx_quantumkeeper.need_sync()) can_rx_quantumkeeper.sync();

	CAN_DATATYPE can_rx_buffer;

	while (!isTerminated()) {

		for (std::vector<CAN_DATATYPE*>::iterator it = can_rx_vect.begin() ; (it != can_rx_vect.end()) && !isTerminated(); ++it) {

			for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
				can_rx_buffer.ID[j] = (*it)->ID[j];
			}
			for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
				can_rx_buffer.Data[j] = (*it)->Data[j];
			}
			can_rx_buffer.Length = (*it)->Length;
			can_rx_buffer.Priority = (*it)->Priority;
			can_rx_buffer.Extended = (*it)->Extended;
			can_rx_buffer.Error = (*it)->Error;
			can_rx_buffer.Remote = (*it)->Remote;
			can_rx_buffer.Timestamp[0] = (*it)->Timestamp[0];
			can_rx_buffer.Timestamp[1] = (*it)->Timestamp[1];

			inject(can_rx_buffer);

		}
	}

}


void CAN_STUB::processCANTX()
{

	while (!isTerminated())
	{
		observe(can_tx_buffer);
	}

}
