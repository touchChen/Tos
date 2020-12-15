; ==========================================
; 编译方法：nasm run.asm -o *.bin run.com
; ==========================================

%include	"pm.inc"	; 常量, 宏, 以及一些说明

org	0100h
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
;                              段基址,        段界限     ,       属性
LABEL_GDT:	   Descriptor       	0,                  0,       0           		; 空描述符
LABEL_DESC_NORMAL: Descriptor       0,             0ffffh,       DA_DRW      		; Normal 描述符
LABEL_DESC_DATA:   Descriptor       0,          DataLen-1,       DA_DRW + DA_DPL2   ; Data
LABEL_DESC_CODE32: Descriptor       0,   SegCode32Len - 1,       DA_C + DA_32		; 非一致代码段
LABEL_DESC_CODE16: Descriptor       0,             0ffffh,       DA_C        		; 非一致代码段, 16
LABEL_DESC_VIDEO:  Descriptor 0B8000h,             0ffffh,       DA_DRW	+ DA_DPL3	; 显存首地址
LABEL_DESC_STACK32:  Descriptor     0,         TopOfStack,       DA_DRWA + DA_32	; Stack, 32位
LABEL_DESC_LDT:    Descriptor       0,         LDTLen - 1,       DA_LDT				; LDT
LABEL_DESC_CODE_DEST: Descriptor    0,   SegCodeDestLen-1,       DA_C + DA_32		; 非一致代码段,32

LABEL_DESC_CODE_RING3: Descriptor   0,  SegCodeRing3Len-1,       DA_C + DA_32 + DA_DPL3
LABEL_DESC_STACK3:     Descriptor   0,        TopOfStack3,       DA_DRWA + DA_32 + DA_DPL3
LABEL_DESC_TSS:        Descriptor   0,           TSSLen-1,       DA_386TSS	   		;TSS
; 门									目标选择子,		偏移,		DCount,		属性
LABEL_CALL_GATE:	Gate	SelectorCodeDest,		0,			0,			DA_386CGate + DA_DPL3
; GDT 结束

GdtLen		equ		$ - LABEL_GDT	; GDT长度
GdtPtr		dw		GdtLen - 1		; GDT界限
			dd		0				; GDT基地址

; GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT + SA_RPL1
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT 
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorStack32		equ	LABEL_DESC_STACK32	- LABEL_GDT
SelectorLDT			equ	LABEL_DESC_LDT		- LABEL_GDT
SelectorCodeDest	equ	LABEL_DESC_CODE_DEST	- LABEL_GDT
SelectorCodeRing3	equ	LABEL_DESC_CODE_RING3	- LABEL_GDT + SA_RPL3
SelectorCallGate	equ	LABEL_CALL_GATE 	- LABEL_GDT + SA_RPL3
SelectorStack3		equ	LABEL_DESC_STACK3	- LABEL_GDT + SA_RPL3
SelectorTSS			equ	LABEL_DESC_TSS		- LABEL_GDT
; END of [SECTION .gdt]


[SECTION .data]	 ; 数据段
ALIGN	32
[BITS	32]
LABEL_DATA:
SPValueInRealMode	dw	0
; 字符串
PMMessage:			db		"Hello world, This is in Protect Mode now. ^-^", 0	; 在保护模式中显示
OffsetPMMessage		equ		PMMessage - $$
DataLen				equ		$ - LABEL_DATA
; END of [SECTION .data]

