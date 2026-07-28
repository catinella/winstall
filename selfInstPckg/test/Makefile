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
#	This Makefile allows you to test the single software components
#
#	Configurable symbols:
#		GDB         = {0|1}         # It enables the GCC debugger support
#		DEBUG       = {0-3}         # It sets the debug messages verbosity (0=silence)
#		MINUTE_PRFX = <folder>      # It allows you to specify a different installation path prefix for Minute software
#		TEXTFILE    = <filename>    # The text file used by utest_elfReading unit-test
#		TGZFILE     = <filename>    # The foo data archive used by utest_archiveExtractor unit test
#
#
#-------------------------------------------------------------------------------------------------------------------------------
MAKEFLAGS += -r

SRCs := $(shell ls ?test_*.c)
OBJs := $(SRCs:.c=.o)
EXEs := $(SRCs:.c=)

MINUTE_PRFX ?= /usr/local
GDB         ?= 0
TEXTFILE    ?= /etc/passwd
TGZFILE     ?= utest_archiveExtractor.tar.gz
DEBUG       ?= 0

INCLUDE = -I. -I../ -I$(MINUTE_PRFX)/include
CODEGEN = $(MINUTE_PRFX)/share/minute/minute_cCodeGenerator.py
LIB     = -L$(MINUTE_PRFX)/lib -lMinuteC $(shell pkg-config --libs libarchive) -lc
SYMBOLS = "-DDEBUG=$(DEBUG)" "-DTEXTFILE=\"$(TEXTFILE)\""

ifeq ($(GDB), 0)
CFLAGS = -Wall -O2
else
CFLAGS = -Wall -g -O0
$(warning GDB support added)
endif

.PHONY: clean cleanall

all:				$(EXEs)

utest_archiveExtractor:	utest_archiveExtractor.o archiveExtractor.o elfReading.o
				@echo "[ LD* ] $@"
				@gcc -Wall -o $@ $^ $(LIB)
				@echo "[SCRIPT] $(TGZFILE)"
				@./makeFooArchive.sh "$(TGZFILE)"
				@echo "[ ADD ] $(TGZFILE)"
				@cat $(TGZFILE) >> $@

utest_elfReading:		utest_elfReading.o elfReading.o
				@echo "[ LD* ] $@"
				@gcc -Wall -o $@ $^ $(LIB)
				@echo "[ ADD ] $(TEXTFILE)"
				@cat $(TEXTFILE) >> $@

utest_%:			utest_%.o %.o
				@echo "[ LD ] $@"
				@gcc -Wall -o $@ $^ $(LIB)

utest_%.o:			utest_%.c utest_%__main.sgc
				@echo "[ CC ] $@"
				@gcc $(CFLAGS) -o $@ $(INCLUDE) $(SYMBOLS) -c $<

%__main.sgc:		%.c
				@echo "[SCRIPT] $@"
				@$(CODEGEN) --srcFilename=$< --tgtFilename=$@

%.o:				../%.c ../%.h
				@echo "[ CC* ] $@"
				@gcc $(CFLAGS) -o $@ $(INCLUDE) $(SYMBOLS) -c $<

clean:
				@echo "[CLEAN]"
				@rm -fv *.o *__main.sgc

cleanall:			clean
				@rm -fv $(EXEs) $(TGZFILE)
