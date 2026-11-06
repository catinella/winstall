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
#	
#-------------------------------------------------------------------------------------------------------------------------------

import shutil
import sys

def sysMessage (message, errorCode, exitFlag = True):
	#
	# Description:
	#	It prints a (error, warning...) status message and stops the process if the argument defined errorCode's value
	#	defines the event as an error and the exitFlag is set to true value
	#
	if (errorCode > 127):
		symbol = "ERROR!"
	elif (errorCode > 31):
		symbol = "WARNING!"
	else:
		symbol = "!"
	
	print(f"{symbol} {message}", file=sys.stderr)
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
