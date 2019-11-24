PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC void	init_prot();
PUBLIC void	init_8259A();
PUBLIC void*	memcpy(void* pDst, void* pSrc, int iSize);
PUBLIC void	memset(void* p_dst, char ch, int size);
PUBLIC void     init_prot();
PUBLIC void     disp_int(int input);
PUBLIC void     disp_int_c(int input);
PUBLIC char*    itoa(char * str, int num);
PUBLIC void     delay(int time);
PUBLIC int      kernel_main();
PUBLIC void     restart();
PUBLIC u32      seg2phys(u16 seg);
PUBLIC char*    strcpy(char* p_dst, char* p_src);
PUBLIC int      strlen(char* p_str);

PUBLIC void     TestA();
PUBLIC void     TestB();
PUBLIC void     TestC();

PUBLIC void     clock_handler(int irq);
PUBLIC void     put_irq_handler(int irq, irq_handler handler);
PUBLIC void     spurious_irq(int irq);

PUBLIC int      disable_irq(int irq);
PUBLIC void     enable_irq(int irq);
PUBLIC int      get_ticks();
PUBLIC void     sys_call(); 
PUBLIC int      sys_get_ticks();
PUBLIC void     milli_delay(int milli_sec);
PUBLIC void     schedule();
PUBLIC void     clear_disp();
PUBLIC void     clear_last_row(int row);
PUBLIC void     init_clock();
PUBLIC void     init_keyboard();
PUBLIC void     enable_int();
PUBLIC void     disable_int();
PUBLIC void     keyboard_read(TTY* p_tty);
PUBLIC void     task_tty();
PUBLIC void     in_process(TTY* p_tty, u32 key);
PUBLIC void     set_disp_pos_cursor();

PUBLIC int      is_current_console(CONSOLE* p_con);
PUBLIC void     out_char(CONSOLE* p_con, char ch);
PUBLIC void     init_screen(TTY* p_tty);
PUBLIC void     set_video_start_addr(u32 addr);
PUBLIC void     select_console(int nr_console);
PUBLIC void     scroll_screen(CONSOLE* p_con, int direction);
PUBLIC void     flush(CONSOLE* p_con);


PUBLIC int      printf(const char *fmt, ...);
#define	printl	printf
PUBLIC int      sys_write(char* buf, int len, PROCESS* p_proc);
PUBLIC void     write(char* buf, int len);


PUBLIC void*    va2la(int pid, void* va);
PUBLIC void     panic(const char *fmt, ...);

PUBLIC int      sendrec(int function, int src_dest, MESSAGE* msg);
PUBLIC void     printx(char* s);

PUBLIC int      sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* p);
PUBLIC int      sprintf(char *buf, const char *fmt, ...);



/**
 * `phys_copy' and `phys_set' are used only in the kernel, where segments
 * are all flat (based on 0). In the meanwhile, currently linear address
 * space is mapped to the identical physical address space. Therefore,
 * a `physical copy' will be as same as a common copy, so does `phys_set'.
 */
#define	phys_copy	memcpy
#define	phys_set	memset

