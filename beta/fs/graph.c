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

	
	/* head */ 
	SYSLOG("digraph filedesc%02d {\n", graph_idx++);
	SYSLOG("\tgraph [\n");
	SYSLOG("		rankdir = \"LR\"\n");
	SYSLOG("	];\n");
	SYSLOG("	node [\n");
	SYSLOG("		fontsize = \"16\"\n");
	SYSLOG("		shape = \"ellipse\"\n");
	SYSLOG("	];\n");
	SYSLOG("	edge [\n");
	SYSLOG("	];\n");   


	int k;
	PROCESS* p_proc = proc_table;
	SYSLOG("\n\tsubgraph cluster_0 {\n");
	for (i = 0; i < NR_TASKS + NR_PROCS; i++,p_proc++) {
		/* skip procs which open no files */
		for (k = 0; k < NR_FILES; k++) {
			if (p_proc->filp[k] != 0)
				break;
		}
		if (k == NR_FILES)
			continue;

		SYSLOG("\t\t\"proc%d\" [\n", i);
		SYSLOG("\t\t\tlabel = \"<f0>%s (%d) "
		       "|<f1> p_flags:%d(%s)",
		       p_proc->p_name,
		       i,
		       p_proc->p_flags,
		       proc_flags[p_proc->p_flags]);
		int fnr = 3;
		for (k = 0; k < NR_FILES; k++) {
			if (p_proc->filp[k] == 0)
				continue;

			int fdesc_tbl_idx = p_proc->filp[k] - f_desc_table;
			SYSLOG("\t|<f%d> filp[%d]: %d",
			       fnr,
			       k,
			       fdesc_tbl_idx);
			pfm[pfm_idx].pid = i;
			pfm[pfm_idx].filp = fnr;
			pfm[pfm_idx].desc = fdesc_tbl_idx;
			fnr++;
			pfm_idx++;
		}

		SYSLOG("\t\"\n");
		SYSLOG("\t\t\tshape = \"record\"\n");
		SYSLOG("\t\t];\n");
	}
	SYSLOG("\t\tlabel = \"procs\";\n");
	SYSLOG("\t}\n");



}
