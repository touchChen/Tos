#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "keyboard.h"
#include "global.h"
#include "elf.h"
#include "proto.h"

PRIVATE void cleanup(PROCESS * proc);


/*****************************************************************************
 * Perform the fork() syscall.
 * 
 * @return  Zero if success, otherwise -1.
 *****************************************************************************/
PUBLIC int do_fork()
{
	/* find a free slot in proc_table */
	PROCESS * p = proc_table;
	int i;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++,p++)
		if (p->p_flags == FREE_SLOT)
			break;

	int child_pid = i;
	assert(p == &proc_table[child_pid]);
	assert(child_pid >= NR_TASKS + NR_NATIVE_PROCS);
	if (i == NR_TASKS + NR_PROCS) /* no free slot */
		return -1;
	assert(i < NR_TASKS + NR_PROCS);

	printl("MM#ldt_sel=0%d\n",p->ldt_sel);


	/******** 进程表 ************/
	/* duplicate the process table */
	int pid = mm_msg.source;
	u16 child_ldt_sel = p->ldt_sel;
	*p = proc_table[pid];    // 修改指针指向的内容， 复制父进程信息到子进程中
	p->ldt_sel = child_ldt_sel;  // 恢复 子进程的选择子
	p->p_parent = pid;
	sprintf(p->p_name, "%s_%d", proc_table[pid].p_name, child_pid);

	

	/********** 描述符 获取父进程内存状况 ***********/
	/* duplicate the process: T, D & S */
	DESCRIPTOR * ppd;

	/* Text segment */
	ppd = &proc_table[pid].ldts[INDEX_LDT_C];
	/* base of T-seg, in bytes */
	int caller_T_base  = reassembly(ppd->base_high, 24,
					ppd->base_mid,  16,
					ppd->base_low);
	/* limit of T-seg, in 1 or 4096 bytes,
	   depending on the G bit of descriptor */
	int caller_T_limit = reassembly(0, 0,
					(ppd->limit_high_attr2 & 0xF), 16,
					ppd->limit_low);
	/* size of T-seg, in bytes */
	int caller_T_size  = ((caller_T_limit + 1) *
			      ((ppd->limit_high_attr2 & (DA_LIMIT_4K >> 8)) ?
			       4096 : 1));

	printl("MM# caller_T_base=0x%x, limit=0x%x, size=0x%x\n",caller_T_base,caller_T_limit,caller_T_size);

	/* Data & Stack segments */
	ppd = &proc_table[pid].ldts[INDEX_LDT_RW];
	/* base of D&S-seg, in bytes */
	int caller_D_S_base  = reassembly(ppd->base_high, 24,
					  ppd->base_mid,  16,
					  ppd->base_low);
	/* limit of D&S-seg, in 1 or 4096 bytes,
	   depending on the G bit of descriptor */
	int caller_D_S_limit = reassembly((ppd->limit_high_attr2 & 0xF), 16,
					  0, 0,
					  ppd->limit_low);
	/* size of D&S-seg, in bytes */
	int caller_D_S_size  = ((caller_T_limit + 1) *
				((ppd->limit_high_attr2 & (DA_LIMIT_4K >> 8)) ?
				 4096 : 1));

	/* we don't separate T, D & S segments, so we have: */
	assert((caller_T_base  == caller_D_S_base ) &&
	       (caller_T_limit == caller_D_S_limit) &&
	       (caller_T_size  == caller_D_S_size ));


	/*************** 子进程内存分配 ********************/
	/* base of child proc, T, D & S segments share the same space,
	   so we allocate memory just once */
	int child_base = alloc_mem(child_pid, caller_T_size);
	/* int child_limit = caller_T_limit; */
	printl("MM# 0x%x <- 0x%x (0x%x bytes)\n",
	       child_base, caller_T_base, caller_T_size);

	/* child is a copy of the parent */
	phys_copy((void*)child_base, (void*)caller_T_base, caller_T_size);


	/**************** 进程描述符 *********************/
	/* child's LDT */
	init_descriptor(&p->ldts[INDEX_LDT_C],
		  child_base,
		  (PROC_IMAGE_SIZE_DEFAULT - 1) >> LIMIT_4K_SHIFT,
		  DA_LIMIT_4K | DA_32 | DA_C | PRIVILEGE_USER << 5);
	init_descriptor(&p->ldts[INDEX_LDT_RW],
		  child_base,
		  (PROC_IMAGE_SIZE_DEFAULT - 1) >> LIMIT_4K_SHIFT,
		  DA_LIMIT_4K | DA_32 | DA_DRW | PRIVILEGE_USER << 5);
 

	/* tell FS, see fs_fork() */
	MESSAGE msg2fs;
	msg2fs.type = FORK;
	msg2fs.PID = child_pid;
	send_recv(BOTH, TASK_FS, &msg2fs);

	/* child PID will be returned to the parent proc */
	mm_msg.PID = child_pid;

	/* birth of the child */
	MESSAGE m;
	m.type = SYSCALL_RET;
	m.RETVAL = 0;
	m.PID = 0;
	send_recv(SEND, child_pid, &m);  // 子进程返回
	
	return 0;   /* 父进程 返回 */
}


