;%define	_BOOT_DEBUG_	; 做 Boot Sector 时一定将此行注释掉!将此行打开后可用 nasm Boot.asm -o Boot.com 做成一个.COM文件易于调试

%ifdef	_BOOT_DEBUG_
	org  0100h			; 调试状态, 做成 .COM 文件, 可调试
%else
	org  07c00h			; Boot 状态, Bios 将把 Boot Sector 加载到 0:7C00 处并开始执行
%endif

;================================================================================================
%ifdef	_BOOT_DEBUG_
BaseOfStack		equ	0100h	; 调试状态下堆栈基地址(栈底, 从这个位置向低地址生长)
%else
BaseOfStack		equ	07c00h	; 堆栈基地址(栈底, 从这个位置向低地址生长)
%endif

BaseOfLoader		equ	09000h	; LOADER.BIN 被加载到的位置 ----  段地址
OffsetOfLoader		equ	0100h	; LOADER.BIN 被加载到的位置 ---- 偏移地址
;================================================================================================


	jmp	short LABEL_START		; Start to boot.
	nop				; 这个 nop 不可少

	%include	"fat12hdr.inc"

LABEL_START:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov		ss, ax
	mov		sp, BaseOfStack

	; 清屏
	mov		ax, 0600h		; AH = 6,  AL = 0h
	mov		bx, 0700h		; 黑底白字(BL = 07h)
	mov		cx, 0			; 左上角: (0, 0)
	mov		dx, 0184fh		; 右下角: (24, 79)
	int		10h				; int 10h

	mov		dh, 0			; "Booting  "
	call	DispStr		; 显示字符串


	xor		ah, ah	; `.
	xor		dl, dl	;  |  软驱复位
	int		13h		; /


	; 下面在 A 盘的根目录寻找 LOADER.BIN
	mov		word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp		word [wRootDirSizeForLoop], 0	;  `. 判断根目录区是不是已经读完
	jz		LABEL_NO_LOADERBIN				;  /  如果读完表示没有找到 LOADER.BIN
	dec		word [wRootDirSizeForLoop]		; /

	mov		ax, BaseOfLoader
	mov		es, ax				; es <- BaseOfLoader
	mov		bx, OffsetOfLoader	; bx <- OffsetOfLoader
	mov		ax, [wSectorNo]		; ax <- Root Directory 中的某 Sector 号
	mov		cl, 1
	call	ReadSector			;从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
	
	mov		di, OffsetOfLoader	; es:di -> BaseOfLoader:0100
	cld							; 将标志寄存器Flag的方向标志位DF清零, 结合lodsb
	mov		dx, 10h  			; 一个扇区最多有10h个文件(32字节)，512=10h*32