; 全局堆栈段
[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
	times 512 db 0

TopOfStack	equ	$ - LABEL_STACK - 1
; END of [SECTION .gs]


; 堆栈段ring3
[SECTION .s3]
ALIGN	32
[BITS	32]
LABEL_STACK3:
	times 512 db 0

TopOfStack3	equ	$ - LABEL_STACK3 - 1
; END of [SECTION .s3]


; TSS ---------------------------------------------------------------------------------------------
[SECTION .tss]
ALIGN	32
[BITS	32]
LABEL_TSS:
		DD	0			; Back
		DD	TopOfStack			; 0 级堆栈
		DD	SelectorStack32		; 0 级堆栈描述符
		DD	0			; 1 级堆栈
		DD	0			; 
		DD	0			; 2 级堆栈
		DD	0			; 
		DD	0			; CR3
		DD	0			; EIP
		DD	0			; EFLAGS
		DD	0			; EAX
		DD	0			; ECX
		DD	0			; EDX
		DD	0			; EBX
		DD	0			; ESP
		DD	0			; EBP
		DD	0			; ESI
		DD	0			; EDI
		DD	0			; ES
		DD	0			; CS
		DD	0			; SS
		DD	0			; DS
		DD	0			; FS
		DD	0			; GS
		DD	0			; LDT
		DW	0			; 调试陷阱标志
		DW	$ - LABEL_TSS + 2	; I/O位图基址
		DB	0ffh				; I/O位图结束标志
TSSLen		equ	$ - LABEL_TSS
; TSS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov		ss, ax
	mov		sp, 0100h

	mov		[LABEL_GO_BACK_TO_REAL+3], ax
	mov		[SPValueInRealMode], sp
 
	call    DispStr
	mov     di, (80 * 3 + 0) * 2
	push    cs
	call    DispInt
         


	; 初始化 16 位代码段描述符
	mov		ax, cs
	movzx	eax, ax
	shl		eax, 4
	add		eax, LABEL_SEG_CODE16
	mov		word [LABEL_DESC_CODE16 + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_CODE16 + 4], al
	mov		byte [LABEL_DESC_CODE16 + 7], ah

	; 初始化 32 位代码段描述符
	xor		eax, eax
	mov		ax, cs
	shl		eax, 4
	add		eax, LABEL_SEG_CODE32
	mov		word [LABEL_DESC_CODE32 + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_CODE32 + 4], al
	mov		byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化数据段描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_DATA
	mov		word [LABEL_DESC_DATA + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_DATA + 4], al
	mov		byte [LABEL_DESC_DATA + 7], ah

	; 初始化堆栈段描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_STACK
	mov		word [LABEL_DESC_STACK32 + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_STACK32 + 4], al
	mov		byte [LABEL_DESC_STACK32 + 7], ah
        
	; 初始化堆栈段描述符(STACK3)
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_STACK3
	mov		word [LABEL_DESC_STACK3 + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_STACK3 + 4], al
	mov		byte [LABEL_DESC_STACK3 + 7], ah

	; 初始化 LDT 在 GDT 中的描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_LDT
	mov		word [LABEL_DESC_LDT + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_LDT + 4], al
	mov		byte [LABEL_DESC_LDT + 7], ah

	; 初始化 LDT 中的描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_CODE_A
	mov		word [LABEL_LDT_DESC_CODEA + 2], ax
	shr		eax, 16
	mov		byte [LABEL_LDT_DESC_CODEA + 4], al
	mov		byte [LABEL_LDT_DESC_CODEA + 7], ah

	; 初始化 返回16位代码段 的描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_CODE_RETURN
	mov		word [LABEL_LDT_DESC_CODE_RETURN + 2], ax
	shr		eax, 16
	mov		byte [LABEL_LDT_DESC_CODE_RETURN + 4], al
	mov		byte [LABEL_LDT_DESC_CODE_RETURN + 7], ah

	; 初始化测试调用门的代码段描述符
	xor		eax, eax
	mov		ax, cs
	shl		eax, 4
	add		eax, LABEL_SEG_CODE_DEST
	mov		word [LABEL_DESC_CODE_DEST + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_CODE_DEST + 4], al
	mov		byte [LABEL_DESC_CODE_DEST + 7], ah

	; 初始化Ring3描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_CODE_RING3
	mov		word [LABEL_DESC_CODE_RING3 + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_CODE_RING3 + 4], al
	mov		byte [LABEL_DESC_CODE_RING3 + 7], ah

	; 初始化 TSS 描述符
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_TSS
	mov		word [LABEL_DESC_TSS + 2], ax
	shr		eax, 16
	mov		byte [LABEL_DESC_TSS + 4], al
	mov		byte [LABEL_DESC_TSS + 7], ah


	; 为加载 GDTR 作准备
	xor		eax, eax
	mov		ax, ds
	shl		eax, 4
	add		eax, LABEL_GDT				; eax <- gdt 基地址
	mov		dword [GdtPtr + 2], eax		; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli

	; 打开地址线A20
	in		al, 92h
	or		al, 00000010b
	out		92h, al

	; 准备切换到保护模式
	mov		eax, cr0
	or		eax, 1
	mov		cr0, eax

	; 真正进入保护模式
	jmp		dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs,
									; 并跳转到 Code32Selector:0  处

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DispStr:
	mov		ax, BootMessage
	mov		bp, ax				; ES:BP = 串地址
	mov		cx, lenMessage		; CX = 串长度
	mov		ax, 01301h			; AH = 13,  AL = 01h
	mov		bx, 000ch			; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	mov		dl, 0
	int		10h					; 10h 号中断
	ret

BootMessage:		db		"Hello, this in real model!"
lenMessage:			equ		$ - BootMessage


DispALReal:
	push	cx
	push	dx

	mov 	dx, DISPLAYSEG
	mov 	es, dx

	mov		ah, 0Ch			; 0000: 黑底    1100: 红字
	mov		dl, al
	shr		al, 4
	mov		cx, 2
.begin:
	and		al, 01111b
	cmp		al, 9
	ja		.1
	add		al, '0'
	jmp		.2
.1:
	sub		al, 0Ah
	add		al, 'A'
.2:
	mov		[es:di], ax

	add		di, 2

	mov		al, dl
	loop	.begin

	pop		dx
	pop		cx

	ret

DISPLAYSEG	equ 0xb800



DispInt:
	mov		bx,  sp      ; 当前sp指向压入的ip(返回后的下条指令)
	add		bx,  2

	mov		ax, [ss:bx]       
	shr		ax, 8
	call	DispALReal

	mov     ax, [ss:bx]
	call	DispALReal

	ret     2  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LABEL_REAL_ENTRY:		; 从保护模式跳回到实模式就到了这里
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov		ss, ax

	mov		sp, [SPValueInRealMode]

	in		al, 92h			; `.
	and		al, 11111101b	;  | 关闭 A20 地址线
	out		92h, al			; /

	sti					; 开中断

	mov		ax, 4c00h	; `.
	int		21h			; /  回到 DOS

; END of [SECTION .s16]



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;保护模式
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS	32]
LABEL_SEG_CODE32:
	mov		ax, SelectorVideo
	mov		gs, ax			; 视频段选择子(目的)

	mov		ax, SelectorData
	mov		ds, ax			; 数据段选择子

    ; 堆栈
	mov		ax, SelectorStack32
	mov		ss, ax			; 堆栈段选择子
	mov		esp, TopOfStack

	call    DisMessage
	;call    SelectorCodeRing3:0
	
	; Load LDT
	mov		ax, SelectorLDT
	lldt	ax

	call	SelectorLDTCodeA:0	; 跳入局部任务
        
	mov		ax, SelectorTSS
	ltr		ax                  ; 在任务内发生特权级变换时要切换堆栈，而内层堆栈的指针存放在当前任务的TSS中，所以要设置任务状态段寄存器 TR。
	push	SelectorStack3
	push	TopOfStack3
	push	SelectorCodeRing3
	push	0
	retf
   
	jmp		SelectorCode16:0


