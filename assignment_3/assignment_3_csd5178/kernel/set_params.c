/********************************************************/
/*** ASPRIDIS NIKOLAOS CSD5178 HY-345 2024 **************/
/*** set_params *****************************************/
/********************************************************/

#include <linux/sched.h> 		/* for struct task_struct definition and current */
#include <linux/kernel.h> 		/* for printk */
#include <linux/syscalls.h> 	    	/* for asmlinkage and syscall */
#include <asm-generic/errno-base.h> 	/* for EINVAL */
#include <linux/time.h>


asmlinkage long sys_set_params(long deadline_1, long deadline_2, long computation_time)
{
	/* pointer to the current process (process task_struct) */
	struct task_struct *task;
	
	/* var where time since 01/01/1970 will be stored */
	struct timespec seconds;	

	/* name, A.M. and syscall name is printed in kernel-level */	
	printk(KERN_INFO "Aspridis Nikolaos\ncsd5178\nsyscall name: 'sys_set_params'\n\n");

	/* current process's task_struct's address is assigned to task pointer */
	task = current;
		
	/* checks parameters' validity */
	if((deadline_1 <= 0) || (deadline_2 <= 0) || (computation_time <= 0))
	{
		/* err msg printed on debug log level */
		printk(KERN_DEBUG "ERROR: invalid parameter(s)!\n\n");
		return -EINVAL;	/* -22 is returned */	
	}
	
	/* var seconds is assigned time since 01/01/1970 */
	seconds = current_kernel_time();	
	
	/* fills in attributes with the given values */
	task->deadline_1 = deadline_1 + (long)(seconds.tv_sec);
	task->deadline_2 = deadline_2 + (long)(seconds.tv_sec);
	task->computation_time = computation_time;	
	
	return 0;
}

 
