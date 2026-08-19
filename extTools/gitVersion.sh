#!/bin/bash
IFS="
"
ver=""
err=0
declare -i c=0
for cmt in $(git log --decorate=short |grep "^commit" |tr 'a-z' 'A-Z')
do
	if expr "$cmt" : '.*(.*TAG: [0-9.]\+.*)' >/dev/null; then
		ver="${cmt##*TAG:}"
		ver="${ver#[, ]}"
		ver="${ver%%[, ]*}"
		ver="${ver//[\t ()]/}"
		break
	else
		c=$(($c + 1))
	fi
done

if [ -z "$ver" ]; then
	echo "unknown"
	err=127
elif [ $c -eq 0 ]; then
	echo "$ver"
else
	echo "$ver.dev$c"
fi

exit $err
