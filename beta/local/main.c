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
	while(1){
		//disp_str("B");
		//disp_int_c(get_ticks());
                //printf("ticks:%x.\n",get_ticks());
                printf("B.c");
                milli_delay(10000);
	}
}



void TestC()
{
	while(1){
                printf("C.\n");
                milli_delay(10000);
		//printf("disp_pos:%x.\n",disp_pos);
		
	}
}


PRIVATE void cc()
{
    static  int  ii = 0;
    static  int  c = 1;
    printf("&ii=0x%xh, ii=%d,   &c=0x%x, c=%d\n", &ii, ++ii, &c, ++c);
}


void TestFs()
{ 
	int fd = open("/tc", O_CREAT);
	close(fd);
        cc();cc();

	fd = open("/tc", O_RDWR);
	char bufw[] = "hello world, this is a test of reading and writing file!";
	int wlen = write(fd, bufw, strlen(bufw));
	close(fd);

	fd = open("/tc", O_RDWR);
        char bufr[wlen];
        int rlen = read(fd, bufr, wlen);
	bufr[rlen] = 0;       
        close(fd);

	printl("Read len: %d, buf: %s\n",rlen,bufr);
   
        spin("Test FS...");
}


