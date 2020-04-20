#include <unisim/util/vcd/vcd.hh>
#include <unisim/util/vcd/gtkwave/gtkwave.hh>

#include <fstream>

static const unsigned int NUM_OUTPUTS = 1000;
static const unsigned int NUM_CYCLES = 1000000;

int main(int argc, char *argv[])
{
	unisim::util::vcd::gtkwave::Stream *gtkwave_stream = new unisim::util::vcd::gtkwave::Stream();
// 	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_GTKWAVE_PATH, "/path-to/gtkwave");
// 	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_DEBUG, false);
	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_KILL_GTKWAVE, true);
	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_VERBOSE, true);
	gtkwave_stream->Start();
	unisim::util::vcd::Writer *gtkwave_writer = new unisim::util::vcd::Writer(*gtkwave_stream, true);
	
	std::ofstream file("out.vcd");
	unisim::util::vcd::Writer *file_writer = new unisim::util::vcd::Writer(file);
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		std::stringstream output_name_sstr;
		output_name_sstr << "out" << i;
		file_writer->Trace(output_name_sstr.str());
		gtkwave_writer->Trace(output_name_sstr.str());
	}

	unisim::util::vcd::Output<bool> *outputs[NUM_OUTPUTS];
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		std::stringstream output_name_sstr;
		output_name_sstr << "out" << i;
		outputs[i] = new unisim::util::vcd::Output<bool>(output_name_sstr.str());
	}
	
	unisim::util::vcd::Writer::SetTimeScale("1 ns");

	srand(1);
	uint64_t t;
	for(t = 0; t < NUM_CYCLES; t++)
	{
		for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
		{
			int r = rand() - ((RAND_MAX / 2) + 1);
			outputs[i]->Push(t, r >= 0);
		}
		unisim::util::vcd::Writer::CommitUntil(t);
	}

	delete file_writer;
	delete gtkwave_writer;
	delete gtkwave_stream;
	for(unsigned int i = 0; i < NUM_OUTPUTS; ++i)
	{
		delete outputs[i];
	}
	return 0;
}
