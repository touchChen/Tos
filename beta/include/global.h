#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int			disp_pos;
EXTERN	u8			gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];  // GDT_SIZE = 128, 为了后续可以继续添加描述符
EXTERN	u8			idt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	TSS			tss;

EXTERN	PROCESS*	p_proc_ready;
EXTERN	PROCESS		proc_table[NR_TASKS_AND_PROCS];

EXTERN	char		task_stack[STACK_SIZE_TOTAL];
EXTERN	u32			k_reenter;
EXTERN  int         ticks;

EXTERN  irq_handler     irq_table[NR_IRQ];


EXTERN  TASK            task_table[];
EXTERN  TASK            user_proc_table[];
EXTERN	TTY				tty_table[];
EXTERN  CONSOLE         console_table[];


EXTERN  int             nr_current_console;


/* FS */
EXTERN	struct file_desc	f_desc_table[NR_FILE_DESC];
EXTERN	struct inode		inode_table[NR_INODE];
EXTERN	struct super_block	super_block[NR_SUPER_BLOCK];
extern	u8 *				fsbuf;
extern	const int			FSBUF_SIZE;
EXTERN	MESSAGE				fs_msg;
EXTERN	PROCESS *	        pcaller;
EXTERN	struct inode *		root_inode;
extern	struct dev_drv_map	dd_map[];
EXTERN  u8   				is_do_mkfs;


/* mem */
extern	u8 * 	kernelfile_phyaddr;
extern  int  	kernelfile_phyaddr_len;
extern	u8 * 	bss_clean_addr;
extern  int  	bss_clean_len;





