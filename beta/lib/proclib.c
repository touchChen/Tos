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
#include "proto.h"


/*****************************************************************************
 * Get the PID.
 * 
 * @return The PID.
 *****************************************************************************/
PUBLIC int getpid()
{
	MESSAGE msg;
	msg.type = GET_PID;

	send_recv(BOTH, TASK_SYS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.PID;
}



PUBLIC int get_u_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}
