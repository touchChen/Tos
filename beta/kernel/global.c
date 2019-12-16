#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


PUBLIC  TASK  task_table[NR_TASKS] = {{task_tty, 20, 0, STACK_SIZE_TTY, "tty"},  //进程体、时间片、tty、堆栈、进程名
                                      {task_sys, 20, 0, STACK_SIZE_SYS, "sys"}};

PUBLIC  TASK  user_proc_table[NR_PROCS] = {{TestA, 1, 0, STACK_SIZE_TESTA, "TestA"},
					   {TestB, 0, 0, STACK_SIZE_TESTB, "TestB"},
					   {TestC, 0, 0, STACK_SIZE_TESTC, "TestC"}};


PUBLIC	system_call  sys_call_table[NR_SYS_CALL] = {sys_printx, 
                                                    sys_sendrec,
                                                    sys_get_ticks};


PUBLIC  TTY             tty_table[NR_CONSOLES];
PUBLIC  CONSOLE         console_table[NR_CONSOLES];
