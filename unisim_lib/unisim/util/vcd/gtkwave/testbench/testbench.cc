#include <unisim/util/vcd/vcd.hh>
#include <unisim/util/vcd/gtkwave/gtkwave.hh>

#include <fstream>

int main(int argc, char *argv[])
{
	unisim::util::vcd::gtkwave::Stream *gtkwave_stream = new unisim::util::vcd::gtkwave::Stream();
// 	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_GTKWAVE_PATH, "/path-to/gtkwave");
// 	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_DEBUG, false);
	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_KILL_GTKWAVE, true);
	gtkwave_stream->SetOption(unisim::util::vcd::gtkwave::OPT_VERBOSE, true);
	gtkwave_stream->Start();
	unisim::util::vcd::Writer *gtkwave_writer = new unisim::util::vcd::Writer(*gtkwave_stream, true);
	gtkwave_writer->Trace("mod.foo");
	
	std::ofstream file("out.vcd");
	unisim::util::vcd::Writer *file_writer = new unisim::util::vcd::Writer(file);
	file_writer->Trace("mod.foo");

	unisim::util::vcd::Output<bool> foo("mod.foo");

	unisim::util::vcd::Writer::SetTimeScale("1 ns");

	srand(1);
	uint64_t t;
	for(t = 0; t < 100000000; t++)
	{
		int r = rand() - ((RAND_MAX / 2) + 1);
		foo.Push(t, r >= 0);
		unisim::util::vcd::Writer::CommitUntil(t);
	}

	delete gtkwave_writer;
	delete file_writer;
 	delete gtkwave_stream;
	
	return 0;
}
