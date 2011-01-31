#include <stdio.h>
#include <sys/utsname.h>

void uname_test()
{
	struct utsname buf;
    int uname_ret;

	printf(
"***********************************************************************\n"
);
	printf(
"START TEST: 'uname' system call translation ***************************\n"
);
	uname_ret = uname(&buf);
	printf("Uname result: %i\n", uname_ret);
	if ( uname_ret == 0 )
	{
		printf(" - sysname = %s\n", buf.sysname);
		printf(" - nodename = %s\n", buf.nodename);
		printf(" - release = %s\n", buf.release);
		printf(" - version = %s\n", buf.version);
		printf(" - machine = %s\n", buf.machine);
	}
	printf(
"END TEST: 'uname' system call translation ******************************\n"
);
	printf(
"************************************************************************\n"
);
}

int rec_fibonnaci(int n)
{
	printf(".");
	fflush(stdout);
	if ( n == 1 ) return 0;
	if ( n == 2 ) return 1;
	return rec_fibonnaci(n-1) + rec_fibonnaci(n-2);
}

void rec_fibonnaci_test()
{
	int goal = 12;
	int result = 0;
	printf(
"***********************************************************************\n"
);
	printf(
"START TEST: 'recursive fibonnaci' system call translation *************\n"
);
	printf("Computing fibonnaci of %d recursively.\n", goal);
	printf("Progress: ");
	result = rec_fibonnaci(goal);
	printf("\n");
	printf("Fibonnaci(%d) = %d\n", goal, result);
	printf(
"END TEST: 'recursive fibonnaci' system call translation ****************\n"
);
	printf(
"************************************************************************\n"
);

}

int main(int argc, char *argv[])
{
	printf(
"ARMEmu simulator selftest =============================================\n"
);
	printf("\tAuthor: daniel.gracia-perez@cea.fr\n");
	printf("\tLast update: 27/08/2010\n");
	printf(
"=======================================================================\n"
"\n\n");

	uname_test();
	printf("\n");

	rec_fibonnaci_test();
	printf("\n");
	printf(
"End of ARMEmu simulator selftest ======================================\n"
);
	printf(
"=======================================================================\n"
);


	return 0;
}
