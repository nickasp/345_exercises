/******************************************************/
/*** ASPRIDIS NIKOLAOS CSD5178 HY-345 2024 ************/
/*** get_params ***************************************/
/******************************************************/

#include <linux/sched.h>		/* for struct task_struct definition and current */
#include <linux/syscalls.h>		/* for asmlinkage and syscall and d_params */
#include <linux/kernel.h>		/* for printk */
#include <asm-generic/errno-base.h>	/* for EINVAL */
#include <linux/uaccess.h>		/* for access_ok() */

asmlinkage long sys_get_params(struct d_params *params)
{
	/* pointer to the current process's task_struct */
	struct task_struct *task;
	/* struct d_params for temporary storage of deadline1, deadline2 and computation time */
	struct d_params s_buffer;

	/* name, A.M. and syscall name are printed onto kernel level */
	printk(KERN_INFO "Aspridis Nikolaos\ncsd5178\nsyscall name: 'get_params'\n\n");

	/* task is assigned the address of current process's task_struct */
	task = current;

	/* checks if params == NULL */
	if(!params)
	{
		/* error message is printed and -22 (-EINVAL) is returned */
		printk(KERN_DEBUG "ERROR: params parameter is NULL\n\n");
		return -EINVAL;				
	}	

	/* else */

	/* checks if kernel can write to struct pointed to by param safely */
	if(!access_ok(VERIFY_WRITE, params, sizeof(struct d_params)))
	{
		/* error message is printed and -14 (-EFAULT) is returned */
		printk(KERN_DEBUG "ERROR: params pointer not safe\n\n");
		return -EFAULT; 
	}
 
	/* s_buffer is assigned the values found into task_struct of current procedure */
	s_buffer.deadline_1 = task->deadline_1;
	s_buffer.deadline_2 = task->deadline_2;
	s_buffer.computation_time = task->computation_time;

	if(copy_to_user(params, &s_buffer, sizeof(struct d_params)))
	{
		printk(KERN_DEBUG "ERROR: copying from kernel-space to user-space failed\n\n");
		return -EFAULT;
	}

	return 0;
}
