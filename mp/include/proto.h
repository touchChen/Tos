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

PUBLIC void     TestA();
PUBLIC void     spurious_irq(int irq);
