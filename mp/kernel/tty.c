#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);


/*======================================================================*
                           task_tty
 *======================================================================*/
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
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}

        /*
	while (1) {
		keyboard_read();
	}
       */
}


/*======================================================================*
			   init_tty
 *======================================================================*/
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



/*======================================================================*
			      tty_do_read
 *======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}


/*======================================================================*
			      tty_do_write
 *======================================================================*/
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




/*======================================================================*
				in_process
 *======================================================================*/
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


/*======================================================================*
			      put_key
*======================================================================*/
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

/*======================================================================*
			    set_cursor
 *======================================================================*/
PUBLIC void set_disp_pos_cursor()
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, ((disp_pos/2)>>8)&0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, (disp_pos/2)&0xFF);
	enable_int();
}

