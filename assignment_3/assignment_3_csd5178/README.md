//-------- ASPRIDIS NIKOLAOS csd5178 ----------//
//
//-------- hy345 2024
//
//---------------------------------------------//

step 1: I defined three system-call numbers (one for syscall set_params, one for
	syscall get_params and one for syscall get_score) and I added them
	into linux-2.6.38.1/arch/x86/include/asm/unistd_32.h header file.
	Then, I incremented the total number of system calls by one.

step 2: I defined a function pointer for each of the three new system calls,
	so as kernel to know which function to call when its corresponding system-call
	number is used. Those function pointers were added into
	linux-2.6.38.1/arch/z86/kernel/syscall_table_32.S file.

step 3: I defined a function signature for each function-implementation of
	each of the system calls. Those function signatures were added into
	linux-2.6.38.1/include/asm-generic/syscalls.h header file.

step 4: I implemented the 3 new system calls in three different .c files
	(set_params.c, get_params.c and get_score.c), that were added into
	linux-2.6.38.1/kernel folder. 

	sys_set_params(long d1, long d2, long cmpt_time):
		I copy d1, d2 and cmpt_time to current process' task_struct's
		field deadline_1, deadline_2 and computation_time correspondingly, 
		adding kernel time the moment the syscall occured.
		I use current_kernel_time() function to obtain kernel time
		and to add it to deadline_1 / deadline_2 , so as deadlines to
		to have their starting point the time the syscall occured. If
		at least one argument is invalid, -EINVAL is returned and syscall is
		terminated. Otherwise, 0 is returned.

	sys_get_params(d_params *params): 
		After I have checked that kernel can have secure access to the
		user memory space parsed as syscall's parameter (access_ok()), 
		I copy current process's task_struct's deadline_1, deadline_2
		and computation_time fields to a buffer struct and afterwards, 
		I copy buffer struct fields' values to the user memory space
		pointed to by params pointer. If params is a NULL pointer,
		-EINVAL is returned and syscall is terminated. If else kernel
		has not write access for user memory space pointed to by
		params, or copying from kernel-level memory to user-level memory 
		fails, -EFAULT is returned. Else, 0 is returned and syscall is
		terminated.

	sys_get_score():
		Firstly, I use current_kernel_time() in order to obtain and
		store (into D3) the kernel time the moment this syscall has occured.
		Afterwards, I compare it to deadline_1 (D1) and deadline_2 (D2), so 
		as to decide whether score to be returned will be 100 points (D1>D3), 
		0 points (D3>D2) or something between (any other case).

step 5: I implemented a testfile (named test.c), where I tested the
	correct function of the syscall-implementations described above. Syscalls are
	called five times in total. Each time, program sleeps for different number of
	seconds, simulating the actual computation time of the process. First two
	sleep scenarios are simulating the case when process' actual computation time is smaller
	or equal to deadline_1 (100 is returned), third scenario is simulating the case when process' actual
	computation time is between deadline_1 and deadline_2, while fourth and fifth
	scenarios are simulating the case when process' actual computation time is
	greater or equal to deadline_1 (0 is returned). 



