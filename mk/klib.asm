[SECTION .data]
disp_pos	dd	0

[SECTION .text]

; 导出函数
global	disp_str
global  disp_al
global  disp_int

; ========================================================================
;                  void disp_str(char * info);
; ========================================================================
disp_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[disp_pos], edi

	pop	ebp
	ret


; ------------------------------------------------------------------------
; 显示 AL 中的数字
; ------------------------------------------------------------------------
disp_al:
	push	ecx
	push	edx
	push	edi

	mov	edi, [disp_pos]

	mov	ah, 0Fh			; 0000b: 黑底    1111b: 白字
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.begin:
	and	al, 01111b
	cmp	al, 9
	ja	.1
	add	al, '0'
	jmp	.2
.1:
	sub	al, 0Ah
	add	al, 'A'
.2:
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	.begin
	;add	edi, 2

	mov	[disp_pos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
; DispAL 结束-------------------------------------------------------------


; ------------------------------------------------------------------------
; 显示一个整形数
; ------------------------------------------------------------------------
disp_int:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	disp_al

	mov	eax, [esp + 4]
	shr	eax, 16
	call	disp_al

	mov	eax, [esp + 4]
	shr	eax, 8
	call	disp_al

	mov	eax, [esp + 4]
	call	disp_al

	mov	ah, 07h			; 0000b: 黑底    0111b: 灰字
	mov	al, 'h'
	push	edi
	mov	edi, [disp_pos]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[disp_pos], edi
	pop	edi

	ret
; DispInt 结束------------------------------------------------------------
