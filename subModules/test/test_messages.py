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
#	Tests for messages library
#	
#-------------------------------------------------------------------------------------------------------------------------------

import sys

sys.path.insert(0, "../")

from messages   import *
from errorCodes import *

print("Error message will be printed:")
sysMessage ("Everything is broken", (WIN_LAST_WARNING + 1), exitFlag = False)
print("")

print("Warning message will be printed:")
sysMessage ("Be carefull, it is a dangerouse action", (WIN_LAST_SUCCESS + 1), exitFlag = False)
print("")

print("Info message will be printed:")
sysMessage ("Ok but consider this", 1, exitFlag = False)
print("")

print("Title H1")
printTitle("Main title bla bla...", 1)
print("")

print("Title H2")
printTitle("Sub-title bla bla...", 2)
print("")
