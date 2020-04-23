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
 * open/create a file.
 * 
 * @param pathname  The full path of the file to be opened/created.
 * @param flags     O_CREAT, O_RDWR, etc.
 * 
 * @return File descriptor if successful, otherwise -1.
 *****************************************************************************/
PUBLIC int open(const char *pathname, int flags)
{
	MESSAGE msg;

	msg.type		= OPEN;

	msg.PATHNAME	= (void*)pathname;
	msg.FLAGS		= flags;
	msg.NAME_LEN	= strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.FD;
}


/*****************************************************************************
 * Close a file descriptor.
 * 
 * @param fd  File descriptor.
 * 
 * @return Zero if successful, otherwise -1.
 *****************************************************************************/
PUBLIC int close(int fd)
{
    if (fd >= 0)
    {
		MESSAGE msg;
		msg.type   = CLOSE;
		msg.FD     = fd;

		send_recv(BOTH, TASK_FS, &msg);

		return msg.RETVAL;
    }
	
	return -1;
}


/*****************************************************************************
 * Read from a file descriptor.
 * 
 * @param fd     File descriptor.
 * @param buf    Buffer to accept the bytes read.
 * @param count  How many bytes to read.
 * 
 * @return  On success, the number of bytes read are returned.
 *          On error, -1 is returned.
 *****************************************************************************/
PUBLIC int read(int fd, void *buf, int count)
{
	MESSAGE msg;
	msg.type = READ;
	msg.FD   = fd;
	msg.BUF  = buf;
	msg.CNT  = count;
    
	send_recv(BOTH, TASK_FS, &msg);

	return msg.CNT;
}


/*****************************************************************************
 * Write to a file descriptor.
 * 
 * @param fd     File descriptor.
 * @param buf    Buffer including the bytes to write.
 * @param count  How many bytes to write.
 * 
 * @return  On success, the number of bytes written are returned.
 *          On error, -1 is returned.
 *****************************************************************************/
PUBLIC int write(int fd, const void *buf, int count)
{
	MESSAGE msg;
	msg.type = WRITE;
	msg.FD   = fd;
	msg.BUF  = (void*)buf;
	msg.CNT  = count;

	send_recv(BOTH, TASK_FS, &msg);

	return msg.CNT;
}


/*****************************************************************************
 * Delete a file.
 * 
 * @param pathname  The full path of the file to delete.
 * 
 * @return Zero if successful, otherwise -1.
 *****************************************************************************/
PUBLIC int unlink(const char * pathname)
{
	MESSAGE msg;
	msg.type   		= UNLINK;

	msg.PATHNAME	= (void*)pathname;
	msg.NAME_LEN	= strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);

	return msg.RETVAL;
}




