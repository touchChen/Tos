# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
ENTRYPOINT	= 0x30400

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET	=   0x400

# Programs, flags, etc.
ASM		= nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32  -fno-stack-protector
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
BOOT    	= boot/boot.bin boot/loader.bin
KERNEL	        = kernel.bin
OBJS		= kernel/kernel.o kernel/start.o kernel/main.o kernel/protect.o \
                  kernel/i8259.o kernel/clock.o kernel/syscall.o kernel/proc.o \
                  lib/klib.o lib/klibc.o lib/string.o # 注意连接的头
DASMOUTPUT	= kernel.bin.asm

IMG             = x.img
FLOPPY          = /mnt/fd/

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
everything : $(BOOT) $(KERNEL)

all : realclean everything

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)
	#rm -f $(OBJS) $(BOOT) $(KERNEL)

realclean :
	rm -f $(OBJS) $(BOOT) $(KERNEL) $(DASMOUTPUT)

disasm :
	$(DASM) $(DASMFLAGS) $(KERNEL) > $(DASMOUTPUT)

# We assume that "x.img" exists in current folder
buildimg :
	dd if=boot/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp -fv boot/loader.bin $(FLOPPY)
	sudo cp -fv kernel.bin $(FLOPPY)
	sudo umount $(FLOPPY)

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc \
			boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(KERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

kernel/kernel.o : kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o : kernel/start.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o : kernel/i8259.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o : kernel/protect.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c 
	$(CC) $(CFLAGS) -o $@ $<

#include/type.h include/const.h include/protect.h include/proto.h include/interrupt.h  include/global.h
kernel/main.o  : kernel/main.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/proc.o: kernel/proc.c
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/klibc.o : lib/klibc.c include/type.h include/const.h include/proto.h 
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

