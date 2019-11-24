#ifndef	_TOS_TYPE_H_
#define	_TOS_TYPE_H_

typedef	unsigned long long	u64;
typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;

typedef	char *			va_list;


 /* typedef  返回类型(*新类型)(参数表)*/
typedef	void (*int_handler)();   /* typedef  定义了新类型*/
typedef	void (*task_f)();
typedef void (*irq_handler) (int irq);

typedef void* system_call;    /*太通用了*/


/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;



#endif /* _TOS_TYPE_H_ */
