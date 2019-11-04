#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"



/*======================================================================*
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	p_proc_ready++;
	if (p_proc_ready >= proc_table + NR_TASKS)
		p_proc_ready = proc_table;
}
