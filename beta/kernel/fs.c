#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "hd.h"
#include "proto.h"




/*****************************************************************************
 * task_fs
 * <Ring 1> The main loop of TASK FS.
 *****************************************************************************/
PUBLIC void task_fs()
{
	printl("Task FS begins.\n");

	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	send_recv(BOTH, TASK_HD, &driver_msg);

	spin("FS");
}
