#define GLOBAL_VARIABLES_HERE

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


PUBLIC	PROCESS proc_table[NR_TASKS + NR_PROCS];

PUBLIC  TASK  task_table[NR_TASKS] = {{task_tty, TIMESLICE_TASK, 0, STACK_SIZE_TTY, "tty"},  //进程体、时间片、tty、堆栈、进程名
                                      {task_sys, TIMESLICE_TASK, 0, STACK_SIZE_SYS, "sys"},
                                      {task_hd, TIMESLICE_TASK, 0, STACK_SIZE_HD,  "hd"},
                                      {task_fs, TIMESLICE_TASK, 1, STACK_SIZE_FS,  "fs"}};


PUBLIC  TASK  user_proc_table[NR_NATIVE_PROCS] = {{Init, TIMESLICE_USER_PROC, 0, STACK_SIZE_INIT, "INIT"},
											{TestA, 0, 0, STACK_SIZE_TESTA, "TestA"},
					   						{TestB, TIMESLICE_USER_PROC, 2, STACK_SIZE_TESTB, "TestB"},
					  						{TestC, 0, 0, STACK_SIZE_TESTC, "TestC"},
                                           	{test_fs, TIMESLICE_USER_PROC, 1, STACK_SIZE_TESTFS, "user proc"}};


PUBLIC	system_call  sys_call_table[NR_SYS_CALL] = {sys_printx, 
                                                    sys_sendrec,
                                                    sys_get_ticks,
													sys_test};


PUBLIC  TTY             tty_table[NR_CONSOLES];
PUBLIC  CONSOLE         console_table[NR_CONSOLES];



struct dev_drv_map dd_map[] = {
	/* driver nr.		major device nr.
	   ----------		---------------- */
	{INVALID_DRIVER},	/**< 0 : Unused */
	{INVALID_DRIVER},	/**< 1 : Reserved for floppy driver */
	{INVALID_DRIVER},	/**< 2 : Reserved for cdrom driver */
	{TASK_HD},			/**< 3 : Hard disk */
	{TASK_TTY},			/**< 4 : TTY */
	{INVALID_DRIVER}	/**< 5 : Reserved for scsi disk driver */
};

/**
 * 6MB~7MB: buffer for FS
 */
PUBLIC	u8 *		fsbuf		= (u8*)0x600000;
PUBLIC	const int	FSBUF_SIZE	= 0x100000;

 
PUBLIC  u8   is_do_mkfs = UN_MK_FS;


