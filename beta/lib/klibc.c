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


/************************************************
 *
 * 数值转字符串
 * str 生成的字符串
 * num 需要转换的数值
 ***********************************************/
PUBLIC char* itoa(char * str, int num) 
{
	char *p = str;
	char ch;
	int i;
	int flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
	    *p++ = '0';
	}
	else{	
	    for(i=28;i>=0;i-=4){
	        ch = (num >> i) & 0xF;
		if(flag || (ch > 0)){
		    flag = TRUE;
		    ch += '0';
		    if(ch > '9'){
			ch += 7;
		    }

		    *p++ = ch;
		}
           }
	}

	*p = 0;

	return str;
}



PUBLIC void disp_int_c(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}


PUBLIC void clear_disp()
{
        int i;
        disp_pos = 0;
	for (i = 0; i < 80*25; i++) {
		disp_str(" ");
	}
	disp_pos = 0;

}

PUBLIC void clear_last_row(int row)
{
      int r = 25 - row;
      int i;
      
      disp_pos = r*80*2;
      for (i=r*80; i < 80*25; i++) {
		disp_str(" ");
      }
      disp_pos = r*80*2;
}


PUBLIC void delay(int time)
{
	int i, j, k;
	for (k = 0; k < time; k++) {
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 10000; j++) {}
		}
	}
}