/*****************************************************************************
 * Perform the exec() system call.
 * 
 * @return  Zero if successful, otherwise -1.
 *****************************************************************************/
PUBLIC int do_exec()
{
	/* get parameters from the message */
	int name_len = mm_msg.NAME_LEN;	/* length of filename */
	int src = mm_msg.source;	/* caller proc nr. */
	assert(name_len < MAX_PATH);

	char pathname[MAX_PATH];
	phys_copy((void*)va2la(TASK_MM, pathname),
		  (void*)va2la(src, mm_msg.PATHNAME),
		  name_len);
	pathname[name_len] = 0;	/* terminate the string */

	/* get the file size */
	struct stat s;
	int ret = stat(pathname, &s);
	if (ret != 0) {
		printl("MM# do_stat of (%s) returns error\n", pathname);
		return -1;
	}

	/* read the file */
	int fd = open(pathname, O_RDWR);
	if (fd == -1)
		return -1;
	assert(s.st_size < MMBUF_SIZE);
	read(fd, mmbuf, s.st_size);
	close(fd);

	/* overwrite the current proc image with the new one */
	Elf32_Ehdr* elf_hdr = (Elf32_Ehdr*)(mmbuf);
	int i;
	for (i = 0; i < elf_hdr->e_phnum; i++) {
		Elf32_Phdr* prog_hdr = (Elf32_Phdr*)(mmbuf + elf_hdr->e_phoff +
			 			(i * elf_hdr->e_phentsize));
		if (prog_hdr->p_type == PT_LOAD) {
			assert(prog_hdr->p_vaddr + prog_hdr->p_memsz <
				PROC_IMAGE_SIZE_DEFAULT);
			phys_copy((void*)va2la(src, (void*)prog_hdr->p_vaddr),
				  (void*)va2la(TASK_MM,
						 mmbuf + prog_hdr->p_offset),
				  prog_hdr->p_filesz);
		}
	}

	/* setup the arg stack */
	int orig_stack_len = mm_msg.BUF_LEN;
	char stackcopy[PROC_ORIGIN_STACK];
	phys_copy((void*)va2la(TASK_MM, stackcopy),
		  (void*)va2la(src, mm_msg.BUF),
		  orig_stack_len);

	u8 * orig_stack = (u8*)(PROC_IMAGE_SIZE_DEFAULT - PROC_ORIGIN_STACK);

	int delta = (int)orig_stack - (int)mm_msg.BUF;

	int argc = 0;
	if (orig_stack_len) {	/* has args */
		char **q = (char**)stackcopy;
		for (; *q != 0; q++,argc++)
			*q += delta;
	}

	phys_copy((void*)va2la(src, orig_stack),
		  (void*)va2la(TASK_MM, stackcopy),
		  orig_stack_len);

	proc_table[src].regs.ecx = argc; /* argc */
	proc_table[src].regs.eax = (u32)orig_stack; /* argv */

	/* setup eip & esp */
	proc_table[src].regs.eip = elf_hdr->e_entry; /* @see _start.asm */
	proc_table[src].regs.esp = PROC_IMAGE_SIZE_DEFAULT - PROC_ORIGIN_STACK;

	strcpy(proc_table[src].p_name, pathname);

	return 0;
}



