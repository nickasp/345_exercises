/****** ASPRIDIS NIKOLAOS csd5178 *******/
/*** hy-345 2024 ************************/

#ifndef __ASM_GENERIC_SYSCALLS_H
#define __ASM_GENERIC_SYSCALLS_H

#include <linux/compiler.h>
#include <linux/linkage.h>

/* --- mine --- */
#include <linux/d_params.h>
/* ------------ */

/*
 * Calling conventions for these system calls can differ, so
 * it's possible to override them.
 */
#ifndef sys_clone
asmlinkage long sys_clone(unsigned long clone_flags, unsigned long newsp,
			void __user *parent_tid, void __user *child_tid,
			struct pt_regs *regs);
#endif

#ifndef sys_fork
asmlinkage long sys_fork(struct pt_regs *regs);
#endif

#ifndef sys_vfork
asmlinkage long sys_vfork(struct pt_regs *regs);
#endif

#ifndef sys_execve
asmlinkage long sys_execve(const char __user *filename,
			   const char __user *const __user *argv,
			   const char __user *const __user *envp,
			   struct pt_regs *regs);
#endif

#ifndef sys_mmap2
asmlinkage long sys_mmap2(unsigned long addr, unsigned long len,
			unsigned long prot, unsigned long flags,
			unsigned long fd, unsigned long pgoff);
#endif

#ifndef sys_mmap
asmlinkage long sys_mmap(unsigned long addr, unsigned long len,
			unsigned long prot, unsigned long flags,
			unsigned long fd, off_t pgoff);
#endif

#ifndef sys_sigaltstack
asmlinkage long sys_sigaltstack(const stack_t __user *, stack_t __user *,
			struct pt_regs *);
#endif

#ifndef sys_rt_sigreturn
asmlinkage long sys_rt_sigreturn(struct pt_regs *regs);
#endif

#ifndef sys_rt_sigsuspend
asmlinkage long sys_rt_sigsuspend(sigset_t __user *unewset, size_t sigsetsize);
#endif

#ifndef sys_rt_sigaction
asmlinkage long sys_rt_sigaction(int sig, const struct sigaction __user *act,
			 struct sigaction __user *oact, size_t sigsetsize);
#endif

/*------------- mine -------------------------------------------*/
#ifndef sys_set_params
asmlinkage long sys_set_params(long deadline_1, long deadline_2, long computation_time);
#endif

#ifndef sys_get_params
asmlinkage long sys_get_params(struct d_params *params);
#endif

#ifndef sys_get_score
asmlinkage long sys_get_score();
#endif
/*--------------------------------------------------------------*/

#endif /* __ASM_GENERIC_SYSCALLS_H */
