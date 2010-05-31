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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */



#include "xml_atd_pwm_stub.hh"

XML_ATD_PWM_STUB::XML_ATD_PWM_STUB(const sc_module_name& name, Object *parent) :
	ATD_PWM_STUB(name, parent),

	atd0_anx_stimulus_file(""),
	param_atd0_anx_stimulus_file("atd0-anx-stimulus-file", this, atd0_anx_stimulus_file),
	atd0_anx_start_channel(0),
	param_atd0_anx_start_channel("atd0-anx-start-channel", this, atd0_anx_start_channel),
	atd0_anx_wrap_around_channel(ATD0_SIZE -1),
	param_atd0_anx_wrap_around_channel("atd0-anx-wrap-around-channel", this, atd0_anx_wrap_around_channel),

	atd1_anx_stimulus_file(""),
	param_atd1_anx_stimulus_file("atd1-anx-stimulus-file", this, atd1_anx_stimulus_file),
	atd1_anx_start_channel(0),
	param_atd1_anx_start_channel("atd1-anx-start-channel", this, atd1_anx_start_channel),
	atd1_anx_wrap_around_channel(ATD1_SIZE -1),
	param_atd1_anx_wrap_around_channel("atd1-anx-wrap-around-channel", this, atd1_anx_wrap_around_channel)

{
	SC_HAS_PROCESS(XML_ATD_PWM_STUB);

	SC_THREAD(ProcessATD);
	SC_THREAD(ProcessPWM);

}

XML_ATD_PWM_STUB::~XML_ATD_PWM_STUB() {

	atd0_vect.clear();
	atd1_vect.clear();

}


template <int SIZE> void XML_ATD_PWM_STUB::parseRow (xmlDocPtr doc, xmlNodePtr cur, data_t<SIZE> &data) {

	xmlChar *key;
	vector<xmlNodePtr> rowCells;

	if ((xmlStrcmp(cur->name, (const xmlChar *)"Row"))) {
		cerr << "Error: Can't parse " << cur->name << endl;
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Cell"))) {
			rowCells.push_back(cur);
		}
		cur = cur->next;
	}

	if ((rowCells.size() < 2) || (rowCells.size() > (SIZE+1))) {
		cerr << "Error: Wrong row size. The minimum is 2 and the maximum is " << std::dec << (SIZE+1) << " cells by row." << endl;
		return;
	}

	for (int i=0; i < SIZE; i++) {
		data.volte[i] = 0;
	}
	data.time = 0;

	/**
	 * First cells of row are ATD voltage
	 * The last cell is time of sampling
	 */
	for (int i=0; (i < rowCells.size()) && (i < SIZE); i++) {
		cur = rowCells.at(i);

		xmlNodePtr node = cur->children;
		while (xmlStrcmp(node->name, (const xmlChar *)"Data")) {
			node = node->next;
		}

		key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
		if (i < (rowCells.size()-1)) {
			data.volte[i] = std::atof((const char *) key);
		} else {
			data.time = std::atof((const char *) key);
		}

		xmlFree(key);

	}


	return;
}

template <int SIZE> int XML_ATD_PWM_STUB::LoadXmlData(const char *filename, std::vector<data_t<SIZE> > &vect) {

	const char *path = "//Row";

	xmlDocPtr doc = NULL;
	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr xmlobject;
	int result = 0;

	doc = xmlParseFile (GetSimulator()->SearchSharedDataFile(filename).c_str());
	if (!doc)
	{
		cerr << __FILE__ << ":" << __LINE__ << " Could not parse the document" << endl;
		return (0);
	}

	xmlXPathInit ();
	context = xmlXPathNewContext (doc);

	xmlobject = xmlXPathEval ((xmlChar *) path, context);
	xmlXPathFreeContext (context);
	if ((xmlobject->type == XPATH_NODESET) && (xmlobject->nodesetval))
	{
		if (xmlobject->nodesetval->nodeNr)
		{
			result = xmlobject->nodesetval->nodeNr;

			xmlNodePtr node;
			data_t<SIZE> data;
			for (int i=0; i<xmlobject->nodesetval->nodeNr; i++) {
				node = xmlobject->nodesetval->nodeTab[i];
				parseRow<SIZE> (doc, node, data);
				vect.push_back(data);
			}
		}
	}

	xmlXPathFreeObject (xmlobject);
	xmlFreeDoc(doc);

	return (result);
}

