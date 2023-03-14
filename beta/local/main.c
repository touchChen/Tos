#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                               Test 用户进程
 *======================================================================*/
void TestA()
{       
	//char ss[] = "hello world";
	while(1){
	    //assert(0);
	    // panic("error:%20s in TestA",ss);
        //printf("A.");
        printf("ticks:%d.",get_u_ticks());
		milli_delay(10000);      
	}
}


void TestB()
{
	char tty_name[] = "/dev_tty2";

	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];

	//syslog("hello world\n");
	while (1) {
		write(fd_stdout, "$ ", 2);
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;

		if (strcmp(rdbuf, "hello") == 0) {
			write(fd_stdout, "this is a user proc\n", 20);
		}
		else {
			if (rdbuf[0]) {
				write(fd_stdout, "{", 1);
				write(fd_stdout, rdbuf, r);
				write(fd_stdout, "}\n", 2);
			}
		}
	}

	assert(0); /* never arrive here */
}



void TestC()
{
	while(1){
        printf("C.\n");
        milli_delay(10000);
		//printf("disp_pos:%x.\n",disp_pos);
		
	}
}



void test_fs()
{ 
	clearlog();

	int fd = open("/tc", O_CREAT);
	printf("fd:%d\n",fd);    
	close(fd);
	

	fd = open("/tc", O_RDWR);
	int fd2 = open("/tc", O_RDWR);
	printf("fd1:%d, fd2:%d\n",fd,fd2);
	char bufw[] = "hello world, this is a test of reading and writing file!";
	int wlen = write(fd, bufw, strlen(bufw));
	close(fd);

	fd = open("/tc", O_RDWR);
	char bufr[wlen];
	int rlen = read(fd, bufr, wlen);
	bufr[rlen] = 0;       
	close(fd);

	printf("Read len: %d, buf: %s\n",rlen,bufr);
    

	fd = open("/tt", O_CREAT);
	close(fd);

	
	int ul = unlink("/tt");
	if(ul == 0)
	{
		printf(" unlink success!\n");
    }

	fd = open("new_file", O_CREAT);
	close(fd);

	fd = open("/chen", O_RDWR);
	printf("fd:%d\n",fd);    
	close(fd);

/*
	char buf[512*32];
	buf[0] = '\0';
	int log_pos = readlog(buf); 
	printf("log pos: %d, log len %d\nlog:", log_pos, strlen(buf));
*/
    
	//graphlog();

    spin("Test FS...");
}


