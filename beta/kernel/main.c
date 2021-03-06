#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


PRIVATE void init_process();

/*
 * 内核主程序
 */
PUBLIC int kernel_main()
{
		k_reenter = 0;             // 判断是否重入
		ticks = 0;
                      
		init_process();
		p_proc_ready = proc_table; 

		init_clock();

		restart();

		disp_str("-----\"kernel\" Never run to here-----\n");
        while(1){}
}


PRIVATE void init_process()
{
	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16			selector_ldt	= SELECTOR_LDT_FIRST;
	int i;

	u8              privilege;
	u8              rpl;
	int             eflags;
	for(i=0;i<NR_TASKS + NR_PROCS;i++){
		if (i >= NR_TASKS + NR_NATIVE_PROCS) {
			p_proc->p_flags = FREE_SLOT;
			p_proc->ldt_sel = selector_ldt;

			if(i<NR_TASKS + NR_PROCS-1)
			{
				p_proc++;  
				selector_ldt += 1 << 3;
			}
			continue;
		}

		if (i < NR_TASKS) {     /* 任务 */
			p_task    = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl       = RPL_TASK;
			eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
		}else{
			p_task    = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_USER;
			rpl       = RPL_USER;
			eflags    = 0x0202; /* IF=1, bit 2 is always 1 */
 		}

		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;						// pid
		p_proc->priority = p_proc->ticks = p_task->priority;
		p_proc->nr_tty = p_task->tty;

		p_proc->p_parent = NO_TASK;

		p_proc->ldt_sel = selector_ldt;  		//在GDT中选择子 一个进程一个选择子

		/*
		// 描述符已在 protect.c 中初始化
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;         //左移5位->DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		*/

		if (strcmp(p_task->name, "INIT") != 0) {
			p_proc->ldts[INDEX_LDT_C]  = gdt[SELECTOR_KERNEL_CS >> 3];
			p_proc->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

			/* change the DPLs */
			p_proc->ldts[INDEX_LDT_C].attr1  = DA_C   | privilege << 5;
			p_proc->ldts[INDEX_LDT_RW].attr1 = DA_DRW | privilege << 5;
		}
		else {		/* INIT process */
			unsigned int k_base;
			unsigned int k_limit;
			int ret = get_kernel_map(&k_base, &k_limit);
			assert(ret == 0);
           
			init_descriptor(&p_proc->ldts[INDEX_LDT_C],
				  0, 
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_C | privilege << 5);

			init_descriptor(&p_proc->ldts[INDEX_LDT_RW],
				  0,
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_DRW | privilege << 5);
		}

		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;  //进程中的esp
		//p_proc->regs.eflags = 0x2202;	// IF=1, IOPL=1, bit 2 is always 1. default 0x1202  0x0202
		p_proc->regs.eflags = eflags;


		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;


		int fi;
		for (fi = 0; fi < NR_FILES; fi++) {
			p_proc->filp[fi] = 0;
		}

		p_task_stack -= p_task->stacksize;

		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
        
}






