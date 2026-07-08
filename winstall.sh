#!/bin/bash
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: winstall.sh
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	It is a generic installer bash-script for POSIX systems.
#	
#	use: <path>/winstall.sh --cmd={install|uninstall|build|clean|pkg} \
#		[--verbose] \
#		[--tmpFolder=<dir>] \
#		[--dataLogFolder=<dir>] \
#		[--prefix=<dir>] \
#		[--prjName=<string>]
#
#	How to create a winstall_<label>.conf file:
#	===========================================
#		BUILDER="<commands>"      # Exe-file or commands-sequence to build the files you want to install (eg. make all)
#		FILES="lib*.a minute.h"   # Files you want to install
#		TGTPATH="$PREFIX/lib"     # Folder where the files will be stored
#		CHMOD="644"               # File permissions
#		CHOWN="root"              # File owner
#		CLEANER="<commands>"      # Exe-file or commands-sequence to remove the produced files
#	
#	
#	
#	
#-------------------------------------------------------------------------------------------------------------------------------

errUsage() {
	local    file="$1"
	local -i err=$2
	echo -e "$file --cmd={install|uninstall|build|clean|pkg} \\"
	echo -e "\t[--verbose] \\"
	echo -e "\t[--tmpFolder=<dir>] \\"
	echo -e "\t[--dataLogFolder=<dir>] \\"
	echo -e "\t[--prefix=<dir>] \\"
	echo -e "\t[--prjName=<string>"
	[ $err -gt 127 ] && exit $err
}

doit() {
	local action="$1"
	if [ -n "$action" ]; then
		$action
	else
		echo "WARNING! Nothing to build in this folder"
	fi
	return $?
}

errDir() {
	errAndExit "The \"$1\" directory is not present and I cannot create it" $2
}

errAndExit() {
	local    msg="$1"
	local -i err=$2
	echo "ERROR! $msg"
	[ $err -gt 127 ] && exit $err

}

getFreeName() {
	local file="$1"
	local -i counter=0
	while [ -e $file ]; do
		file="$1-$counter"
		counter=$(($counter+1))
	done
	echo $file
	return 0
}

printLine() {
	local symbol="$1"
	local -i length=$2
	local x=0
	local buffer=""
	while [ $x -lt $length ]; do
		buffer="${buffer}${symbol}"
		x=$(($x+1))
	done
	echo "$buffer"
	return 0
}

printTitle() {
	local msg="$1"
	local -i level=$2
	local buffer=""
	case $level in
		0)
			echo "$msg"
		;;
		1)
			echo ""
			printLine "-" $(tput cols)
			buffer="$(printLine ' ' $(($(tput cols)/2 - ${#msg}/2)))"
			echo "${buffer}${msg^^}"
			printLine "-" $(tput cols)
			echo ""
		;;
		2)
			echo ""
			echo "[*] $msg"
		;;
	esac
	return 0
}
#-------------------------------------------------------------------------------------------------------------------------------
#                                                      M A I N 
#-------------------------------------------------------------------------------------------------------------------------------
myPwd="${0%/*}"
callerPWD="$PWD"
PREFIX="/usr/local"
DATALOGFOLDER="/var/local/winstall"
TMPFOLDER="/tmp/winstall"
PRJNAME=""
VERBOSE=0
CONFFILE="$callerPWD/winstall.conf"
cmd=""
err=0

#
# Argument parsing
#
for arg in $*
do
	if expr "$arg" : '^--[^-= \t]\+' >/dev/null; then
		arg="${arg#--}"
		if expr "$arg" : '[^=]\+=.\+' >/dev/null; then
			key="${arg%%=*}"
			val="${arg#*=}"
			
			case "$key" in
				"cmd")
					cmd="$val"
				;;
				"prjName")
					PRJNAME="$val"
				;;
				"tmpFolder")
					TMPFOLDER="$val"
				;;
				"dataLogFolder")
					DATALOGFOLDER="$val"
				;;
				"prefix")
					PREFIX="$value"
				;;
				*)
					echo "ERROR! \"--${key}\" is not a valid argument"
					errUsage $0 131
				;;
			esac
		else
			case "$arg" in
				"verbose")
					VERBOSE=1
				;;
				*)
					echo "ERROR! \"$arg\" is not a valid argument"
					errUsage $0 131
				;;
			esac
		fi
	else
		echo "ERROR! \"$arg\" is not a valid argument"
		errUsage $0 131
	fi
done


#
# Checking for the arguments
#
[ "$cmd" != "install" -a "$cmd" != "uninstall" -a "$cmd" != "build" -a "$cmd" != "clean" -a "$cmd" != "pkg" ] &&  \
	errAndExit "\"$cmd\" is not a valid command" 133


