#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PRIVATE void process_start();

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");
        
        put_irq_handler(CLOCK_IRQ, clock_handler);
        enable_irq(CLOCK_IRQ);
        
        enable_irq(KEYBOARD_IRQ);

        process_start();
	while(1){}
}


PRIVATE void process_start()
{
        TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for(i=0;i<NR_TASKS;i++){
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;  //在GDT中选择子

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],     
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;         //左移5位->DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;


		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;  //进程中的esp
		p_proc->regs.eflags = 0x0202;	// IF=1, IOPL=1, bit 2 is always 1. default 0x1202

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	k_reenter = 0;

	p_proc_ready	= proc_table; 
	restart();
	
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int i = 0;
	while(1){
                disp_str("|A");
		disp_int_c(i++);
		disp_str(".");
		delay(1);
                i%=256;           
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0;
	while(1){
		disp_str("@B");
		disp_int_c(i++);
		disp_str(".");
		delay(2);
                i%=256;
	}
}


/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{
	int i = 0;
	while(1){
		disp_str("%C");
		disp_int_c(i++);
		disp_str(".");
		delay(1);
                i%=256;
	}
}



