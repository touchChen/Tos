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
 * Write log directly to the disk by sending message to FS.
 * 
 * @param fmt The format string.
 * 
 * @return How many chars have been printed.
 *****************************************************************************/
PUBLIC int syslog(const char *fmt, ...)
{

	int i;
	char buf[STR_DEFAULT_LEN];

	va_list arg = (va_list)((char*)(&fmt) + 4); /**
						     * 4: size of `fmt' in
						     *    the stack
						     */
	i = vsprintf(buf, fmt, arg);
	assert(strlen(buf) == i);

	if (getpid() == TASK_FS) { /* in FS */
		return disklog(buf);
	}
	else {			/* any proc which is not FS */              
		MESSAGE msg;
		msg.type = DISK_LOG;
		msg.BUF= buf;
		msg.CNT = i;
		send_recv(BOTH, TASK_FS, &msg);
		if (i != msg.CNT) {
			panic("failed to write log");
		}

		return msg.RETVAL;
	}
}


/*****************************************************************************
 * Read from fs log.
 * 
 * @param buf    Buffer to accept the bytes read.
 * @return position of log file.
 *****************************************************************************/
PUBLIC int readlog(void *buf)
{
	MESSAGE msg;
	msg.type = READ_LOG;
	msg.BUF  = buf;
    
	send_recv(BOTH, TASK_FS, &msg);

	return msg.POS;
}



/*****************************************************************************
 * @param buf    Buffer to write.
 * @return position of log file.
 *****************************************************************************/
PUBLIC void graphlog()
{
	MESSAGE msg;
	msg.type = GRAPH_LOG;
    
	send_recv(BOTH, TASK_FS, &msg);

}

