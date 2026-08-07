#!/bin/bash

ec=1
ARCHNAME="$1"
FOODATA="./fooData"

if [ -n "$ARCHNAME" ]; then
	[ -d "$FOODATA"  ] && rm -rf "$FOODATA"
	[ -e "$ARCHNAME" ] && rm -f  "$ARCHNAME" 

	mkdir -pv "$FOODATA/d1" && echo "1111111111111111" > "$FOODATA/d1/uno" && \
	mkdir -pv "$FOODATA/d2" && echo "22222222"         > "$FOODATA/d2/due" && \
	mkdir -pv "$FOODATA/d3" && echo "3333"             > "$FOODATA/d3/tre" && \
	tar cvzf "$ARCHNAME" "$FOODATA"
	ec=$?

	[ -d "$FOODATA"  ] && rm -rf "$FOODATA"

else
	echo "ERROR! use $0 <TGZ archive name>" 2>&1
	ec=2
fi

exit $ec