void XML_ATD_PWM_STUB::ProcessATD()
{
	srand(12345);

	sc_time delay(anx_stimulus_period, SC_PS);

	int atd0_data_size, atd0_data_index = 0;
	int atd1_data_size, atd1_data_index = 0;


	LoadXmlData<ATD0_SIZE>(atd0_anx_stimulus_file.c_str(), atd0_vect);
	LoadXmlData<ATD1_SIZE>(atd1_anx_stimulus_file.c_str(), atd1_vect);


	atd0_data_size = atd0_vect.size();
	atd1_data_size = atd1_vect.size();

	if (atd0_data_size == 0) {
		cerr << name() << " Warning: ATD0 random inputs values will be used during simulation !" << endl;
	}
	if (atd1_data_size == 0) {
		cerr << name() << " Warning: ATD1 random inputs values will be used during simulation !" << endl;
	}

	/**
	 * Note: The Software sample the ATDDRx every 20ms. As well as for the first sampling
	 */
	wait(sc_time(20, SC_MS));

	while(1)
	{
		double atd1_anValue[ATD1_SIZE];
		double atd0_anValue[ATD0_SIZE];
		bool pwmValue[PWM_SIZE];

		uint8_t atd0_wrap_around;
		uint8_t atd0_start;

		uint8_t atd1_wrap_around;
		uint8_t atd1_start;

		if (atd0_anx_wrap_around_channel < ATD0_SIZE) {
			atd0_wrap_around = atd0_anx_wrap_around_channel;
		} else {
			atd0_wrap_around = ATD0_SIZE - 1;
		}

		if (atd0_anx_start_channel < ATD0_SIZE) {
			atd0_start = atd0_anx_start_channel;
		} else {
			atd0_start = 0;
		}

		if (atd1_anx_wrap_around_channel < ATD1_SIZE) {
			atd1_wrap_around = atd1_anx_wrap_around_channel;
		} else {
			atd1_wrap_around = ATD1_SIZE - 1;
		}

		if (atd1_anx_start_channel < ATD1_SIZE) {
			atd1_start = atd1_anx_start_channel;
		} else {
			atd1_start = 0;
		}

		if (atd0_data_size > 0) {
			atd0_data_index = (atd0_data_index + 1) % atd0_data_size;
		}

		uint8_t j = 0;
		for (uint8_t i=0; i < ATD0_SIZE; i++) {
			if (atd0_data_size > 0) {
				if ((i < atd0_start) || (i > atd0_wrap_around)) {
					atd0_anValue[i] = 0;
				} else {
					atd0_anValue[i] = atd0_vect.at(atd0_data_index).volte[j++];
				}
			} else {
				atd0_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			}
		}

		if (atd1_data_size > 0) {
			atd1_data_index = (atd1_data_index + 1) % atd1_data_size;
		}

		j = 0;
		for (uint8_t i=0; i < ATD1_SIZE; i++) {
			if (atd1_data_size > 0) {
				if ((i < atd1_start) || (i > atd1_wrap_around)) {
					atd1_anValue[i] = 0;
				} else {
					atd1_anValue[i] = atd1_vect.at(atd1_data_index).volte[j++];
				}
			} else {
				atd1_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			}
		}

		Output_ATD1(atd1_anValue);
		Output_ATD0(atd0_anValue);

		wait(delay);

		quantumkeeper.inc(delay);
		quantumkeeper.sync();
	}

}

void XML_ATD_PWM_STUB::ProcessPWM()
{

	bool pwmValue[PWM_SIZE];

	while(1)
	{
		wait(input_payload_queue.get_event());

		Input(pwmValue);

		quantumkeeper.sync();
	}

}
