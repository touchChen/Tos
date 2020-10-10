#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC int sys_test()
{

	unsigned int k_base;
	unsigned int k_limit;
	int ret = get_kernel_map(&k_base, &k_limit);

	printl("bl:0x%x, 4k:0x%x",(k_base + k_limit),(k_base + k_limit) >> LIMIT_4K_SHIFT);
	assert(ret == 0);
	

	return 0;
}
