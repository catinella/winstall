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
#	ANSI colors codes:
#		+------+---------+
#		| code |  color  |
#		+------+---------+
#		| 30   | Nero    |
#		| 31   | Rosso   |
#		| 32   | Verde   |
#		| 33   | Giallo  |
#		| 34   | Blu     |
#		| 35   | Magenta |
#		| 36   | Ciano   |
#		| 37   | Bianco  |
#		| 0    | Reset   |
#		+------+---------+
#
#-------------------------------------------------------------------------------------------------------------------------------

import shutil
import sys

from errorCodes import *

def sysMessage (message, errorCode, exitFlag = True):
	#
	# Description:
	#	It prints a (error, warning...) status message and stops the process if the argument defined errorCode's value
	#	defines the event as an error and the exitFlag is set to true value
	#
	colorCode = 0
	if (errorCode > WIN_LAST_WARNING):
		colorCode = 31
		symbol = "ERROR!"
	elif (errorCode >WIN_LAST_SUCCESS):
		colorCode = 33
		symbol = "WARNING!"
	else:
		colorCode = 36
		symbol = "INFO"
	
	print(f"[\033[{colorCode}m{symbol}\033[0m] {message}", file=sys.stderr)
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
			printLine()
			print(string.center(cols))
			printLine()

		case 2:
			print("[*] " + string);
