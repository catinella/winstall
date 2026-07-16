
.PHONY: clean

all:			selfInstaller

selfInstaller:	selfInstaller.o
			@echo "[ LD ] $@"
			@gcc -Wall -o $@ $<

%.o:			%.c
			@echo "[ CC ] $@"
			@gcc -Wall -o $@ -c $<

%.c:			%.c.tmpl
			@echo "[SCRIPT] $@"
			@if [ -n "$(WINSTALLCONF)" ]; then                          \
				sed 's|##WINSTALLCONF##|"$(WINSTALLCONF)"|' $< > $@ ; \
			else                                                        \
				echo "ERROR! WINSTALLCONF is undefined symbol";       \
				exit 127;                                             \
			fi

clean:
			@echo "[CLEAN]"
			@rm -fv selfInstaller.c selfInstaller.o

cleanall:		clean
			@rm -fv selfInstaller
