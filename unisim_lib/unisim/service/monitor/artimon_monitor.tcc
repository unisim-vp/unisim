
#include <unisim/service/monitor/artimon_monitor.hh>

#include <stdio.h>

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
ArtimonMonitor<ADDRESS>::ArtimonMonitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)

	, nb_main(0)
	, main_values(NULL)
	, nb_atoms(0)
	, nb_sig(0)

	, isInitialized(false)

{

}

template <class ADDRESS>
ArtimonMonitor<ADDRESS>::~ArtimonMonitor()
{
	if (isInitialized) { close_artimon_4_simulators(); }
}

template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool ArtimonMonitor<ADDRESS>::BeginSetup()
{

	return true;
}

template <class ADDRESS>
bool ArtimonMonitor<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool ArtimonMonitor<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
int ArtimonMonitor<ADDRESS>::generate_monitor_spec(const char* file_path)
{
	std::cout << "ARTiMon:: generate_monitor_spec() " << file_path << std::endl;

	if (! init_artimon_4_simulators((char*) "Dummy",&nb_atoms, &nb_sig, 1, &nb_main, &main_values))
	{
	    isInitialized = true;
		printf("\nInit ARTiMon Ok\n");
	}
	else
		return (-1);

//	int N = 15;
//
//	int n = 0, n_1 = 1, n_2 = 1, res;
//
//	while (n <= N)
//	{
//
//		if (n < 2)
//		{
//			res = 1;
//		}
//		else
//		{
//			res = n_1 + n_2;
//		}
//
//		unsigned sigtab[2];
//
//		double d = (double) res;
//		sigtab[0] = (* ((unsigned *) &d));
//		sigtab[1] = (*(((unsigned *) &d) + 1));
//		artimon_refresh_signal(0, sigtab);
//
//		artimon_refresh_time((double) n);
//
//		if (n != 10)
//		{
//			n_2 = n_1;
//			n_1 = res;
//		}
//		else
//		{
//			// classical error (assignation in wrong order)
//			n_1 = res;
//			n_2 = n_1;
//
//		}
//
//
//
//		n += 1;
//
//	};


	return 0;
}

template <class ADDRESS>
int ArtimonMonitor<ADDRESS>::getPropertyIndex(const char* name)
{
	return 0;
}


template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::refresh_value(const char* name, bool value)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	artimon_refresh_atom(getPropertyIndex(name), value);
}

template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::refresh_value(const char* name, double value)
{
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);

	unsigned sigtab[2];

	sigtab[0] = (* ((unsigned *) &value));
	sigtab[1] = (*(((unsigned *) &value) + 1));
	artimon_refresh_signal(getPropertyIndex(name), sigtab);

}

template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern double (* artimon_refresh_time)(double t);

	artimon_refresh_atom(getPropertyIndex(name), value);

	artimon_refresh_time(time);
}

template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	//	extern double (* artimon_refresh_time)(double t);

	unsigned sigtab[2];

	sigtab[0] = (* ((unsigned *) &value));
	sigtab[1] = (*(((unsigned *) &value) + 1));
	artimon_refresh_signal(getPropertyIndex(name), sigtab);

	artimon_refresh_time(time);
}

template <class ADDRESS>
void ArtimonMonitor<ADDRESS>::refresh_time(double time)
{
	//	extern double (* artimon_refresh_time)(double t);
	artimon_refresh_time(time);
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

