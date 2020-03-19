#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);


/************************************************************************
 *                            
 * 终端
 ***********************************************************************/
PUBLIC void task_tty()
{ 
        TTY*	p_tty; 
        init_keyboard(); //特权级，没有问题？

        for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}
	//nr_current_console = 0;   //简洁

        select_console(0);  

	while (1) {
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) 
                {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}


PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

        /*
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;    //指针
        */
        init_screen(p_tty);
}




PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}



PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		out_char(p_tty->p_console, ch);
	}
}




PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)) {
                /*
                output[0] = key & 0xFF;
                disp_str(output);

                set_disp_pos_cursor();
               */
               put_key(p_tty, key);  // ptty 缓存数据栈
        }else 
        {
                int raw_code = key & MASK_RAW;
                switch(raw_code) {
                        case ENTER:
				put_key(p_tty, '\n');
				break;
                	case BACKSPACE:
				put_key(p_tty, '\b');
				break;
		        case UP:
		                if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
		                        //set_video_start_addr((u32)(80*15));
                                        scroll_screen(p_tty->p_console, SCR_UP);
		                }
		                break;
		        case DOWN:
		                if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
					scroll_screen(p_tty->p_console, SCR_DN);
		                }
		                break;
		        case F1:
			case F2:
			case F3:
			case F4:
			case F5:
			case F6:
			case F7:
			case F8:
			case F9:
			case F10:
			case F11:
			case F12:
				/* Alt + F1~F12 */
				if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
					select_console(raw_code - F1);
				}
				break;
		        default:
		                break;
                }
        }

}



PRIVATE void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key & 0xFF;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


PUBLIC void set_disp_pos_cursor()
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, ((disp_pos/2)>>8)&0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, (disp_pos/2)&0xFF);
	enable_int();
}


PUBLIC void tty_write(TTY* p_tty, char* buf, int len)
{
        char* p = buf;
        int i = len;

        while (i) {
                out_char(p_tty->p_console, *p++);
                i--;
        }
}


PUBLIC int sys_write(char* buf, int len, PROCESS* p_proc)
{
        tty_write(&tty_table[p_proc->nr_tty], buf, len);
        return 0;
}


/************************************************************************
 * char* s  检测 s[0] is MAG_CH key                           
 *
 ***********************************************************************/
PUBLIC int sys_printx(int _unused1, int _unused2, char* s, PROCESS* p_proc)
{
	const char * p;
	char ch;

	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;

	/**
	 * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
	 * If this happens in Ring 0, no linear-physical address mapping
	 * is needed.
	 *
	 * @attention The value of `k_reenter' is tricky here. When
	 *   -# printx() is called in Ring 0
	 *      - k_reenter > 0. When code in Ring 0 calls printx(),
	 *        an `interrupt re-enter' will occur (printx() generates
	 *        a software interrupt). Thus `k_reenter' will be increased
	 *        by `kernel.asm::save' and be greater than 0.
	 *   -# printx() is called in Ring 1~3
	 *      - k_reenter == 0.
	 */
	if (k_reenter == 0)  /* printx() called in Ring<1~3> */
		p = va2la(proc2pid(p_proc), s);
	else if (k_reenter > 0) /* printx() called in Ring<0> */
		p = s;
	else	/* this should NOT happen */
		p = reenter_err;

	/**
	 * @note if assertion fails in any TASK, the system will be halted;
	 * if it fails in a USER PROC, it'll return like any normal syscall
	 * does.
	 */
	if ((*p == MAG_CH_PANIC) ||  // if panic and assert
	    (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_TASKS])) {  // NR_TASKS 系统进程
		disable_int();
		char * v = (char*)V_MEM_BASE;
		const char * q = p + 1; /* +1: skip the magic char */

		while (v < (char*)(V_MEM_BASE + V_MEM_SIZE)) {  //全屏
			*v++ = *q++;
			*v++ = RED_CHAR;
			if (!*q) {  // 结束
				while (((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {  //至行末，空格填补
					/* *v++ = ' '; */
					v++;
					*v++ = GRAY_CHAR;
				}   
				q = p + 1;  //重新开始
			}
		}

		__asm__ __volatile__("hlt");
	}

	while ((ch = *p++) != 0) {
		if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
			continue; /* skip the magic char */

		out_char(tty_table[p_proc->nr_tty].p_console, ch);
	}

	return 0;
}



