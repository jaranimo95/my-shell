#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>

int main()
{
	struct tms exe_time;

	if(fork() == 0)
	{
		int i;
		for(i = 0; i < 120000; i++){
			printf("fuck\n");
		}
	}
	else
	{
		int status;
		wait(&status);

		int user_time = exe_time.tms_cutime;
		int sys_time = exe_time.tms_cstime;

		printf("User Time: %Lf\n", (long double) user_time);
		printf("Sys Time:  %Lf\n", (long double) sys_time);
	}
}