#ifndef _TOS_TTY_H_
#define _TOS_TTY_H_


#define TTY_IN_BYTES	     256	/* tty input queue size */
#define TTY_OUT_BUF_LEN		 2

struct s_console;


/* TTY */
typedef struct s_tty
{
	u32	ibuf[TTY_IN_BYTES];	 /* TTY 输入缓冲区 */
	u32*	ibuf_head;		 /* 指向缓冲区中下一个空闲位置 */
	u32*	ibuf_tail;		 /* 指向键盘任务应处理的键值 */
	int	ibuf_cnt;		 /* 缓冲区中已经填充了多少 */

	int	tty_caller;
	int	tty_procnr;
	void*	tty_req_buf;
	int	tty_left_cnt;
	int	tty_trans_cnt;

	struct s_console *	console;
}TTY;


#endif 

/* _TOS_TTY_H_ */