DisMessage:
	; 下面显示一个字符串
	mov		ah, 0Ch					; 0000: 黑底    1100: 红字
	xor		esi, esi
	xor		edi, edi
	mov		esi, OffsetPMMessage	; 源数据偏移
	mov		edi, (80 * 3 + 0) * 2	; 目的数据偏移。屏幕第 10 行, 第 0 列。
	cld
.1:
	lodsb
	test	al, al
	jz		.2
	mov		[gs:edi], ax
	add		edi, 2
	jmp		.1
.2:	; 显示完毕 到此停止
	ret



DispAL:
	push	ecx
	push	edx

	mov		ah, 0Ch			; 0000: 黑底    1100: 红字
	mov		dl, al
	shr		al, 4
	mov		ecx, 2
.begin:
	and		al, 01111b
	cmp		al, 9
	ja		.1
	add		al, '0'
	jmp		.2
.1:
	sub		al, 0Ah
	add		al, 'A'
.2:
	mov		[gs:edi], ax
	add		edi, 2

	mov		al, dl
	loop	.begin

	pop		edx
	pop		ecx

	ret


SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]




; 16 位代码段. 由 32 位代码段跳入, 跳出后到实模式
[SECTION .s16code]
ALIGN	32
[BITS	16]
LABEL_SEG_CODE16: ; 16位，为了cs段描述符高速缓冲寄存器合理
	; 跳回实模式:
	mov		ax, SelectorNormal  ;清空段描述符高速缓冲寄存器
	mov		ds, ax
	mov		es, ax
	mov		fs, ax
	mov		gs, ax
	mov		ss, ax

	mov		eax, cr0
	and		al, 11111110b
	mov		cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp	0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值