LABEL_SEARCH_FOR_LOADERBIN:
    mov		si, LoaderFileName 	; ds:si -> "LOADER  BIN"  要与 es:di 上被载入内存的文件名比对
	cmp		dx, 0				; `. 循环次数控制,
	jz		LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR ;  / 如果已经读完了一个 Sector,
	dec		dx				   	; /  就跳到下一个 Sector
	mov		cx, 11 				;文件名8个字节，扩展名3个字节
LABEL_CMP_FILENAME:
	cmp		cx, 0
	jz		LABEL_FILENAME_FOUND	; 如果比较了 11 个字符都相等, 表示找到
	dec		cx
	lodsb							; ds:si -> al
	cmp		al, byte [es:di]
	jz		LABEL_GO_ON
	jmp		LABEL_DIFFERENT			; 只要发现不一样的字符就表明本 DirectoryEntry
									; 不是我们要找的 LOADER.BIN
LABEL_GO_ON:
	inc		di    					; 指向载入内存空间每个文件名的字符，一个扇区10h个文件项，每个文件项目的前11位是文件名
	jmp	LABEL_CMP_FILENAME			; 继续循环

LABEL_DIFFERENT:
	and		di, 0FFE0h					;  di &= E0 为了让它指向本条目开头
	add		di, 20h						
	jmp		LABEL_SEARCH_FOR_LOADERBIN	

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add		word [wSectorNo], 1
	jmp		LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
	mov		dh, 2			; "No LOADER."
	call	DispStr			; 显示字符串

%ifdef	_BOOT_DEBUG_
	mov		ax, 4c00h		
	int		21h			;  没有找到 LOADER.BIN, 回到 DOS
%else
	jmp		$			; 没有找到 LOADER.BIN, 死循环在这里
%endif



LABEL_FILENAME_FOUND:			; 找到 LOADER.BIN 后便来到这里继续
	and		di, 0FFE0h			; di -> 当前条目的开始
	add		di, 01Ah			; di -> 首 Sector  文件开始的第一个簇
	mov		cx, word [es:di]
	push	cx					; 保存此 Sector 在 FAT 中的序号
	
	add		cx, SectorFakeDataArea	; cx -> fat 对应 数据区中的扇区号，fat表前两项不用
	mov		ax, BaseOfLoader
	mov		es, ax				; es <- BaseOfLoader
	mov		bx, OffsetOfLoader	; bx <- OffsetOfLoader
	mov		ax, cx				; ax <- Sector 号

LABEL_GOON_LOADING_FILE:
	push	ax					; `.
	push	bx					;  |
	mov		ah, 0Eh				;  | 每读一个扇区就在 "Booting  " 后面
	mov		al, '.'				;  | 打一个点, 形成这样的效果:
	mov		bl, 0Fh				;  | Booting ......
	int		10h					;  |
	pop		bx					;  |
	pop		ax					; /


	mov		cl, 1
	call	ReadSector
	pop		ax					; 取出此 Sector 在 FAT 中的序号
	call	GetFATEntry			; 获取下个fat项，存放站ax中
	cmp		ax, 0FFFh
	jz		LABEL_FILE_LOADED
	push	ax					; 保存 Sector 在 FAT 中的序号
	
	add		bx, [BPB_BytsPerSec]    ; 内存地址增加一个扇区
    add 	ax, SectorFakeDataArea
	jmp		LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:

	mov		dh, 1				; "Ready."
	call	DispStr				; 显示字符串

; *****************************************************************************************************
	jmp	BaseOfLoader:OffsetOfLoader	; 这一句正式跳转到已加载到内
								; 存中的 LOADER.BIN 的开始处，
								; 开始执行 LOADER.BIN 的代码。
								; Boot Sector 的使命到此结束。
; *****************************************************************************************************



;============================================================================
;变量
wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

;字符串
LoaderFileName		db	"LOADER  BIN", 0 ; LOADER.BIN 之文件名
; 为简化代码, 下面每个字符串的长度均为 MessageLength
MessageLength		equ	9
BootMessage		db	"Booting  " ; 9字节, 不够则用空格补齐. 序号 0
Message1		db	"Ready.   " ; 9字节, 不够则用空格补齐. 序号 1
Message2		db	"No LOADER" ; 9字节, 不够则用空格补齐. 序号 2
;============================================================================


