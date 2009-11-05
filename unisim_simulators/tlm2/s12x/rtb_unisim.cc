/*
 */


#include "rtb_unisim.hh"

RTBStub::RTBStub(const sc_module_name& name, Object *parent) :
	ATD_PWM_STUB(name, parent)

{
	init_client();

}

RTBStub::~RTBStub() {

}

/*
Initialization of  Client
*/
int RTBStub::init_client(){
	int NB_INPUT, NB_OUTPUT;

	char IN_DATA_TYPE[128],OUT_DATA_TYPE[128];

	char * host = getenv("COB_HOST");
	char * port_ptr = getenv("COB_PORT");
	int port = atoi(port_ptr) ;

	if (host == NULL || port_ptr == NULL) {
		connected = FALSE;
		printf("Cob isn't connected.\n");

	}else {
		connected = TRUE;
		printf("Cob is connected.\n");
	}
	if(connected){
		init_network();

		client_ctl = new_client_struct(host, port);

		if ( !connect_client_struct(client_ctl) ) {
			free_client_struct(client_ctl);
			printf("Connexion to client contoller failed.");
			clean_network();
			return 1;
		}

		client_in = new_client_struct(host , port );
		if ( !connect_client_struct(client_in)  ) {
			free_client_struct(client_ctl);
			free_client_struct(client_in);
			printf("Connexion to client in failed.");
			clean_network();
			return 0;
		}

		client_out = new_client_struct(host , port);
		if ( !connect_client_struct(client_out) ) {
			free_client_struct(client_ctl);
			free_client_struct(client_in);
			free_client_struct(client_out);
			printf("Connexion to client out failed.");
			clean_network();
			return 0;
		}

		dataline_in = new_dataline(2, "integer,integer");
		init_dataline(dataline_in, client_in->sockfd);

		data_in = new_data(2, "integer,integer", TRUE);
		data_out = new_data(1, "integer", TRUE);
	}
}

/*
Control
*/
void RTBStub::control() {

	if ( is_socket_ready_toread(client_ctl->sockfd, 0) ) {
		command_enum command = COB_CONTINUE;
		if ( receive_control(client_ctl->sockfd, &command) ) {
			switch (command) {
			case COB_PAUSE:
				printf("--Command ---> COB_PAUSE received.\n");
				send_ack(client_ctl->sockfd);
				break;
				case COB_CONTINUE:
				// do continue only if simulation is paused
				printf("--Command ---> COB_CONTINUE received.\n");
				send_ack(client_ctl->sockfd);
				break;
			case COB_STOP:
				printf("--Command ---> COB_STOP received.\n");
				// simulation is ended
				//sends ack for stop
				send_ack(client_ctl->sockfd);
				//cout << "Press button for exiting the program" << endl ;
				getchar();
				exit(0);
				break;
			default:
				printf("Controller: Unkwown command: %d.\n", command);
				break;
			}
		}
	}
}

/*
This function can make the exchange process between RT-Builder and RTBstub
*/
void RTBStub::exchange(){
	double current_time = sc_time_stamp().to_seconds() ;
	/*********send data************/
	if ( current_time > data_out->time ) {
		data_out->time = current_time-(2*cycle_time.to_seconds());
		data_out->validity = current_time ;
		if ( !send_data(client_out->sockfd, data_out) ) {
			cerr << "Cannot send data" << endl;
			exit(1);
		}
	}
	/*********get data************/
	if ( !get_data_at_time(dataline_in, current_time, INFINITE, data_in) ) {
		cerr << "error get data " << endl;
		exit(1);
	}
	control();
	cout << "current_time : " << current_time  <<" s "<<endl;
}


void RTBStub::ProcessATD() {

	double atd1_anValue[ATD1_SIZE];
	double atd0_anValue[ATD0_SIZE];

	srand(12345);

	sc_time delay(anx_stimulus_period, SC_PS);

	int atd0_data_index = 0;
	int atd1_data_index = 0;

	/**
	 * Note: The Software sample the ATDDRx every 20ms. As well as for the first sampling
	 */
	wait(sc_time(20, SC_MS));

	while(1)
	{
		for (uint8_t i=0; i < ATD0_SIZE; i++) {
			atd0_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		}

		for (uint8_t i=0; i < ATD1_SIZE; i++) {
			atd1_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		}

		/**
		 * TODO: Note: I think that the exchange() method has to be split into two parts:
		 *  - the first once, used at ProcessATD(), is for reading ATD_Voltage from RTBuilder which are then forwarded to UNISIM simulator "Output() methods"
		 *  - the second once, used at ProcessPWM(), is for forwarding data received from the UNISIM simulator "Input() method)" to RTBuilder
		 */
		exchange();

		Output_ATD1(atd1_anValue);
		Output_ATD0(atd0_anValue);

		wait(delay);

		quantumkeeper.inc(delay);
		quantumkeeper.sync();

	}

}

void RTBStub::ProcessPWM() {

	bool pwmValue[PWM_SIZE];

	while(1)
	{
		wait(input_payload_queue.get_event());

		Input(pwmValue);

		/**
		 * TODO: Note: I think that the exchange() method has to be split into two parts:
		 *  - the first once, used at ProcessATD(), is for reading ATD_Voltage from RTBuilder which are then forwarded to UNISIM simulator "Output() methods"
		 *  - the second once, used at ProcessPWM(), is for forwarding data received from the UNISIM simulator "Input() method)" to RTBuilder
		 */
		exchange();

		quantumkeeper.sync();
	}

}
