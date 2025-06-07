/****** ASPRIDIS NIKOLAOS CSD5178 HY-345 2024 ******/
/*** test.c ****************************************/
/***************************************************/

#include <sys/syscall.h>	/* for syscall numbers */
#include <unistd.h>		/* for syscall() */
#include <stdio.h>		
#include <errno.h>
#include <time.h>		/* for sleep() */

/* define statements for the usage of my syscalls */
#define __NR_set_params 341 
#define __NR_get_params 342
#define __NR_get_score 343

#define set_params(arg1, arg2, arg3) syscall(__NR_set_params, arg1, arg2, arg3)
#define get_params(arg1) syscall(__NR_get_params, arg1)
#define get_score() syscall(__NR_get_score)

/* struct d_params definition (user-level programs cannot include <linux/d_params.h>) */
struct d_params
{
	long deadline_1;
	long deadline_2;
	long computation_time;
};


int main(void)
{
	struct d_params params;		
	long deadline_1, deadline_2, computation_time, score;
	int i, parameters[] = {3, 5, 7, 10, 13};	/* this array contains the seconds process 
			will sleep, simulating each time the time process needed to be executed */
	
	/* sets values for the parameters */
	deadline_1 = 5;
	deadline_2 = 10;
	computation_time = 3;

	for(i=0; i<5; i++)
	{
		/* 5 times set_params, get_params and get_score will be called 
		 * 1st and 2nd call simulates cases when actual computation time is equal or
 		 * less than 1s deadline, 3rd call simulates cases when actual computation
 		 * time is between 1st and second deadlines, 4th call simulates cases when
 		 * actual computation time is equal to the 2nd deadline, while 5th call
 		 * simulates cases when actual computation time is greater than the 2nd
 		 * deadline 
		 */
		/* checks if set_params was executed successfully */
		if(set_params(deadline_1, deadline_2, computation_time))
		{
			fprintf(stderr, "ERROR: set_params() failed\ntest program terminated\n\n");
			return errno;	
		}
		else
		{
			printf("set_params(): ok\n\n");
		}

		/* checks if get_params was executed successfully */
		if(get_params(&params))
		{
			fprintf(stderr, "ERROR: get_params() failed\ntest program terminated\n\n");
			return errno;
		}
		else
		{
			printf("get_params(): ok\n\n");
		}
		
		/* simulates process' actual computation time in seconds */
		sleep(parameters[i]);

		/* score is assigned actual score counted by get_score() */
		score = get_score();

		printf("call no.%d -//- score: %ld\n\n", i+1, score);
	
	}
	
	printf("value to be returned: %d\n", -EINVAL);
	set_params(-5, deadline_2, computation_time);
	printf("errno: %d\n\n", errno);
	
	printf("value to be returned: %d\n", -EINVAL);
	get_params(NULL);		
	printf("errno: %d\n\n", errno);

	return 0;
}