[ $VERBOSE -eq 1 ] && printTitle "winstall" 1

#
# Project name loading
#
[ "$cmd" = "install" -o "$cmd" = "pkg" -o "$cmd" = "uninstall" ] && {
	[ -z "$PRJNAME" ] && {
		[ $VERBOSE -eq 1 ] && echo "[i] configuration file reading"
		
		[ -e "$CONFFILE" ] && source "$CONFFILE"
		
		[ -z "$PRJNAME" ] && {	
			echo "ERROR! I cannot get the name of the project|package"
			exit 134
		}
	}
	[ $VERBOSE -eq 1 ] && echo "[i] Software name: $PRJNAME"
}


#
# Initialization
#
if [ "$cmd" = "install" ]; then
	[ -d "$DATALOGFOLDER" ] || mkdir -p "$DATALOGFOLDER" || errDir "$DATALOGFOLDER" 135
	[ -d "$PREFIX" ]        || mkdir -p "$PREFIX"        || errDir "$PREFIX"        135
	fileList=""
	
elif [ "$cmd" = "pkg" ]; then
	TMPFOLDER=$(getFreeName "$TMPFOLDER")
	[ "$cmd" = "pkg" ] && PREFIX="${TMPFOLDER}/${PREFIX}"
	[ -d "$TMPFOLDER" ]     || mkdir -p "$TMPFOLDER"     || errDir "$TMPFOLDER"     135
	[ -d "$PREFIX" ]        || mkdir -p "$PREFIX"        || errDir "$PREFIX"        135

elif [ "$cmd" = "uninstall" ]; then
	[ -d "$DATALOGFOLDER" ] ||
		errAndExit "The \"$DATALOGFOLDER\" directory does not exists"  137
		
	[ -e "$DATALOGFOLDER/$PRJNAME" ] ||
		errAndExit "No data found for the installed \"$PRJNAME\" software" 139
fi

if [ "$cmd" = "uninstall" ]; then
	if [ -e "${DATALOGFOLDER}/$PRJNAME" ]; then
		rm -fv $(cat "${DATALOGFOLDER}/$PRJNAME") && rm -fv "${DATALOGFOLDER}/$PRJNAME"
	else
		errAndExit "The \"$PRJNAME\" looks like not installed" 142
	fi

else
	for row in $(find . -name "winstall_*.conf")
	do
		dir=${row%/winstall_*.conf}
		wfile=${row##*/}
	
		if cd $dir ; then
			[ $VERBOSE -eq 1 ] && printTitle "Entering i the \"$dir\" directory" 2
	
			# Data loading...
			source $wfile
	
			# Target path building
			if expr "$TGTPATH" : '\/' >/dev/null ; then
				[ "$cmd" = "pkg" ] && TGTPATH="${TMPFOLDER}/${TGTPATH#/}"
			else
				TGTPATH="${PREFIX%/}/${TGTPATH#/}"
			fi
			
			case "$cmd" in
				"clean")
					doit "$CLEANER"
				;;
				"build")
					doit "$BUILDER"
				;;
				"install"|"pkg")
					[ -d "$TGTPATH" ] || mkdir -p "$TGTPATH" || errDir "$TGTPATH" 141
	
					doit "$BUILDER" || exit 146
						
					for file in $FILES ; do
						cp -v $file $TGTPATH && {
							[ -n "$CHMOD" ] && chmod $CHMOD $TGTPATH/${file##*/}
							[ -n "$CHOWN" ] && chown $CHOWN $TGTPATH/${file##*/}
							fileList="$fileList $TGTPATH/${file##*/}"
						}
					done
	
				;;
			esac
		else
			echo "ERROR! I cannot enter in the \"$dir\" directory" 
			break;
		fi
	
		cd - >/dev/null
	done
	
	
	if [ "$cmd" = "pkg" ]; then
		[ $VERBOSE -eq 1 ] && printTitle "pkg construction..." 2
		if cd "$TMPFOLDER" ; then
			#echo "[i] Temp directory: $PWD"
			tar cvzf "${callerPWD}/${PRJNAME}.tgz" * || \
				errAndExit "I cannot create the \"${callerPwd}/${PRJNAME}\" TGZ archive" 143
			cd - >/dev/null
		else
			errAndExit "I cannot enter in the \"$TMPFOLDER\" directory" 145
		fi
	
	else
		[ $VERBOSE -eq 1 ] && printTitle "Installation data registering..." 2
		echo "$fileList" |tr ' ' '\n' > "${DATALOGFOLDER}/$PRJNAME" || \
			errAndExit "I cannot update the installed packages DB" 145
	fi
fi
	
exit 0
	
