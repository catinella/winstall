#!/usr/bin/python3
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: messages.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	This module provides all functions used to generate messages, print banners.....
#	
#-------------------------------------------------------------------------------------------------------------------------------

import shutil
import sys

from errorCodes import *

_ttyEffect_bold    = '\033[1m'
_ttyEffect_reset   = '\033[0m'

_ttyEffect_black   = '\033[30m'
_ttyEffect_red     = '\033[31m'
_ttyEffect_green   = '\033[32m'
_ttyEffect_yellow  = '\033[33m'
_ttyEffect_blue    = '\033[34m'
_ttyEffect_magenta = '\033[35m'
_ttyEffect_cyan    = '\033[36m'
_ttyEffect_white   = '\033[37m'

def sysMessage (message, errorCode, exitFlag = True):
	#
	# Description:
	#	It prints a (error, warning...) status message and stops the process if the argument defined errorCode's value
	#	defines the event as an error and the exitFlag is set to true value
	#
	colorCode = 0
	if (errorCode > WIN_LAST_WARNING):
		colorCode = _ttyEffect_red
		symbol = "ERROR!"
	elif (errorCode >WIN_LAST_SUCCESS):
		colorCode = _ttyEffect_yellow
		symbol = "WARNING!"
	else:
		colorCode = _ttyEffect_cyan
		symbol = "INFO"
	
	print(f"[\033[{colorCode}m{symbol}{_ttyEffect_reset} {message}", file=sys.stderr)
	if (exitFlag and errorCode > 127):
		sys.exit(errorCode)
		
	return
	
def printLine (char='-'):
	#
	# Description:
	#	It prints a simple ASCII line
	#
	cols = shutil.get_terminal_size().columns
	for x in range(cols):
		print(char, end="")
	print("");


def printTitle (string, level):
	#
	# Description:
	#
	cols = shutil.get_terminal_size().columns
	match level:
		case 0:
			print(string)

		case 1:
			string = string.upper();
			string = _ttyEffect_bold + string + _ttyEffect_reset
			printLine()
			print(string.center(cols))
			printLine()

		case 2:
			print(f"[{_ttyEffect_bold}*{_ttyEffect_reset}] {string}")
