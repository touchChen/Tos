# usr/Makefile

HD		= ../50m.img

ASM		= nasm
DASM	= objdump
CC		= gcc
LD		= ld
ASMFLAGS	= -I ../include/ -f elf
CFLAGS		= -I ../include/ -c -fno-builtin -m32  -Wall -fno-stack-protector
LDFLAGS		= -Ttext 0x1000 -m elf_i386
DASMFLAGS	= -D
LIB		= ../lib/libh.a
BIN		= echo pwd

# All Phony Targets
.PHONY : everything final clean realclean disasm all install over


# Default starting position
nop : over

# Default starting position
everything : $(BIN)

install : all clean
	cp ../boot/hdloader.bin ./ -v
	cp ../kernel.bin ./ -v
	tar vcf inst.tar hdloader.bin $(BIN) kernel.bin
	dd if=inst.tar of=$(HD) seek=`echo "obase=10;ibase=16;(\`grep -e '#define[[:space:]]*ROOT_BASE' ../include/hd.h | sed -e 's/.*0x//g'\`+\`grep -e '#define[[:space:]]*INSTALL_START_SECT' ../include/hd.h | sed -e 's/.*0x//g'\`)*200" | bc` bs=1 count=`ls -l inst.tar | awk -F " " '{print $$5}'` conv=notrunc


over : install realallclean

all : realclean everything

final : all clean

clean :
	rm -f *.o

realclean :
	rm -f  $(BIN) *.o

realallclean :
	rm -f inst.tar $(BIN) *.o 

start.o : start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
	
echo.o: echo.c ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<

pwd.o: pwd.c  ../include/stdio.h
	$(CC) $(CFLAGS) -o $@ $<



echo : echo.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?

pwd : pwd.o start.o $(LIB)
	$(LD) $(LDFLAGS) -o $@ $?
	
	
	
tar : all clean
	cp ../boot/hdl.bin ./ -v
	cp ../kernel.bin ./ -v
	tar vcf inst.tar hdl.bin $(BIN) kernel.bin
	
