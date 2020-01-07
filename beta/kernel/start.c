#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "proto.h"
#include "global.h"


PUBLIC void cstart()
{
	 disp_pos = 15*2*80;
	/* 将 LOADER 中的 GDT 复制到新的 GDT 中 */
	memcpy(gdt,				   /* New GDT */
	       (void*)(*((u32*)(&gdt_ptr[2]))),    /* Base  of Old GDT 后4位是段基地址 */
	       *((u16*)(&gdt_ptr[0])) + 1	   /* Limit of Old GDT 前2位是Gdt的长度限制 */
		); 
        
        
	/* gdt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sgdt/lgdt 的参数。*/
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);  
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base  = (u32)&gdt;


        /* idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sidt/lidt 的参数。*/
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base  = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
	*p_idt_base  = (u32)&idt;

         
        init_prot();


}
