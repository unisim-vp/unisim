#include <unisim/util/vcd/vcd.hh>
#include <unisim/util/vcd/gtkwave/gtkwave.hh>

#include <fstream>
#include <cstdlib>

static const unsigned int NUM_OUTPUTS = 1000;
static const unsigned int NUM_CYCLES = 100000;
static const bool enable_file = true;
static const bool enable_gtkwave = true;

volatile unsigned int d;

int main(int argc, char *argv[])
{
	bool dist_commit = (argc >= 2) ? atoi(argv[1]) : false;
	std::cerr << "dist_commit=" << dist_commit << std::endl;
	unisim::util::vcd::gtkwave::Stream *gtkwave_stream = enable_gtkwave ? new unisim::util::vcd::gtkwave::Stream() : 0;
	if(gtkwave_stream)
	{
// 		gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_GTKWAVE_PATH, "/path-to/gtkwave");
// 		gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_DEBUG, false);
		gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_KILL_GTKWAVE, true);
		gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_VERBOSE, true);
		gtkwave_stream->Start();
	}
	unisim::util::vcd::StreamWriter *gtkwave_writer = gtkwave_stream ? new unisim::util::vcd::StreamWriter(*gtkwave_stream, true) : 0;
	
	unisim::util::vcd::SetTimeScale("1 ns");
	unisim::util::vcd::SetOption(unisim::util::vcd::OPT_SORT_VARIABLES, true);
	unisim::util::vcd::SetOption(unisim::util::vcd::OPT_DEBUG, false);

	std::ofstream *file = enable_file ? new std::ofstream("out.vcd") : 0;
	unisim::util::vcd::StreamWriter *file_writer = file ? new unisim::util::vcd::StreamWriter(*file) : 0;
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		std::stringstream output_name_sstr;
		output_name_sstr << "out" << i;
		if(file_writer) file_writer->Trace(output_name_sstr.str());
		if(gtkwave_writer) gtkwave_writer->Trace(output_name_sstr.str());
	}

	unisim::util::vcd::Output<bool> *outputs[NUM_OUTPUTS];
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		std::stringstream output_name_sstr;
		output_name_sstr << "out" << i;
		outputs[i] = new unisim::util::vcd::Output<bool>(output_name_sstr.str());
	}
	
	srand(1);
	uint64_t t;
	for(t = 0; t < NUM_CYCLES; t++)
	{
		for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
		{
			int r = rand() - ((RAND_MAX / 2) + 1);
			if(outputs[i]) outputs[i]->Push(t, r >= 0);
			d = rand() % 16;
			if(dist_commit && outputs[i]) outputs[i]->CommitUntil((t >= d) ? (t - d) : t);
		}
		if(t == 500)
		{
			delete outputs[123];
			outputs[123] = 0;
		}
		if(t == 700)
		{
			outputs[123] = new unisim::util::vcd::Output<bool>("out123");
		}
		if(!dist_commit) unisim::util::vcd::CommitUntil(t);
	}
	if(dist_commit)
	{
		for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
		{
			if(outputs[i]) outputs[i]->CommitUntil(NUM_CYCLES - 1);
		}
	}

	if(file_writer) delete file_writer;
	if(file) delete file;
	if(gtkwave_writer) delete gtkwave_writer;
	if(gtkwave_stream) delete gtkwave_stream;
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		if(outputs[i]) delete outputs[i];
	}
	return 0;
}
