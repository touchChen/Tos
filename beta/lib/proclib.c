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
 *                                send_recv   发起进程
 * <Ring 1~3> IPC syscall.
 *
 * It is an encapsulation of `sendrec', sendrec: 系统中断
 * invoking `sendrec' directly should be avoided
 *
 * @param function  SEND, RECEIVE or BOTH
 * @param src_dest  The caller's proc_nr
 * @param msg       Pointer to the MESSAGE struct
 * 
 * @return always 0.
 *****************************************************************************/
PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg)
{
	int ret = 0;

	if (function == RECEIVE)
		memset(msg, 0, sizeof(MESSAGE));  // 清空消息体

	switch (function) {
		case BOTH:
			if(src_dest==TASK_MM)
			{
				//printf("before...\n");
			}
			ret = sendrec(SEND, src_dest, msg);
			if(src_dest==TASK_MM)
			{
				printf("after...\n");
			}
			if (ret == 0)
				ret = sendrec(RECEIVE, src_dest, msg);
			break;
		case SEND:
		case RECEIVE:
			ret = sendrec(function, src_dest, msg);
			break;
		default:
			assert((function == BOTH) ||
			       (function == SEND) || (function == RECEIVE));
			break;
	}

	return ret;
}


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
	//reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}
