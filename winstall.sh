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
#	It is a generic installer bash-script for POSIX systems and it has been written to replace the typical INSTALL.sh script.
#	winstall's features:
#		- Software installation
#		- Software removing
#		- Self-installing package creation (NOT YET IMPLEMENTED)
#
#	To use the winstall.sh script, you must run it with the root folder of your project, and respect the following syntax:
#
#		<path>/winstall.sh --cmd={install|uninstall|build|clean|pkg} \
#			[--verbose] \
#			[--tmpFolder=<dir>] \
#			[--dataLogFolder=<dir>] \
#			[--prefix=<dir>] \
#			[--prjName=<string>]
#
#	How to set your software to be installed by winstall
#	====================================================
#	In order to find and recognize the files you want install (or packaging), the process looks for configuration files where
#	their names match with the "winstall_<label>.conf" pattern. Everyone of them is associated to the folder that contains it,
#	and in these files winstall process will have to find the following information:
#
#		1) How to build the files
#		2) Which files will be installed (or copied into the package) and where they will be stored
#		4) The file properties to set
#		5) how to clean the folder (used by --cmd=clean option)
#
#	How to create a winstall_<label>.conf file:
#		BUILDER="<commands>"      # Exe-file or commands-sequence to build the files you want to install (eg. make all)
#		FILES="<files list>"      # Files you want to install (eg. lib*.a file.h)
#		TGTPATH="<path>"          # Folder where the files will be stored (eg. lib)
#		CHMOD="<n1><n2><n3>"      # File permissions
#		CHOWN="<user>"            # File owner
#		CLEANER="<commands>"      # Exe-file or commands-sequence to remove the produced files (eg. make cleanall)
#	
#	winstall's main-configuration file and optional values:
#	=======================================================
#	Some other optional information can be provided wit the winstall.conf file. It MUST be stored in the main folder of your
#	software project, and must respect the following syntax:
#		PRJNAME=<string>          # It can be set also with --prjName=<string> file's argument
#		PREINST=<exec-file>       # The script to run before the files copying step
#		POSTINST=<exec-file>      # The script to run after the files copying step
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
PREINST=""
POSTINST=""
VERBOSE=0
CONFFILE="$callerPWD/winstall.conf"
cmd=""
err=0


#
# Configuration file reading
#
[ -e "$CONFFILE" ] && {
	[ $VERBOSE -eq 1 ] && echo "[i] configuration file reading"
	source "$CONFFILE"
}


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
		echo "ERROR! I cannot get the name of the project|package"
		exit 134
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
	# Pre-install script
	[ "$cmd" = "install" -a -n "$PREINST" ] && {
		[ $VERBOSE -eq 1 ] && printTitle "Pre installation script starting" 2
		PREFIX="$PREFIX" DATALOGFOLDER="$DATALOGFOLDER" TMPFOLDER="$TMPFOLDER" PRJNAME="$PRJNAME" ./$PREINST
	}

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
		[ -n "$POSTINST" -o -n "$PREINST" ] && {
			if mkdir "$TMPFOLDER/winstall" ; then
				[ -n "$POSTINST" ] && {
					[ $VERBOSE -eq 1 ] && echo "\"$POSTINST\" adding to the package"
					cp "$callerPWD/$POSTINST" "$TMPFOLDER/winstall/."
					chmod 750 "$TMPFOLDER/winstall/${POSTINST##*/}"
				}
				[ -n "$PREINST" ] && {
					[ $VERBOSE -eq 1 ] && echo "\"$PREINST\" adding to the package"
					cp "$callerPWD/$PREINST" "$TMPFOLDER/winstall/."
					chmod 750 "$TMPFOLDER/winstall/${PREINST##*/}"
				}
			else
				errAndExit "I cannot create the \"$TMPFOLDER/winstall\" dir" 164
			fi
		}
		if cd "$TMPFOLDER" ; then
			#echo "[i] Temp directory: $PWD"
			tar cvzf "${callerPWD}/${PRJNAME}.tgz" * || \
				errAndExit "I cannot create the \"${callerPwd}/${PRJNAME}\" TGZ archive" 143
			cd - >/dev/null
		else
			errAndExit "I cannot enter in the \"$TMPFOLDER\" directory" 145
		fi
	
	elif [ "$cmd" = "install" ]; then
		err=0

		[ $VERBOSE -eq 1 ] && printTitle "Installation data registering..." 2
		echo "$fileList" |tr ' ' '\n' > "${DATALOGFOLDER}/$PRJNAME" || err=150
	
		[ "$cmd" = "install" -a -n "$POSTINST" ] && {
			[ $VERBOSE -eq 1 ] && printTitle "Post installation script starting" 2
			PREFIX="$PREFIX" DATALOGFOLDER="$DATALOGFOLDER" TMPFOLDER="$TMPFOLDER" PRJNAME="$PRJNAME" ./$POSTINST
		}
		
		[ $err -eq 0 ] || errAndExit "I cannot update the installed packages DB" $err
	fi
fi
	
exit 0
	
