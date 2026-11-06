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
#	This module provides all functions used to manage the configuration data
#	
#	
#-------------------------------------------------------------------------------------------------------------------------------

from .errorCodes import *

class ConfigError(Exception):
	def __init__(self, code, message):
		super().__init__(message)
		self.code = code
        
def _checkForForbiddenData (yfile, forbKeys):
	#
	# Description:
	#	This function checks for the forbidden-keys in the YAML file. If at least one keyword is found then the procedure
	#	ends with the proper error code
	#
	# Exceptions:
	#	WIN_ERROR_FILENOTFOUND
	#	WIN_ERROR_FORBIDDENDATA
	#
	ec = ConfigError(WIN_SUCCESS)
	
	try:
		fh = open(yfile, 'r')
		config = yaml.safe_load(fh)
		fh.close()

	except FileNotFoundError:
		# ERROR!
		ec.message = f"{yfile} configuration file not found"
		ec.code    = WIN_ERROR_FILENOTFOUND

	if (is_anError(ec.code) == False and config != None):
		for item in forbKeys:
			if (item in config):
				ec.message = f"You cannot define \"{item}\" keyword in the configuration file"
				ec.code    = WIN_ERROR_FORBIDDENDATA
				break
	
	return(ec.code)


def parseConfig(confile, forbiddenInCF):
	#
	# Description:
	#
	#
	ec = ConfigError(WIN_SUCCESS)
	parser = configargparse.ArgParser(default_config_files=[confile])

	if (is_anError(_checkForForbiddenData(confile, forbiddenInCF))):
		# ERROR!
		iec.message = "Missing file's arguments"
		ec.code = WIN_ERROR_MISSINGDATA

	else:
		parser.add_argument(
			"--cmd", 
			required           = True, 
			choices            = ["install", "uninstall", "build", "clean", "pkg"]
		)
		parser.add_argument(
			"--verbose", 
			action             = "store_true",
			default            = False,
			help               = "Enable verbose output"
		)
		parser.add_argument(
			"--prefix",
			default            = "/usr/local",
			help               = "Installation prefix"
		)
		parser.add_argument(
			"--tmpFolder",
			default            = "/tmp/winstall",
			help               = "Temporary folder"
		)
		parser.add_argument(
			"--dataLogFolder",
			default            = "/var/local/winstall",
			help               = "System folder for installed logs"
		)
		parser.add_argument(
			"--prjName",
			default            = "",
			help               = "Project name"
		)
		parser.add_argument(
			"--version",
			default            = "unknown",
			help               = 'Project version (n.n.n) or "auto"'
		)
		parser.add_argument(
			"--preInstScript",
			default            = "",
			help               = 'Pre files installation script'
		)
		parser.add_argument(
			"--postInstScript",
			default            = "",
			help               = 'Post files installation script'
		)

		parser.add_argument(
			"--depsList",
			default            = "",
			help               = "List of dependences"
		)


		args = parser.parse_args()

		if ()is_aSuccess(ec.code) == false):
			raise(ec)
		else:	
			return(args)
