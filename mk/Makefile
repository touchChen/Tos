##################################################
# Makefile
##################################################

# Programs, flags, etc.
ASM		= nasm
CC		= gcc
LD		= ld
ASMFLAGS	= -f elf
CFLAGS		= -c  -m32  -fno-stack-protector -fno-builtin
LDFLAGS		= -s  -m elf_i386 -Ttext 0x30400 


BOOT:=boot.asm
LDR:=loader.asm
STRING:=string.asm
KLIB:=klib.asm
KERNEL:=kernel.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .asm,.bin,$(KERNEL))
KERNEL_O:=$(subst .asm,.o,$(KERNEL))
STRING_O:=$(subst .asm,.o,$(STRING))
KLIB_O:=$(subst .asm,.o,$(KLIB))
START:=start.c
START_O:=$(subst .c,.o,$(START))



IMG:=x.img
FLOPPY:=/mnt/fd/

.PHONY : everything

everything : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)

clean :
	rm -f $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN) *.o

$(BOOT_BIN) : $(BOOT)
	$(ASM) $< -o $@

$(LDR_BIN) : $(LDR)
	$(ASM) $< -o $@

$(KERNEL_O) : $(KERNEL)
	$(ASM) $(ASMFLAGS) -o $@ $< 

$(STRING_O) : $(STRING)
	$(ASM) $(ASMFLAGS) -o $@ $< 

$(KLIB_O) : $(KLIB)
	$(ASM) $(ASMFLAGS) -o $@ $< 

$(START_O): $(START)
	$(CC) $(CFLAGS) -o $@ $<

$(KERNEL_BIN) : $(KERNEL_O) $(STRING_O) $(START_O) $(KLIB_O)
	$(LD) $(LDFLAGS) -o $@  $(KERNEL_O) $(STRING_O) $(START_O) $(KLIB_O)





