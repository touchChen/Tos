##################################################
# Makefile of main.asm (x=[1,2,3...])
##################################################

SRC:=main.asm
BIN:=$(subst .asm,.com,$(SRC))

.PHONY : everything

everything : $(BIN)
	sudo mount -o loop pm.img /mnt/fd/
	sudo cp $(BIN) /mnt/fd/ -v
	sudo umount /mnt/fd/

$(BIN) : $(SRC)
	nasm $< -o $@
