#!/usr/bin/python3
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: test_checkForOS.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	
#-------------------------------------------------------------------------------------------------------------------------------
import sys

sys.path.insert(0, "../")

from checkForOS import checkForOS

linuxFooData = {
	"family": "posix",
	"osID":   "linux"
}

winFooData = {
	"family": "nt",
	"osID":   "win32"
}

linuxReq = checkForOS(linuxFooData)
winReq   = checkForOS(winFooData)

if (linuxReq.do() == 1) :
	print("You are using a Linux OS")

elif (winReq.do() == 1) :
	print("I am sorry, you are using a winzozz software")
