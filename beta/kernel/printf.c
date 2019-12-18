#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"



PRIVATE int vsprintf(char *buf, const char *fmt, va_list args);



PUBLIC int printf(const char *fmt, ...)
{
        
	int i;
	char buf[256];
       
	va_list arg = (va_list)((char*)(&fmt) + 4); /*4是参数fmt所占堆栈中的大小*/
	
        i = vsprintf(buf, fmt, arg);
	
        buf[i] = 0;
	printx(buf);     // 系统调用，只有一个参数

	return i;
}


/*
 *  测试
 */
PRIVATE int vsprintf_bak(char *buf, const char *fmt, va_list args)
{
	char*	p;
	char	tmp[256];
	va_list	p_next_arg = args;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt) {
			case 'x':
				itoa(tmp, *((int*)p_next_arg));
				strcpy(p, tmp);
				p_next_arg += 4;
				p += strlen(tmp);
				break;
			case 's':
				break;
			default:
				break;
		}
	}

	return (p - buf);
}



/*
 * 递归
 * char ** ps 指向数组的元素
 */
PRIVATE char* i2a(int val, int base, char ** ps)  // 指针的指针，影响到最原先的字符数组(inner_buf)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		i2a(q, base, ps);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');  // *ps--> 数组下标   **ps--> 数组元素

	return *ps;
}



/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;

	va_list	p_next_arg = args;
	int	m;

	char	inner_buf[STR_DEFAULT_LEN];
	char	cs;
	int	align_nr;

	for (p=buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;   // 补打印的长度 下述代码 cs
		}

		fmt++;

		if (*fmt == '%') {      // 出现'%%'，即不转化，只显示 '%'
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}

		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}

		char * q = inner_buf;
		memset(q, 0, sizeof(inner_buf));

		switch (*fmt) {
			case 'c':
				*q++ = *((char*)p_next_arg);
				p_next_arg += 4;
				break;
			case 'x':
				m = *((int*)p_next_arg);
				i2a(m, 16, &q);
				p_next_arg += 4;
				break;
			case 'd':
				m = *((int*)p_next_arg);
				if (m < 0) {
					m = m * (-1);
					*q++ = '-';
				}
				i2a(m, 10, &q);
				p_next_arg += 4;
				break;
			case 's':
				strcpy(q, (*((char**)p_next_arg))); // 字符数组
				q += strlen(*((char**)p_next_arg));
				p_next_arg += 4;
				break;
			default:
				break;
		}

		int k;
		for (k = 0; k < ((align_nr > strlen(inner_buf)) ? (align_nr - strlen(inner_buf)) : 0); k++) {
			*p++ = cs;
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}

	*p = 0;

	return (p - buf);
}



PUBLIC int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}


PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