Code16Len	equ	$ - LABEL_SEG_CODE16

; END of [SECTION .s16code]




; LDT
[SECTION .ldt]
ALIGN	32
LABEL_LDT:
;                            段基址       段界限      属性
LABEL_LDT_DESC_CODEA: Descriptor 0, CodeALen - 1, DA_C + DA_32 ; Code, 32 位
LABEL_LDT_DESC_CODE_RETURN: Descriptor 0, CodeReturnLen - 1, DA_C + DA_32 ; Code, 32 位

LDTLen		equ	$ - LABEL_LDT

; LDT 选择子
SelectorLDTCodeA		equ		LABEL_LDT_DESC_CODEA	- LABEL_LDT + SA_TIL
SelectorLDTCodeReturn	equ		LABEL_LDT_DESC_CODE_RETURN	- LABEL_LDT + SA_TIL
; END of [SECTION .ldt]


; CodeA (LDT, 32 位代码段)
[SECTION .la]
ALIGN	32
[BITS	32]
LABEL_CODE_A:
	mov		ax, SelectorVideo
	mov		gs, ax			; 视频段选择子(目的)

	mov		edi, (80 * 4 + 0) * 2	; 屏幕第 10 行, 第 0 列。
	mov		ah, 0Ch			; 0000: 黑底    1100: 红字
	mov		al, 'L'
	mov		[gs:edi], ax

	retf

CodeALen	equ	$ - LABEL_CODE_A
; END of [SECTION .la]




; CodeReturn (LDT, 32 位代码段)
[SECTION .lr]
ALIGN	32
[BITS	32]
LABEL_CODE_RETURN:
	jmp		SelectorCode16:0

CodeReturnLen	equ	$ - LABEL_CODE_RETURN
; END of [SECTION .lr]



[SECTION .sdest]; 调用门目标段
[BITS	32]
LABEL_SEG_CODE_DEST:
	mov		ax, SelectorVideo
	mov		gs, ax			; 视频段选择子(目的)

	mov		edi, (80 * 7 + 0) * 2	; 屏幕第 12 行, 第 0 列。
	mov		ah, 0Ch			; 0000: 黑底    1100: 红字
	mov		al, 'G'
	mov		[gs:edi], ax

	call	SelectorLDTCodeReturn:0	; 跳入局部任务
	;retf
	
SegCodeDestLen	equ	$ - LABEL_SEG_CODE_DEST
; END of [SECTION .sdest]


; CodeRing3
[SECTION .ring3]
ALIGN	32
[BITS	32]
LABEL_CODE_RING3:
	mov		ax, SelectorVideo
	mov		gs, ax

	mov		edi, (80 * 6 + 0) * 2
	mov		ah, 0Ch
	mov		al, '3'
	mov		[gs:edi], ax

	call	SelectorCallGate:0

SegCodeRing3Len	equ	$ - LABEL_CODE_RING3
; END of [SECTION .ring3]



