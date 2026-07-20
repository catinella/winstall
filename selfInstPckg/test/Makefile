#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: Makefile
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	This Makefile allows you to test the external software components
#
#-------------------------------------------------------------------------------------------------------------------------------
MAKEFLAGS += -r

SRCs := $(shell ls ?test_*.c)
OBJs := $(SRCs:.c=.o)
EXEs := $(SRCs:.c=)

MINUTE_PRFX ?= /usr/local
GDB         ?= 0

INCLUDE = -I. -I../ -I$(MINUTE_PRFX)/include

CODEGEN = $(MINUTE_PRFX)/share/minute/minute_cCodeGenerator.py
LIB     = -L$(MINUTE_PRFX)/lib -lMinuteC

ifeq ($(GDB), 0)
CFLAGS = -Wall -O2
else
CFLAGS = -Wall -g -O0
endif

.PHONY: clean cleanall

aLL:			$(EXEs)

utest_%:		utest_%.o %.o
			@echo "[ LD ] $@"
			@gcc -Wall -o $@ $^ $(LIB)

utest_%.o:		utest_%.c utest_%__main.sgc
			@echo "[ CC ] $@"
			@gcc $(CFLAGS) -o $@ $(INCLUDE) -c $<

%__main.sgc:	%.c
			@echo "[SCRIPT] $@"
			@$(CODEGEN) --srcFilename=$< --tgtFilename=$@ --verbose

%.o:			../%.c ../%.h
			@echo "[ CC* ] $@"
			@gcc $(CFLAGS) -o $@ $(INCLUDE) -c $<

clean:
			@echo "[CLEAN]"
			@rm -fv *.o *__main.sgc

cleanall:		clean
			@rm -fv $(EXEs)