/*****************************************************************************
 * Perform the exit() syscall.
 *
 * If proc A calls exit(), then MM will do the following in this routine:
 *     <1> inform FS so that the fd-related things will be cleaned up
 *     <2> tell TASK_SYS (TODO)
 *     <3> free A's memory
 *     <4> set A.exit_status, which is for the parent
 *     <5> depends on parent's status. if parent (say P) is:
 *           (1) WAITING
 *                 - clean P's WAITING bit, and
 *                 - send P a message to unblock it
 *                   {P's wait() call is done}
 *                 - release A's proc_table[] slot
 *                   {A's exit() call is done}
 *           (2) not WAITING
 *                 - set A's HANGING bit
 *                   {things will be done at do_wait()::comment::<1>}
 *     <6> iterate proc_table[], if proc B is found as A's child, then:
 *           (1) make INIT the new parent of B, and
 *           (2) if INIT is WAITING and B is HANGING, then:
 *                 - clean INIT's WAITING bit, and
 *                 - send INIT a message to unblock it
 *                   {INIT's wait() call is done}
 *                 - release B's proc_table[] slot
 *                   {B's exit() call is done}
 *               else
 *                 if INIT is WAITING but B is not HANGING, then
 *                     - B will call exit() and things will be done at
 *                       do_exit()::comment::<5>::(1)
 *                 if B is HANGING but INIT is not WAITING, then
 *                     - INIT will call wait() and things will be doen at
 *                       do_wait()::comment::<1>
 *
 * TERMs:
 *     - HANGING: everything except the proc_table entry has been cleaned up.
 *     - WAITING: a proc has at least one child, and it is waiting for the
 *                child(ren) to exit()
 *     - zombie(僵尸): say P has a child A, A will become a zombie if
 *         - A exit(), and
 *         - P does not wait(), neither does it exit(). that is to say, P just
 *           keeps running without terminating itself or its child
 *         - @see Linux manpage for more details:
 *             - man 2 exit
 *             - man 3 exit
 *             - man 2 wait
 * 
 * @param status  Exiting status for parent.
 * 
 *****************************************************************************/
PUBLIC void do_exit(int status)
{
	int i;
	int pid = mm_msg.source; /* PID of caller */
	int parent_pid = proc_table[pid].p_parent;
	PROCESS * p = &proc_table[pid];
	/* struct proc * p_parent = &proc_table[parent_pid]; */

	/* tell FS, see fs_exit() */
	MESSAGE msg2fs;
	msg2fs.type = EXIT;
	msg2fs.PID = pid;
	send_recv(BOTH, TASK_FS, &msg2fs);

	/**
	 * @todo should also send a message to TASK_SYS to do some cleanup work.
	 *       e.g. if the proc is killed by another proc, TASK_SYS should
	 *            check if the proc happens to be SENDING a message, if so,
	 *            the proc should be removed from the sending queue.
	 * @see MINIX::src/kernel/system.c:do_xit()
	 */

	free_mem(pid);

	p->exit_status = status;

	if (proc_table[parent_pid].p_flags & WAITING) { /* parent is waiting */
		printl("MM# do_exit():: %s (%d) is WAITING, %s (%d) will be cleaned up.\n",
		       proc_table[parent_pid].p_name, parent_pid,
		       p->p_name, pid);
		printl("MM# do_exit():: proc_table[%d].p_flags: 0x%x\n", parent_pid,
		       proc_table[parent_pid].p_flags);
		
		proc_table[parent_pid].p_flags &= ~WAITING;
		cleanup(&proc_table[pid]);  // parent must wait...
	}
	else { /* parent is not waiting */  //没有清空进程表里的资源
		printl("MM# do_exit():: %s (%d) is not WAITING, %s (%d) will be HANGING.\n",
		       proc_table[parent_pid].p_name, parent_pid,
		       p->p_name, pid);
		proc_table[pid].p_flags |= HANGING;
	}

	/* if the proc has any child, make INIT the new parent */
	for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
		if (proc_table[i].p_parent == pid) { /* is a child */
			proc_table[i].p_parent = INIT; /* FIXME: make sure INIT always waits */
			
			if ((proc_table[INIT].p_flags & WAITING) &&
			    (proc_table[i].p_flags & HANGING)) {
				/* assert(proc_table[INIT].p_flags & RECEIVING); */
				proc_table[INIT].p_flags &= ~WAITING;
				cleanup(&proc_table[i]);
				assert(0);
			}
			else {
				/* assert(0); */
			}
		}
	}
}

/*****************************************************************************
 * Do the last jobs to clean up a proc thoroughly:
 *     - Send proc's parent a message to unblock it, and
 *     - release proc's proc_table[] entry
 * 
 * @param proc  Process to clean up.
 *****************************************************************************/
