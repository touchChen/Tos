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


static char _buf[SECTOR_SIZE];

struct proc_fdesc_map {
	int pid;	/* PID */
	int filp;	/* idx of proc_table[pid].filp[] */
	int desc;	/* idx of f_desc_table[] */
} pfm[256]; //
int pfm_idx = 0;

struct fdesc_inode_map {
	int desc;	/* idx of f_desc_table[] */
	int inode;	/* idx of inode_table[] */
} fim[256]; //256
int fim_idx = 0;

/*****************************************************************************
 * Output a dot graph.
 *****************************************************************************/
PUBLIC void dump_fd_graph(const char * fmt, ...)
{
	int i;
	char title[STR_DEFAULT_LEN];

	va_list arg = (va_list)((char*)(&fmt) + 4); /**
						     					 * 4: size of `fmt' in
						     				     *    the stack
						                         */
	i = vsprintf(title, fmt, arg);
	assert(strlen(title) == i);

	static int graph_idx = 0;
	char filename[MAX_FILENAME_LEN+1];

	char * proc_flags[32];
	proc_flags[0]    = "RUNNING";
	proc_flags[0x02] = "SENDING";
	proc_flags[0x04] = "RECEIVING";
	proc_flags[0x08] = "WAITING";
	proc_flags[0x10] = "FREE_SLOT";

}
