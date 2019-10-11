#################################################
# Makefile of main.asm (x=[1,2,3...])
##################################################

SRC:=main.asm 
BIN:=$(subst .asm,.com,$(SRC))

SRCT:=run.asm
BINT:=$(subst .asm,.com,$(SRCT))

.PHONY : everything

everything : $(BIN) $(BINT) 
	sudo mount -o loop pm.img /mnt/fd/
	sudo cp $(BIN) /mnt/fd/ -v
	sudo cp $(BINT) /mnt/fd/ -v
	sudo umount /mnt/fd/

$(BIN) : $(SRC)
	nasm $< -o $@

$(BINT) : $(SRCT)
	nasm $< -o $@
