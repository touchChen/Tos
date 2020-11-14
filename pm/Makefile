#################################################
# Makefile of main.asm (x=[1,2,3...])
##################################################

SRC:=main.asm 
BIN:=$(subst .asm,.com,$(SRC))

SRCT:=run.asm
BINT:=$(subst .asm,.com,$(SRCT))


IMG			= pm.img
FLOPPY		= /mnt/fd/
UPTARGET	= tc@192.168.2.10:/home/tc/Tos/pm

.PHONY : everything	clean up

everything : $(BIN) $(BINT) 
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(BIN) $(FLOPPY) -v
	sudo cp $(BINT) $(FLOPPY) -v
	sudo umount $(FLOPPY)
	
up : $(IMG)
	scp ${IMG} ${UPTARGET}

clean :
	rm -f $(BIN) $(BINT)

$(BIN) : $(SRC)
	nasm $< -o $@

$(BINT) : $(SRCT)
	nasm $< -o $@
