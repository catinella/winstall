#!/bin/bash

dep="$1"
err=0
linkedCache="/etc/ld.so.cache"

echo "Checking for dependences"
for file in $*
do
	if grep -q "$file" $linkedCache
	then
		echo -e "\t   OK     $file"
	else
		echo -e "\tMISSING!! $file"
		err=127
	fi
done

exit $err
