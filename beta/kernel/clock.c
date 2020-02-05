#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proto.h"
#include "global.h"


/************************************************
 *
 * 时钟中断
 * idt 已初始化， 修改 irq_table 中的指向
 ***********************************************/
PUBLIC void clock_handler(int irq)
{
        ticks ++;
        p_proc_ready->ticks--;

        if(k_reenter!=0) // k_reenter >0,即是重入，不进行调度
        {
            return;
        }

	schedule();
}


PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}


PUBLIC void init_clock()
{
        u32 divisor = TIMER_FREQ/HZ;
       
        /* 初始化 8253 PIT */ //可编程间隔定时器 PIT (Programmable Interval Timer)
        out_byte(TIMER_MODE, RATE_GENERATOR);
        
        out_byte(TIMER0, (u8) (divisor&0xff));
        out_byte(TIMER0, (u8) (divisor >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler);      /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                          /* 让8259A可以接收时钟中断 */
}

