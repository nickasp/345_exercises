/******************************************************/
/*** ASPRIDIS NIKOLAOS CSD5178 HY-345 2024 ************/
/*** get_score ****************************************/
/******************************************************/

#include <linux/syscalls.h>	/* for asmlinkage and syscall */
#include <linux/sched.h>	/* for struct task_struct definition and current */
#include <linux/time.h>
#include <linux/kernel.h>	/* for printk */

asmlinkage long sys_get_score()
{
	/* D1 for deadline_1, D2 for deadline_2 and D3 for current time */
	long D1, D2, D3;
	/* pointer to the current process's task_struct */
	struct task_struct *task;
	/* struct for storing time since 01/01/1970 */
	struct timespec seconds;

	/* seconds is assigned the time since 01/01/1970 */
	seconds = current_kernel_time();

	/* var D3 is assigned time since system boot */
	D3 = (long)(seconds.tv_sec);

	/* name, A.M. and syscall name are printed onto kernel level */
	printk(KERN_INFO "Aspridis Nikolaos\ncsd5178\nsyscall name: 'get_score'\n\n");

	/* task is assigned the address of current process's task_struct */
	task = current;

	/* D1 is assigned current process's deadline_1 */
	D1 = task->deadline_1;
	/* D2 is assigned current process's deadline_2 */
	D2 = task->deadline_2;

	if(D1>D3)
	{
		/* full points are returned */
		return 100;
	}
	else if(D3>D2)
	{
		/* no points returned */
		return 0;
	}
	else
	{
		/* some points returned */
		return ((D2-D3)*100)/(D2-D1);
	}	

}
 
