#ifndef	_TOS_TYPE_H_
#define	_TOS_TYPE_H_

typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;

 /* typedef  返回类型(*新类型)(参数表)*/
typedef	void (*int_handler)();   /* typedef  定义了新类型*/
typedef	void (*task_f)();
typedef void (*irq_handler) (int irq);

typedef void* system_call;    /*太通用了*/


#endif /* _TOS_TYPE_H_ */
