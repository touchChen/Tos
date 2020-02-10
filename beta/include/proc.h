#ifndef	_TOS_PROC_H_
#define	_TOS_PROC_H_

/* the assert macro */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp)  if (exp) ; \
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif


/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;



/*********************************************************
************** process 进程 ******************************
*********************************************************/
typedef struct s_stackframe {
	u32	gs;		/* \                                    */
	u32	fs;		/* |                                    */
	u32	es;		/* |                                    */
	u32	ds;		/* |                                    */
	u32	edi;		/* |                                    */
	u32	esi;		/* | pushed by save()                   */
	u32	ebp;		/* |                                    */
	u32	kernel_esp;	/* <- 'popad' will ignore it            */
	u32	ebx;		/* |                                    */
	u32	edx;		/* |                                    */
	u32	ecx;		/* |                                    */
	u32	eax;		/* /                                    */
	u32	retaddr;	/* return addr for kernel.asm::save()   */
	u32	eip;		/* \                                    */
	u32	cs;		/* |                                    */
	u32	eflags;		/* | pushed by CPU during interrupt     */
	u32	esp;		/* |                                    */
	u32	ss;		/* /                                    */
}STACK_FRAME;




typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* ldt 在 gdt 中的选择子    gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

        int ticks;                 /* remained ticks */
        int priority;

	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */

        int  p_flags;              /**
				    * process flags. A proc is runnable iff p_flags==0
				    */
        MESSAGE * p_msg;
	int p_recvfrom;
	int p_sendto;

	int has_int_msg;           /**
				    * nonzero if an INTERRUPT occurred when
				    * the task is not ready to deal with it.
				    */

	struct s_proc * q_sending;    /**
				       * queue of procs sending messages to this proc
				       */
	struct s_proc * next_sending; /**
				       * next proc in the sending queue (q_sending)
				       */

        int nr_tty;

        struct file_desc * filp[NR_FILES];
}PROCESS;


typedef struct s_task {
	task_f	initial_eip;
        int     priority;
        int     tty;
	int	stacksize;
	char	name[32];
}TASK;


#define proc2pid(x) (x - proc_table)


/* Process */
#define SENDING   0x02	/* set when proc trying to send */
#define RECEIVING 0x04	/* set when proc trying to recv */


/* Number of tasks */
#define NR_TASKS	4

/* Number of user proc */
#define NR_PROCS	4

#define NR_TASKS_AND_PROCS    (NR_TASKS + NR_PROCS)

/* system call */
#define NR_SYS_CALL     3

/* TTY */
#define NR_CONSOLES	3	

/* stacks of tasks */
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
#define STACK_SIZE_TESTFS	0x8000
#define STACK_SIZE_TTY		0x8000
#define STACK_SIZE_SYS		0x8000
#define STACK_SIZE_HD		0x8000
#define STACK_SIZE_FS		0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_TESTA + \
                                 STACK_SIZE_TESTC + \
				 STACK_SIZE_TESTB + \
                                 STACK_SIZE_TESTFS + \
                                 STACK_SIZE_TTY + \
                                 STACK_SIZE_SYS + \
                                 STACK_SIZE_HD + \
                                 STACK_SIZE_FS)

#define ANY		(NR_TASKS + NR_PROCS + 10)
#define NO_TASK		(NR_TASKS + NR_PROCS + 20)

/* ipc */
#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

/* magic chars used by `printx' */
#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

#define INVALID_DRIVER	-20
#define INTERRUPT	-10
#define TASK_TTY	0
#define TASK_SYS	1
#define TASK_HD		2
#define TASK_FS		3



/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,

	/* FS */
	OPEN, CLOSE, READ, WRITE, LSEEK, STAT, UNLINK,

	/* TTY, SYS, FS, MM, etc */
	SYSCALL_RET,

	/* message type for drivers */
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};


#define	FD		u.m3.m3i1
#define	PATHNAME	u.m3.m3p1
#define	FLAGS		u.m3.m3i1
#define	NAME_LEN	u.m3.m3i2
#define	CNT		u.m3.m3i2
#define	REQUEST		u.m3.m3i2
#define	PROC_NR		u.m3.m3i3
#define	DEVICE		u.m3.m3i4
#define	POSITION	u.m3.m3l1
#define	BUF		u.m3.m3p2
#define	RETVAL		u.m3.m3i1

#define	STR_DEFAULT_LEN	1024


#endif /* _TOS_PROC_H_ */


