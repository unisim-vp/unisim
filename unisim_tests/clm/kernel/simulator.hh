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


template <int NSIGNALS>
class Source: public module
{
public:
  /* clock */
  inclock inClock;                                                 ///< clock port
  outport<int, NSIGNALS> out; 

  Source(const char *name): module(name)
  {
    class_name = " SOURCE";
    out.set_unisim_name(this, "out");
    
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;

    sensitive_method(onAccept) << out.accept;

  }

  // SOC
  void start_of_cycle()
  {
    if (timestamp() < 10)
      { for (int i=0; i< NSIGNALS; i++) { cerr << "source " << i << endl; }//out.data[i] = 1000 + timestamp(); }
      }
    else
      { //terminate_now(); 
      }
  }
  // ON ACCEPT
  void onAccept()
  {
    for (int i=0; i<NSIGNALS; i++) { out.enable[i] = out.accept[i]; }
  }

  // EOC
  void end_of_cycle()
  {
    for (int i=0; i<NSIGNALS; i++)
      {
	if (!out.enable[i])
	  {
	    cerr << "error sink didn't accepting !!!" << endl;
	    abort();
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
  inport<int, NSIGNALS> in;

  Sink(const char *name): module(name)
  {
    class_name = " SINK";
    in.set_unisim_name(this, "in");

    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;

    sensitive_method(onData) << in.data;

  }

  // SOC
  void start_of_cycle()
  {
  }
  // ON ACCEPT
  void onData()
  {
    for (int i=0; i<NSIGNALS; i++) 
      { in.accept[i] = in.data[i].something(); }
  }

  // EOC
  void end_of_cycle()
  {
    for (int i=0; i<NSIGNALS; i++)
      {
	if (true)//in.enable[i])
	  {
	    if ( true )//in.data[i].something() )
	    cerr << "We recevied an integer from source : " 
	      //<< in.data[i] 
		 << endl;
	  }
      }
  }

};


class GeneratedSimulator : public Simulator{
public:

  static const int NBS = 2;
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
    __src->out( __snk->in );

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
