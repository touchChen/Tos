#ifndef	_TOS_STDIO_H_
#define	_TOS_STDIO_H_


#define	PUBLIC		/* PUBLIC is the opposite of PRIVATE */
#define	PRIVATE	static	/* PRIVATE x limits the scope of x */

typedef	unsigned long long	u64;
typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;

typedef	char *			va_list;

/* the assert macro */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp)  if (exp) ; \
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

/* EXTERN */
#define	EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* string */
#define	STR_DEFAULT_LEN	1024

#define	O_CREAT		1
#define	O_RDWR		2

#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

#define	MAX_PATH	128

/**
 * @struct stat
 * @brief  File status, returned by syscall stat();
 */
struct stat {
	int st_dev;		/* major/minor device number */
	int st_ino;		/* i-node number */
	int st_mode;		/* file mode, protection bits, etc. */
	int st_rdev;		/* device ID (if special file) */
	int st_size;		/* file size */
};

/**
 * @struct time
 * @brief  RTC time from CMOS.
 */
struct time {
	u32 year;
	u32 month;
	u32 day;
	u32 hour;
	u32 minute;
	u32 second;
};

#define  BCD_TO_DEC(x)      ( (x >> 4) * 10 + (x & 0x0f) )



/* printf.c */
PUBLIC  int     printf(const char *fmt, ...);
PUBLIC  int     printl(const char *fmt, ...);

PUBLIC  int     vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC	int		sprintf(char *buf, const char *fmt, ...);



#ifdef ENABLE_DISK_LOG
#define SYSLOG syslog
#endif

/* lib/fslib.c */
PUBLIC	int	open(const char *pathname, int flags);

PUBLIC	int	close(int fd);

PUBLIC	int	read(int fd, void *buf, int count);

PUBLIC	int	write(int fd, const void *buf, int count);

PUBLIC	int	unlink(const char *pathname);



/* lib/proclib.c */
PUBLIC 	int	getpid();



/* lib/mmlib.c */
PUBLIC	int	fork();

PUBLIC	void exit(int status);

PUBLIC	int	wait(int * status);



/* lib/syslog.c */
PUBLIC	int	syslog		(const char *fmt, ...);


#endif /* _TOS_STDIO_H_ */