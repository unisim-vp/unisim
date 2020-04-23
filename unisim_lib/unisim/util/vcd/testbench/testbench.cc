#include <unisim/util/vcd/vcd.hh>

int main(int argc, char *argv[])
{
	unisim::util::vcd::Writer *writer = new unisim::util::vcd::Writer(std::cout);
	writer->Trace("mod1.foo");
	writer->Trace("mod2.bar");
	writer->Trace("mod1.dummy");
	writer->Trace("mod2.reg16");

	unisim::util::vcd::Output<bool> foo("mod1.foo");
	unisim::util::vcd::Output<bool> bar("mod2.bar");
	unisim::util::vcd::Output<double> dummy("mod1.dummy");
	unisim::util::vcd::Output<uint16_t> reg16("mod2.reg16", "reg");

	foo.Push(0, 1); // At 0, foo <- '1'
	foo.Push(0, 0); // At 0, foo <- '0' (overwriting)
	foo.Push(1, 0);
	foo.Push(2, 0);
	foo.Push(3, 1);
	foo.Push(4, 0);
	foo.Push(5, 1);
	foo.Push(5, 0); // overwriting

	bar.Push(0, 0);
	bar.Push(1, 1);
	bar.Push(2, 0);
	bar.Push(3, 0);
	bar.Push(4, 1);
	bar.Push(5, 0);

	dummy.Push(0, 0.1);
	dummy.Push(1, 0.4);
	dummy.Push(2, 0.2);
	dummy.Push(3, 0.25);
	
	reg16.Push(0, 0xabcd);
	reg16.Push(1, 0xef01);
	reg16.Push(2, 0x2345);
	reg16.Push(3, 0x6789);

	unisim::util::vcd::SetTimeScale("1 ns");

	unisim::util::vcd::CommitUntil(2);

	unisim::util::vcd::CommitBefore(6);

	delete writer;
	
	return 0;
}
