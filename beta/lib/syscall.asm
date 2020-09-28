%include "sconst.inc"


_NR_write	    equ 1
INT_VECTOR_SYS_CALL equ 0x90

_NR_printx	    equ 0
_NR_sendrec	    equ 1
_NR_get_ticks	equ 2
_NR_test		equ 3 

global	get_ticks ; 导出符号
;global  write

global	printx
global	sendrec
global	t_test

bits 32
[section .text]



; ====================================================================================
;                          int get_ticks();
; ====================================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret


; ====================================================================================
;                          int get_ticks();
; ====================================================================================
t_test:
	mov	eax, _NR_test
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                          void write(char* buf, int len);
; ====================================================================================
write:
        mov     eax, _NR_write
        mov     ebx, [esp + 4]
        mov     ecx, [esp + 8]
        int     INT_VECTOR_SYS_CALL
        ret



; ====================================================================================
;                    int sendrec(int function, int src_dest, MESSAGE* msg);
; ====================================================================================
; Never call sendrec() directly, call send_recv() instead.
sendrec:
	mov	eax, _NR_sendrec
	mov	ebx, [esp + 4]	; function
	mov	ecx, [esp + 8]	; src_dest
	mov	edx, [esp + 12]	; p_msg
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                          void printx(char* s);
; ====================================================================================
printx:
	mov	eax, _NR_printx
	mov	edx, [esp + 4]
	int	INT_VECTOR_SYS_CALL
	ret
