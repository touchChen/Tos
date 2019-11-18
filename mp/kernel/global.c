#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


PUBLIC  TASK  task_table[NR_TASKS] = {{task_tty, 2000, 0, STACK_SIZE_TTY, "tty"}};

PUBLIC  TASK  user_proc_table[NR_PROCS] = {{TestA, 10, 0, STACK_SIZE_TESTA, "TestA"},
					     {TestB, 1100, 2, STACK_SIZE_TESTB, "TestB"},
					     {TestC, 10, 1, STACK_SIZE_TESTC, "TestC"}};
 
PUBLIC	system_call  sys_call_table[NR_SYS_CALL] = {sys_get_ticks,
                                                    sys_write};


PUBLIC  TTY             tty_table[NR_CONSOLES];
PUBLIC  CONSOLE         console_table[NR_CONSOLES];
