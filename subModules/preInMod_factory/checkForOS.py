#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: checkForOS.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	This class allows you to get an object ables to check for the required OS
#
#	[!] The string comparison is not case sensitive
#
#	The checkForOS class' constructor accepts a dictionary (or hash) with the following fields
#		family: {posix|nt}
#		osID:   {linux|solaris|...|win32} # optional
#	
#-------------------------------------------------------------------------------------------------------------------------------
import os
import sys

class checkForOS :
	
	def __init__ (self, arg) :
		self.myconf = arg;

	def do (self) :
		out = 1
		if (self.myconf["family"].upper() != os.name.upper()) :
			out = 0

		elif ("osID" in self.myconf and self.myconf["osID"].upper() != sys.platform.upper()) :
			out = 0

		return(out)
