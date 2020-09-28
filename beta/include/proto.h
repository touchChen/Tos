
/*************************** lib ***************************************/
/****** string.asm ******/
PUBLIC void* memcpy(void* p_dst, void* p_src, int iSize);
PUBLIC void  memset(void* p_dst, char ch, int size);
PUBLIC char* strcpy(char* p_dst, char* p_src);
PUBLIC int strlen(const char* p_str);


/****** klib.asm ******/
PUBLIC void disp_str(char* info);
PUBLIC void disp_color_str(char* info, int color);
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);

PUBLIC void disp_al(u8 input);
PUBLIC void disp_int(int input);
PUBLIC void enable_irq(int irq);  // 开启硬件中断 (irq 中断号)
PUBLIC int disable_irq(int irq);  // 关闭硬件中断
PUBLIC void enable_int();  
PUBLIC void disable_int();
PUBLIC void port_read(u16 port, void* buf, int n);
PUBLIC void port_write(u16 port, void* buf, int n);


/****** klibc.c ******/
PUBLIC void get_boot_params(struct boot_params * pbp);
PUBLIC int get_kernel_map(unsigned int * b, unsigned int * l);
PUBLIC char* itoa(char * str, int num);
PUBLIC int atoi(char * str);
PUBLIC void disp_int_c(int input);
PUBLIC void delay(int time);
PUBLIC void clear_disp();  //清屏
PUBLIC void clear_last_row(int row);


/****** mics.c ******/
PUBLIC int memcmp(const void * s1, const void *s2, int n);
PUBLIC int strcmp(const char * s1, const char *s2);
PUBLIC char * strcat(char * s1, const char *s2); 
PUBLIC void spin(char *func_name);
PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line);


/****** syscall.asm ******/  //系统中断
PUBLIC void printx(char* s);
PUBLIC int sendrec(int function, int src_dest, MESSAGE* msg);
PUBLIC int get_ticks();
//PUBLIC void write(char* buf, int len);


/****** printf.c ******/
PUBLIC int printf(const char *fmt, ...);
#define	printl	printf
PUBLIC int sprintf(char *buf, const char *fmt, ...);
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC void panic(const char *fmt, ...);


/****** fslib.c ******/
PUBLIC int open(const char *pathname, int flags);
PUBLIC int close(int fd);
PUBLIC int read(int fd, void *buf, int count);
PUBLIC int write(int fd, const void *buf, int count);
PUBLIC int unlink(const char * pathname);


/****** proclib.c ******/
PUBLIC int getpid();
PUBLIC int get_u_ticks();


/****** syslog.c ******/
PUBLIC int syslog(const char *fmt, ...);
PUBLIC int readlog(void *buf);
PUBLIC void graphlog();
PUBLIC void clearlog();



/****** mmlib.c ******/
PUBLIC int fork();
PUBLIC void exit(int status);
PUBLIC int wait(int * status);








/*************************** kernel ***************************************/
/****** i8259.c ******/ //硬件中断，从保护模式进入
PUBLIC void init_8259A();
PUBLIC void put_irq_handler(int irq, irq_handler handler);


/****** kernel.asm ******/ // _start 内核进口 ,调用 cstart, 跳转到 kernel_main
PUBLIC void restart();  // 进程入口
PUBLIC void sys_call(); // 系统中断


/****** start.c ******/
PUBLIC void cstart();  // kernel.asm 跳转 到此函数，c语言开端


/****** protect.c ******/ //保护模式
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);
PUBLIC void spurious_irq(int irq);
PUBLIC void init_descriptor(DESCRIPTOR * p_desc, u32 base, u32 limit, u16 attribute);
PUBLIC void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags);


/****** clock.c  ******/  //时钟中断
PUBLIC void init_clock();
PUBLIC void clock_handler(int irq);  // --> schedule
PUBLIC void milli_delay(int milli_sec);


/****** main.c ******/
PUBLIC int kernel_main();  //内核主程序
PUBLIC void Init();


/****** proc.c ******/   //进程
PUBLIC void schedule();
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* p);
PUBLIC int sys_get_ticks();

PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg); // 系统调用sendrec的替代

PUBLIC int ldt_seg_linear(PROCESS* p, int idx); // ldt段的段基址
PUBLIC void* va2la(int pid, void* va); // 虚拟地址转线性地址
PUBLIC void reset_msg(MESSAGE* p);
PUBLIC void dump_proc(PROCESS* p);
PUBLIC void dump_msg(const char * title, MESSAGE* m);

PUBLIC void inform_int(int task_nr); // 通知有一条中断消息


/****** keyboard.c ******/
PUBLIC void init_keyboard();  // task_tty 调用该函数 
PUBLIC void keyboard_read(TTY* p_tty);


/****** console.c ******/
PUBLIC void init_screen(TTY* p_tty);
PUBLIC void select_console(int nr_console);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);
PUBLIC int is_current_console(CONSOLE* p_con);
PUBLIC void out_char(CONSOLE* p_con, char ch);
PUBLIC void set_video_start_addr(u32 addr);
PUBLIC void flush(CONSOLE* p_con);


/****** tty.c ******/
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);
PUBLIC void set_disp_pos_cursor();
PUBLIC void tty_write(TTY* p_tty, char* buf, int len);
PUBLIC int sys_write(char* buf, int len, PROCESS* p_proc);
PUBLIC int sys_printx(int _unused1, int _unused2, char* s, PROCESS* p_proc);


/****** systask.c ******/
PUBLIC void task_sys();


/****** hd.c ******/
PUBLIC void task_hd();
PUBLIC void hd_handler(int irq);








/*************************** fs ***************************************/
/****** main.c ******/
PUBLIC void task_fs();
PUBLIC int rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr, void* buf);
PUBLIC struct super_block * get_super_block(int dev);


/****** fslib.c ******/
PUBLIC int strip_path(char * filename, const char * pathname, struct inode** ppinode);


/****** disklog.c ******/
PUBLIC int do_disklog();
PUBLIC int do_readlog();
PUBLIC void do_clearlog();
PUBLIC int disklog(char * logstr);


/****** graph.c ******/
PUBLIC void dump_fd_graph(const char * fmt, ...);




/*************************** mm ***************************************/
/****** main.c ******/
PUBLIC void task_mm();
PUBLIC int alloc_mem(int pid, int memsize);
PUBLIC int free_mem(int pid);


/****** mmlib.c ******/
PUBLIC int do_fork();
PUBLIC void do_exit(int status);
PUBLIC void do_wait();

 






/*************************** local ***************************************/
/****** main.c ******/
PUBLIC void TestA();
PUBLIC void TestB();
PUBLIC void TestC();
PUBLIC void test_fs();




/*************************** 测试 ***************************************/
PUBLIC int t_test();
PUBLIC int  sys_test();



/**
 * `phys_copy' and `phys_set' are used only in the kernel, where segments
 * are all flat (based on 0). In the meanwhile, currently linear address
 * space is mapped to the identical physical address space. Therefore,
 * a `physical copy' will be as same as a common copy, so does `phys_set'.
 */
#define	phys_copy	memcpy
#define	phys_set	memset

/* max() & min() */
#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))







