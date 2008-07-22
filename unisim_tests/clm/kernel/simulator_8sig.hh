/*
 *  Copyright (c) 2007,
 *  University of Perpignan (UPVD),
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
 * Authors: 
 *     David Parello (david.parello@univ-perp.fr)
 *
 */

/***************************************************************************
                            simulator.hh  -  description
 ***************************************************************************/

// -------- COMMANDS ------------------------------
const int looptime = 100000001;


template <int NSIGNALS>
class Source: public module
{
public:
  /* clock */
  inclock inClock;                                                 ///< clock port
  outport<int> out[NSIGNALS]; 

  Source(const char *name): module(name)
  {
    class_name = " SOURCE";

    for (int i=0; i<NSIGNALS; i++)
      {
	out[i].set_unisim_name(this, "out",i);
      }    
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;

    for (int i=0; i<NSIGNALS; i++)
      {
	sensitive_method(onAccept) << out[i].accept;
      }
  }

  // SOC
  void start_of_cycle()
  {
    if (timestamp() < looptime)
      { 
	//	for (int i=0; i< NSIGNALS/2; i++) 
	for (int i=0; i< NSIGNALS; i++) 
	  { 
	    out[i].data = (timestamp());
	    //	    out.data[2*i+1].nothing();// = (1000 + timestamp());
	  }
	//	out.data.send();
      }
    else
      { //terminate_now(); 
      }
  }
  // ON ACCEPT
  void onAccept()
  {

    bool areallknown(true);
    for (int i=0; i<NSIGNALS; i++) areallknown &= out[i].accept.known();
    if (areallknown)
      {
	for (int i=0; i<NSIGNALS; i++) 
	  { out[i].enable = out[i].accept; }
	//    out.enable.send();
      }
  }
  // EOC
  void end_of_cycle()
  {
    for (int i=0; i<NSIGNALS; i++)
      {
	if (!out[i].accept)
	  {
	    //cerr << "error sink didn't accepting !!!" << endl;
	    //	    terminate_now();
	    //	    abort();
	  }
      }
  }
};

template <int NSIGNALS>
class Sink: public module
{
public:
  /* clock */
  inclock inClock;                                                 ///< clock port
  inport<int> in[NSIGNALS];

  Sink(const char *name): module(name)
  {
    class_name = " SINK";
      for (int i=0; i<NSIGNALS; i++)
      {
	in[i].set_unisim_name(this, "in",i);
      }
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;

    for (int i=0; i<NSIGNALS; i++)
      {
	sensitive_method(onData) << in[i].data;
      }
  }

  // SOC
  void start_of_cycle()
  {
  }
  // ON ACCEPT
  void onData()
  {
    bool areallknown(true);
    for (int i=0; i<NSIGNALS; i++) areallknown &= in[i].data.known();
    if (areallknown)
      {
	for (int i=0; i<NSIGNALS; i++)
	  { in[i].accept = in[i].data.something(); }
	//    in.accept.send();
      }
  }

  // EOC
  void end_of_cycle()
  {
    for (int i=0; i<NSIGNALS; i++)
      {
	if (in[i].enable)
	  {
	    if ( in[i].data.something() )
	      {
		//		cerr << "We recevied an integer from source : ";
		//		cerr << in.data[i].Data();
		//		cerr << endl;
		if ( in[i].data >= looptime - 1 ) { terminate_now(); }
	      }
	    /*
	    else
	      {
		cerr << "We recevied nothing... ";
		cerr << endl;
	      }
	    */
	  }
      }
  }

};


class GeneratedSimulator : public Simulator{
public:

  static const int NBS = 4;
  /**************************************************************************
   *                      DEFINITION OF CLM COMPONENTS                      *
   **************************************************************************/
  Source<NBS> *__src;
  Sink<NBS> *__snk;

  /**************************************************************************
   *                      CLM COMPONENT GENERATION and CONNECTION           *
   **************************************************************************/
  GeneratedSimulator() {

    // Module instantiactions
    __src = new Source<NBS>("__src__");
    __snk = new Sink<NBS>("__snk__");

    // Clock connections
    __src->inClock(global_clock);
    __snk->inClock(global_clock);

    // SIGNAL (or Module) connections
    //__src->out >> __snk->in;
    //    __src->out( __snk->in );
    for (int i=0; i < NBS; i++)
      {
	__src->out[i]( __snk->in[i] );
      }
  } //GeneratedSimulator()

  /**************************************************************************
   *               GENERATION and CONNECTION OF SERVICE COMPONENTS          *
   **************************************************************************/
  void ServiceConnection(
			 /*

			 //bool kernel_mode, bool use_gdb_server, bool use_inline_debugger)
			 const char *filename,
			 int sim_argc,
			 //	char **sim_argv,
			 //list<string> sim_argv,
			 bool use_gdb_server,
			 bool use_inline_debugger,
			 bool kernel_mode,
			 uint32_t video_refresh_period,
			 int gdb_server_tcp_port,
			 char *device_tree_filename,
			 char *gdb_server_arch_filename,
			 uint64_t maxinst
			 */
	)
  {

	int c;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	bool estimate_power = false;
	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	uint64_t maxinst = 0; // maximum number of instruction to simulate
	char *logger_filename = 0;
	bool logger_zip = false;
	bool logger_error = false;
	bool logger_out = false;
	bool logger_on = false;
	bool logger_messages = false;
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;

	const char *filename =  command_line[0];
	int sim_argc = command_line.count();

  } // ServiceConnection(...)

  void ServiceDeconnection()
  {
  }
  
}; //class GeneratedSimulator