;----------------------------------------------------------------------------
; 函数名: DispStr
;----------------------------------------------------------------------------
; 作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
DispStr:
	mov		ax, MessageLength
	mul		dh
	add		ax, BootMessage
	mov		bp, ax				; `.
	mov		ax, ds				;  | ES:BP = 串地址
	mov		es, ax				; /
	mov		cx, MessageLength	; CX = 串长度
	mov		ax, 01301h			; AH = 13,  AL = 01h
	mov		bx, 0007h			; 页号为0(BH = 0) 黑底白字(BL = 07h)
	mov		dl, 0
	int		10h					; int 10h
	ret


;----------------------------------------------------------------------------
; 函数名: ReadSector
;----------------------------------------------------------------------------
; 作用:
;	从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
ReadSector:
	; -----------------------------------------------------------------------
	; 怎样由扇区号求扇区在磁盘中的位置 (扇区号 -> 柱面号, 起始扇区, 磁头号)
	; -----------------------------------------------------------------------
	; 设扇区号为 x
	;                          ┌ 柱面号 = y >> 1
	;       x           ┌ 商 y ┤
	; -------------- => ┤      └ 磁头号 = y & 1
	;  每磁道扇区数     │
	;                   └ 余 z => 起始扇区号 = z + 1
	push	bp
	mov		bp, sp
	sub		esp, 2 			; 辟出两个字节的堆栈区域保存要读的扇区数: byte [bp-2]

	mov		byte [bp-2], cl
	push	bx				; 保存 bx
	mov		bl, [BPB_SecPerTrk]		; bl: 除数
	div		bl				; y 在 al 中, z 在 ah 中
	inc		ah				; z ++
	mov		cl, ah			; cl <- 起始扇区号
	mov		dh, al			; dh <- y
	shr		al, 1			; y >> 1 (y/BPB_NumHeads)
	mov		ch, al			; ch <- 柱面号
	and		dh, 1			; dh & 1 = 磁头号
	pop		bx				; 恢复 bx
	; 至此, "柱面号, 起始扇区, 磁头号" 全部得到
	mov		dl, [BS_DrvNum]		; 驱动器号 (0 表示 A 盘)
.GoOnReading:
	mov		ah, 2			; 读
	mov		al, byte [bp-2]	; 读 al 个扇区
	int		13h
	jc		.GoOnReading	; 如果读取错误 CF 会被置为 1, 
							; 这时就不停地读, 直到正确为止
	add		esp, 2
	pop		bp

	ret

;----------------------------------------------------------------------------
; 函数名: GetFATEntry
;----------------------------------------------------------------------------
; 作用:
;	找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;	需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry:
	push	es
	push	bx
	push	ax
	mov		ax, BaseOfLoader	; `.
	sub		ax, 0100h			;  | 在 BaseOfLoader 后面留出 4K 空间用于存放 FAT
	mov		es, ax				; /
	pop		ax
	mov		byte [bOdd], 0
	mov		bx, 3
	mul		bx					; dx:ax = ax * 3
	mov		bx, 2
	div		bx					; dx:ax / 2  ==>  ax <- 商, dx <- 余数
	cmp		dx, 0
	jz		LABEL_EVEN			; 偶数
	mov		byte [bOdd], 1		; 奇数
LABEL_EVEN:		
	; 现在 ax 中是 FATEntry 在 FAT 中的偏移量,下面来
	; 计算 FATEntry 在哪个扇区中(FAT占用不止一个扇区)
	xor		dx, dx			
	mov		bx, [BPB_BytsPerSec]
	div		bx 			; dx:ax / BPB_BytsPerSec
		   				;  ax <- 商 (FATEntry 所在的扇区相对于 FAT 的扇区号)
		   				;  dx <- 余数 (FATEntry 在扇区内的偏移)。
	push	dx
	mov		bx, 0 		; bx <- 0 于是, es:bx = (BaseOfLoader - 100):00  这个位置没有使用
	add		ax, SectorNoOfFAT1 ; 此句之后的 ax 就是 FATEntry 所在的扇区号
	mov		cl, 2
	call	ReadSector  ; 读取 FATEntry 所在的扇区, 一次读两个, 避免在边界
			   			; 发生错误, 因为一个 FATEntry 可能跨越两个扇区
	pop		dx
	add		bx, dx 		; bx包含12位fat值的内存空间地址
	mov		ax, [es:bx]
	cmp		byte [bOdd], 1
	jnz		LABEL_EVEN_2 
	shr		ax, 4
LABEL_EVEN_2:			; 偶数
	and		ax, 0FFFh

LABEL_GET_FAT_ENRY_OK:

	pop		bx
	pop		es
	ret
;----------------------------------------------------------------------------

times 	510-($-$$)	db	0	; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55				; 结束标志
