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


PRIVATE void test_static_int()
{
    static  int  bss_i = 0;
    static  int  data_i = 1;
    printf("&bss_i=0x%xh, bss_i=%d,   &data_i=0x%x, data_i=%d\n", &bss_i, ++bss_i, &data_i, ++data_i);
}


void test_fs()
{ 
	int fd = open("/tc", O_CREAT);
	close(fd);
    test_static_int();
	test_static_int();

	fd = open("/tc", O_RDWR);
	char bufw[] = "hello world, this is a test of reading and writing file!";
	int wlen = write(fd, bufw, strlen(bufw));
	close(fd);

	fd = open("/tc", O_RDWR);
	char bufr[wlen];
	int rlen = read(fd, bufr, wlen);
	bufr[rlen] = 0;       
	close(fd);

	printf("Read len: %d, buf: %s\n",rlen,bufr);

	char buf[512*32];
	int log_pos = readlog(buf); 
	printf("log pos: %d, log len %d\nlog:", log_pos, strlen(buf));
    
    /*
	printf("==============================\n");
	char _buf[256];
    do{
        int l = min(255,strlen(buf));
        memcpy((void*)_buf, (void*)buf, l);
		if(strlen(buf)>255)
		{  
			_buf[l] = '\0';
            memcpy((void*)buf, (void*)(&buf[255]), (strlen(buf)-255));			
		}else{
			buf[0] = '\0';
		}

		//printf(_buf);
    }while(strlen(buf)>0);
	printf("==============================\n");
    */

	int nfd = open("/tt", O_CREAT);
	close(nfd);
	
	int ul = unlink("/tt");
	if(ul == 0)
	{
		printf(" unlink success!\n");
    }

    spin("Test FS...");
}


