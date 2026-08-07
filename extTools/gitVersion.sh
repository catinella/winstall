#!/bin/bash
IFS="
"
ver=""
err=0
for cmt in $(git log --decorate=short |grep "^commit" |tr 'a-z' 'A-Z')
do
	if expr "$cmt" : '.*(TAG: [0-9.]\+)' >/dev/null; then
		ver="$cmt"
		ver="${ver##*TAG:}"
		ver="${ver//[\t ()]/}"
		break
	else
		c=$(($c + 1))
	fi
done

if [ -z "$ver" ]; then
	echo "unknown"
	err=127
else
	echo "$ver.dev$c"
fi

exit $err
