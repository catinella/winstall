#!/usr/bin/python3
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: errorCodes.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	All the error-codes, used by all other modules, are defined in this module
#	
#	
#-------------------------------------------------------------------------------------------------------------------------------

WIN_LAST_SUCCESS = 31
WIN_LAST_WARNING = 127
WIN_LAST_ERROR   = 255

def is_anError (errcode):
	out = False
	if ((errcode > WIN_LAST_WARNING and errcode <= WIN_LAST_ERROR) or errcode == 0):
		out = True
	return(out)
	
def is_aWarning (errcode):
	out = False
	if (errcode <= WIN_LAST_WARNING and errcode > WIN_LAST_SUCCESS):
		out = True
	return(out)
		
def is_aSuccess (errcode):
	out = False
	if (errcode > 0 and errcode <= WIN_LAST_SUCCESS):
		out = True
	return(out)


WIN_SUCCESS             = 1

WIN_WARNING_GENERIC     = 32

WIN_ERROR_GENERIC       = 0
WIN_ERROR_MISSINGDATA   = 129
WIN_ERROR_FILENOTFOUND  = 131
WIN_ERROR_FORBIDDENDATA = 133
