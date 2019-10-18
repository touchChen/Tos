##################################################
# Makefile
##################################################

BOOT:=boot.asm
#LDR:=loader.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
#LDR_BIN:=$(subst .asm,.bin,$(LDR))

everything : $(BOOT_BIN) #$(LDR_BIN)
	dd if=$(BOOT_BIN) of=x.img bs=512 count=1 conv=notrunc
	#sudo mount -o loop x.img /mnt/fd/
	#sudo cp $(LDR_BIN) /mnt/fd/ -v
	#sudo umount /mnt/fd/

clean :
	rm -f $(BOOT_BIN) #$(LDR_BIN)

$(BOOT_BIN) : $(BOOT)
	nasm $< -o $@

#$(LDR_BIN) : $(LDR)
#	nasm $< -o $@
