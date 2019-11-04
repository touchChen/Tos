#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];  // GDT_SIZE = 128, 为了后续可以继续添加描述符
EXTERN	u8		idt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	TSS		tss;

EXTERN	PROCESS*	p_proc_ready;
EXTERN	PROCESS		proc_table[NR_TASKS];

EXTERN	char		task_stack[STACK_SIZE_TOTAL];
EXTERN	u32		k_reenter;





#ifdef	GLOBAL_VARIABLES_HERE
PUBLIC TASK  task_table[NR_TASKS] = {{TestA, STACK_SIZE_TESTA, "TestA"},
				     {TestB, STACK_SIZE_TESTB, "TestB"},
                                     {TestC, STACK_SIZE_TESTC, "TestC"}};
#else
EXTERN TASK  task_table[NR_TASKS];

#endif
