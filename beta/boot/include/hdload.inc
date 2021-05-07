;; super block will be stored at: [0x700,0x900)
SUPER_BLK_SEG		equ	0x70


;; paging
PAGE_DIR_BASE		equ	0x100000
PAGE_TBL_BASE		equ	0x101000

;; where_loader_is_loaded
LOADER_SEG		equ	0x9000
LOADER_OFF		equ	0x100
LOADER_PHY_ADDR		equ	LOADER_SEG * 0x10

;; where_kernel_is_loaded
KERNEL_FILE_SEG		equ	0x7000
KERNEL_FILE_OFF		equ	0
KERNEL_FILE_PHY_ADDR	equ	KERNEL_FILE_SEG * 0x10


;; we don't calculate the base sector nr of the root device while loading
;; but define it as a macro for two reasons:
;; 1. it is a constant for any certain system
;; 2. it simplifies boot sector and loader
ROOT_BASE		equ	0x4681

;; corresponding with include/sys/fs.h
SB_MAGIC_V1				equ	0x98
SB_MAGIC				equ	4 *  0
SB_NR_INODES			equ	4 *  1
SB_NR_SECTS				equ	4 *  2
SB_NR_IMAP_SECTS		equ	4 *  3
SB_NR_SMAP_SECTS		equ	4 *  4
SB_N_1ST_SECT			equ	4 *  5
SB_NR_INODE_SECTS		equ	4 *  6
SB_ROOT_INODE			equ	4 *  7
SB_INODE_SIZE			equ	4 *  8
SB_INODE_ISIZE_OFF		equ	4 *  9
SB_INODE_START_OFF		equ	4 * 10
SB_DIR_ENT_SIZE			equ	4 * 11
SB_DIR_ENT_INODE_OFF	equ	4 * 12
SB_DIR_ENT_FNAME_OFF	equ	4 * 13
