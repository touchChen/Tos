#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "elf.h"
#include "proto.h"




/*****************************************************************************
 * <Ring 0~1> The boot parameters have been saved by LOADER.
 *            We just read them out.
 *
 * @param pbp  Ptr to the boot params structure
 *****************************************************************************/
PUBLIC void get_boot_params(struct boot_params * pbp)
{
	/**
	 * Boot params should have been saved at BOOT_PARAM_ADDR.
	 */
	int * p = (int*)BOOT_PARAM_ADDR;
	assert(p[BI_MAG] == BOOT_PARAM_MAGIC);

	pbp->mem_size = p[BI_MEM_SIZE];
	pbp->kernel_file = (unsigned char *)(p[BI_KERNEL_FILE]);

	
	/**
	 * the kernel file should be a ELF executable,
	 * check it's magic number
	 */
	int elf_mag = ELFMAG;
	assert(memcmp(pbp->kernel_file, (void *)&elf_mag, SELFMAG) == 0);

}


/*****************************************************************************
 * <Ring 0~1> Parse the kernel file, get the memory range of the kernel image.
 *
 * - The meaning of `base':		base => first_valid_byte
 * - The meaning of `limit':	base + limit => last_valid_byte
 *
 * @param b   Memory base of kernel.
 * @param l   Memory limit of kernel.
 *****************************************************************************/
PUBLIC int get_kernel_map(unsigned int * b, unsigned int * l)
{
	struct boot_params bp;
	get_boot_params(&bp);

	Elf32_Ehdr* elf_header = (Elf32_Ehdr*)(bp.kernel_file);

	/* the kernel file should be in ELF format */
	
	int elf_mag = ELFMAG;
	if (memcmp(elf_header->e_ident, (void *)&elf_mag, SELFMAG) != 0)
		return -1;
	

	*b = ~0;
	unsigned int t = 0;
	int i;
	for (i = 0; i < elf_header->e_shnum; i++) {
		Elf32_Shdr* section_header =
			(Elf32_Shdr*)(bp.kernel_file +
				      elf_header->e_shoff +
				      i * elf_header->e_shentsize);

		if (section_header->sh_flags & SHF_ALLOC) {
			int bottom = section_header->sh_addr;
			int top = section_header->sh_addr +
				section_header->sh_size;

			if (*b > bottom)
				*b = bottom;
			if (t < top)
				t = top;
		}
	}
	assert(*b < t);
	*l = t - *b - 1;

	//printl("base:0x%x, top:0x%x, limit:0x%x\n", *b,t,*l);

	return 0;
}

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


/************************************************
 * 字符串转数值
 * parm  str 生成的字符串
 * return 转换的数值
 ***********************************************/
PUBLIC int atoi(char * str)
{
	int i;
    int str_len = strlen(str);
    int val = 0;
	
    for(i = 0; i < str_len; i ++)
    {
		char c = str[i];
		if(c == '\0')
	    	break;
    	if(c >= '0' && c <= '9')
		{
	    	val = val * 10 + (int)(c - '0');
		}
	    
	}

	return val;
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
