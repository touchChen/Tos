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
PUBLIC void     delay(int time);
PUBLIC int      kernel_main();
PUBLIC void     restart();
PUBLIC u32      seg2phys(u16 seg);
PUBLIC char*    strcpy(char* p_dst, char* p_src);

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
PUBLIC void     schedule();
PUBLIC void     clear_disp();
PUBLIC void     clear_last_row(int row);
PUBLIC void     init_clock();
PUBLIC void     init_keyboard();
PUBLIC void     enable_int();
PUBLIC void     disable_int();
PUBLIC void     keyboard_read();
PUBLIC void     task_tty();
