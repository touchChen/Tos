# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
ENTRYPOINT	= 0x1000

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET	= 0x400

# Programs, flags, etc.
ASM		= nasm
#DASM		= ndisasm
DASM		= objdump
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32  -Wall -fno-stack-protector
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386  -Map ./debug/krnl.map
#DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)
DASMFLAGS	= -D


#CFLAGS		= -I include/ -c -fno-builtin -Wall 
#LDFLAGS	= -Ttext $(ENTRYPOINT) -Map krnl.map  


# This Program
BOOT		= boot/boot.bin boot/loader.bin
KERNEL		= kernel.bin
OBJS		= kernel/kernel.o kernel/start.o kernel/main.o kernel/protect.o \
		  kernel/i8259.o kernel/clock.o kernel/keyboard.o kernel/syscall.o \
		  kernel/proc.o kernel/tty.o kernel/console.o \
		  kernel/systask.o kernel/printf.o kernel/hd.o kernel/global.o \
		  lib/klib.o lib/klibc.o lib/string.o lib/misc.o lib/fslib.o \
		  lib/proclib.o lib/syslog.o \
		  fs/fslib.o fs/main.o fs/disklog.o fs/graph.o \
		  local/main.o # 注意连接的头
DASMOUTPUT	= kernel.bin.asm

IMG			= x.img
FLOPPY		= /mnt/fd/
UPTARGET	= tc@192.168.2.13:/home/tc/beta/

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
everything : $(BOOT) $(KERNEL)

all : realclean everything

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)

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

up : ${IMG}
	scp ${IMG} ${UPTARGET}

boot/boot.bin : boot/boot.asm 
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm
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

kernel/keyboard.o : kernel/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o : kernel/tty.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o : kernel/console.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o  : kernel/main.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/proc.o : kernel/proc.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/systask.o : kernel/systask.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/printf.o : kernel/printf.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/hd.o : kernel/hd.c
	$(CC) $(CFLAGS) -o $@ $<

fs/fslib.o : fs/fslib.c
	$(CC) $(CFLAGS) -o $@ $<

fs/main.o : fs/main.c
	$(CC) $(CFLAGS) -o $@ $<

fs/disklog.o : fs/disklog.c
	$(CC) $(CFLAGS) -o $@ $<

fs/graph.o : fs/graph.c
	$(CC) $(CFLAGS) -o $@ $<


lib/klib.o : lib/klib.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/klibc.o : lib/klibc.c 
	$(CC) $(CFLAGS) -o $@ $<

lib/misc.o : lib/misc.c
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/fslib.o : lib/fslib.c
	$(CC) $(CFLAGS) -o $@ $<

lib/proclib.o : lib/proclib.c
	$(CC) $(CFLAGS) -o $@ $<

lib/syslog.o : lib/syslog.c
	$(CC) $(CFLAGS) -o $@ $<



local/main.o : local/main.c
	$(CC) $(CFLAGS) -o $@ $<


