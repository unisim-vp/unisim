/*
 *  Copyright (c) 2022,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/util/hla/hla.tcc>
#include <unisim/util/random/random.hh>

struct TIME_TRAIT
{
	typedef uint64_t TIME_TYPE;
	typedef uint64_t SCALAR_TYPE;
};

typedef unisim::util::hla::Federate<TIME_TRAIT> Federate;

unisim::util::random::Random random_gen = unisim::util::random::Random(0xdeadbeef, 0xc0cababe, 0xbeefc0ca, 0x12345678);

struct ObjectClassA : unisim::util::hla::ObjectClass
{
	unisim::util::hla::Attribute<int32_t> x0;
	unisim::util::hla::Attribute<int32_t> x1;
	
	ObjectClassA(Federate& federate)
		: ObjectClass(federate, L"A")
		, x0(*this, L"X0")
		, x1(*this, L"X1")
	{
	}
};

struct ObjectClassB : unisim::util::hla::ObjectClass
{
	unisim::util::hla::Attribute<int32_t> y0;
	
	ObjectClassB(Federate& federate)
		: unisim::util::hla::ObjectClass(federate, L"B")
		, y0(*this, L"Y0")
	{
	}
};

struct Tester : Federate
{
	ObjectClassA A;
	ObjectClassB B;
	unisim::util::hla::ObjectInstance a;
	unisim::util::hla::ObjectInstance b;
	uint64_t cycle_time_ps;
	uint64_t skew_ps;
	
	Tester(uint64_t _cycle_time_ps, uint64_t _skew_ps = 0)
		: Federate(
			L"localhost",
			L"myfederation",
			L"fdd.xml",
			L"tester",
			L"tester",
			_cycle_time_ps - 1
		)
		, A(*this)
		, B(*this)
		, a(A, L"a")
		, b(B, L"b")
		, cycle_time_ps(_cycle_time_ps)
		, skew_ps(_skew_ps)
	{
		A.x0.Publish();
		A.x1.Publish();
		B.y0.Subscribe();
	}
	
	bool Run(unsigned int cycles)
	{
		if(!Initialize()) return false;
		
		try
		{
			unisim::util::hla::AttributeValue<int32_t>& y0 = B.y0.Value(b);
			unisim::util::hla::AttributeValue<int32_t>& x0 = A.x0.Value(a);
			unisim::util::hla::AttributeValue<int32_t>& x1 = A.x1.Value(a);
			
			uint64_t ref_y0 = 0;
			uint64_t ref_prev_y0 = 0;
			for(unsigned int i = 0; i < cycles; ++i)
			{
				double time_stamp = (double) TimeStamp() / 1e12;
				std::wcout << "Cycle #" << i << ":" << time_stamp << " sec" << std::endl;
				std::wcout << "b.Y0 = " << y0 << std::endl;
				if(uint64_t(y0) != ref_y0)
				{
					std::wcerr << "WARNING! b.Y0 != " << ref_y0 << std::endl;
				}
				x0 = random_gen.Generate();
				x1 = random_gen.Generate();
				ref_y0 = ref_prev_y0;
				ref_prev_y0 = x0 + x1;
				std::wcout << "a.X0 <- " << x0 << std::endl;
				std::wcout << "a.X1 <- " << x1 << std::endl;
				Wait(cycle_time_ps + skew_ps);
				
				if(Killed()) return false;
				
				skew_ps = 0;
			}
		}
		catch(rti1516e::Exception& e)
		{
			DebugErrorStream() << e.what() << std::endl;
			return false;
		}
		catch(unisim::util::hla::Exception& e)
		{
			DebugErrorStream() << e.what() << std::endl;
			return false;
		}
		
		return true;
	}
};

struct Device : Federate
{
	ObjectClassA A;
	ObjectClassB B;
	unisim::util::hla::ObjectInstance a;
	unisim::util::hla::ObjectInstance b;
	uint64_t cycle_time_ps;
	uint64_t skew_ps;
	
	Device(uint64_t _cycle_time_ps, uint64_t _skew_ps = 0)
		: Federate(
			L"localhost",
			L"myfederation",
			L"/local/home/gmouchard/unisim/git/benchmarks/powerpc/unisim_mpc5777m_benchs/hla/fdd.xml",
			L"device",
			L"device",
			_cycle_time_ps - 1
		)
		, A(*this)
		, B(*this)
		, a(A, L"a")
		, b(B, L"b")
		, cycle_time_ps(_cycle_time_ps)
		, skew_ps(_skew_ps)
	{
		A.x0.Subscribe();
		A.x1.Subscribe();
		B.y0.Publish();
	}
	
	bool Run(unsigned int cycles)
	{
		if(!Initialize()) return false;
		
		try
		{
			unisim::util::hla::AttributeValue<int32_t>& y0 = B.y0.Value(b);
			unisim::util::hla::AttributeValue<int32_t>& x0 = A.x0.Value(a);
			unisim::util::hla::AttributeValue<int32_t>& x1 = A.x1.Value(a);
			
			for(unsigned int i = 0; i < cycles; ++i)
			{
				double time_stamp = (double) TimeStamp() / 1e12;
				std::wcout << "Cycle #" << i << ":" << time_stamp << " sec" << std::endl;
				std::wcout << "a.X0 = " << x0 << std::endl;
				std::wcout << "a.X1 = " << x1 << std::endl;
				y0 = x0 + x1;
				std::wcout << "b.Y0 <- " << y0 << std::endl;
				Wait(cycle_time_ps + skew_ps);
				
				if(Killed()) return false;
				
				skew_ps = 0;
			}
		}
		catch(rti1516e::Exception& e)
		{
			DebugErrorStream() << e.what() << std::endl;
			return false;
		}
		catch(unisim::util::hla::Exception& e)
		{
			DebugErrorStream() << e.what() << std::endl;
			return false;
		}
		
		return true;
	}
};

template <typename FEDERATE>
bool Testbench(unsigned int cycles, uint64_t cycle_time_ps, uint64_t skew_ps = 0)
{
	FEDERATE federate(cycle_time_ps, skew_ps);
	federate.SetVerbose();
	federate.SetDebug();
	
	return federate.Run(cycles);
}

int main(int argc, const char *argv[])
{
  std::wcout.sync_with_stdio(false);
  std::wcout.imbue(std::locale("en_US.utf8"));
  std::wcerr.sync_with_stdio(false);
  std::wcerr.imbue(std::locale("en_US.utf8"));
	
	if(argc < 2)
	{
		std::wcout << "Expected a role" << std::endl;
		return -1;
	}
	
	unsigned int device_cycles = 200000;
	uint64_t device_cycle_time_ps = 5000000000;
	uint64_t device_skew_ps = 3000000000;
	
	unsigned int tester_cycles = 100000;
	uint64_t tester_cycle_time_ps = 10000000000;
	uint64_t tester_skew_ps = 0;
	
	if(strcmp(argv[1], "device") == 0)
	{
		return Testbench<Device>(device_cycles, device_cycle_time_ps, device_skew_ps) ? 0 : -1;
	}
	else if(strcmp(argv[1], "tester") == 0)
	{
		return Testbench<Tester>(tester_cycles, tester_cycle_time_ps, tester_skew_ps) ? 0 : -1;
	}
	else
	{
		std::wcout << "Unexpected role " << argv[1] << std::endl;
	}
	
	return -1;
}