PRIVATE void cleanup(PROCESS * proc)
{
	MESSAGE msg2parent;
	msg2parent.type = SYSCALL_RET;
	msg2parent.PID = proc2pid(proc);
	msg2parent.STATUS = proc->exit_status;
	send_recv(SEND, proc->p_parent, &msg2parent);

	proc->p_flags = FREE_SLOT;

	//printl("{MM} ((--cleanup():: %s (%d) has been cleaned up.--))\n", proc->p_name, proc2pid(proc));
}

/*****************************************************************************
 * Perform the wait() syscall.
 *
 * If proc P calls wait(), then MM will do the following in this routine:
 *     <1> iterate proc_table[],
 *         if proc A is found as P's child and it is HANGING
 *           - reply to P (cleanup() will send P a messageto unblock it)
 *             {P's wait() call is done}
 *           - release A's proc_table[] entry
 *             {A's exit() call is done}
 *           - return (MM will go on with the next message loop)
 *     <2> if no child of P is HANGING
 *           - set P's WAITING bit
 *             {things will be done at do_exit()::comment::<5>::(1)}
 *     <3> if P has no child at all
 *           - reply to P with error
 *             {P's wait() call is done}
 *     <4> return (MM will go on with the next message loop)
 *
 *****************************************************************************/
PUBLIC void do_wait()
{
	int pid = mm_msg.source;

	int i;
	int children = 0;
	PROCESS * p_proc = proc_table;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++,p_proc++) {
		if (p_proc->p_parent == pid) {
			children++;
			if (p_proc->p_flags & HANGING) {
				cleanup(p_proc);
				return;
			}
		}
	}

	if (children) {
		/* has children, but no child is HANGING */
		proc_table[pid].p_flags |= WAITING;
	}
	else {
		MESSAGE msg;
		msg.type = SYSCALL_RET;
		msg.PID = NO_TASK;
		send_recv(SEND, pid, &msg);
	}
}



/*****************************************************************************
 * <Ring 0~1> The boot parameters have been saved by LOADER.
 *            We just read them out.
 *
 * @param pbp  Ptr to the boot params structure
 *****************************************************************************/
PUBLIC void get_boot_params(struct boot_params * pbp)
{
	/**
	 * Boot params should have been saved at BOOT_PARAM_ADDR.
	 */
	int * p = (int*)BOOT_PARAM_ADDR;
	assert(p[BI_MAG] == BOOT_PARAM_MAGIC);

	pbp->mem_size = p[BI_MEM_SIZE];
	pbp->kernel_file = (unsigned char *)(p[BI_KERNEL_FILE]);

	
	/**
	 * the kernel file should be a ELF executable,
	 * check it's magic number
	 */
	int elf_mag = ELFMAG;
	assert(memcmp(pbp->kernel_file, (void *)&elf_mag, SELFMAG) == 0);

}


/*****************************************************************************
 * <Ring 0~1> Parse the kernel file, get the memory range of the kernel image.
 *
 * - The meaning of `base':		base => first_valid_byte
 * - The meaning of `limit':	base + limit => last_valid_byte
 *
 * @param b   Memory base of kernel.
 * @param l   Memory limit of kernel.
 *****************************************************************************/
PUBLIC int get_kernel_map(unsigned int * b, unsigned int * l)
{
	struct boot_params bp;
	get_boot_params(&bp);

	Elf32_Ehdr* elf_header = (Elf32_Ehdr*)(bp.kernel_file);

	/* the kernel file should be in ELF format */
	
	int elf_mag = ELFMAG;
	if (memcmp(elf_header->e_ident, (void *)&elf_mag, SELFMAG) != 0)
		return -1;
	

	*b = ~0;
	unsigned int t = 0;
	int i;
	for (i = 0; i < elf_header->e_shnum; i++) {
		Elf32_Shdr* section_header =
			(Elf32_Shdr*)(bp.kernel_file +
				      elf_header->e_shoff +
				      i * elf_header->e_shentsize);

		if (section_header->sh_flags & SHF_ALLOC) {
			int bottom = section_header->sh_addr;
			int top = section_header->sh_addr +
				section_header->sh_size;

			if (*b > bottom)
				*b = bottom;
			if (t < top)
				t = top;
		}
	}
	assert(*b < t);
	*l = t - *b - 1;

	//printl("base:0x%x, top:0x%x, limit:0x%x\n", *b,t,*l);

	return 0;
}


