##################################################
# Makefile
##################################################

# Programs, flags, etc.
ASM		= nasm
CC		= gcc
LD		= ld
ASMFLAGS	= -f elf
CFLAGS		= -c  -m32  -fno-stack-protector
LDFLAGS		= -s  -m elf_i386 -Ttext 0x30400 


BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .asm,.bin,$(KERNEL))
KERNEL_O:=$(subst .asm,.o,$(KERNEL))

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
	$(ASM) $(ASMFLAGS) $< -o $@ 

$(KERNEL_BIN) : $(KERNEL_O)
	$(LD) $(LDFLAGS)  $< -o $@ 




