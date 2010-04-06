#include <stdio.h>
#include <sys/utsname.h>


int main(int argc, char *argv[])
{
	struct utsname buf;
    int uname_ret = uname(&buf);
	printf("This simulator seems to be working :)))\n");
	printf("Uname result: %i\n", uname_ret);
	if ( uname_ret == 0 )
	{
		printf(" - sysname = %s\n", buf.sysname);
		printf(" - nodename = %s\n", buf.nodename);
		printf(" - release = %s\n", buf.release);
		printf(" - version = %s\n", buf.version);
		printf(" - machine = %s\n", buf.machine);
	}
	return 0;
}
