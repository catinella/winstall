#!/usr/bin/python3
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: winstall.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	It is a generic installer developed in Python and it has been written to replace the typical INSTALL.sh script.
#	winstall's features:
#		- Software installation
#		- Software removing
#		- Self-installing package creation (NOT YET IMPLEMENTED)
#
#	To use the winstall.py software, you must run it inside the root folder of your project, and respect the following syntax:
#
#		<path>/winstall.py --cmd={install|uninstall|build|clean|pkg} \
#			[--verbose]                   \
#			[--tmpFolder=<dir>]           \
#			[--dataLogFolder=<dir>]       \
#			[--prefix=<dir>]              \
#			[--prjName=<string>]          \
#			[--version={auto|<n.n.n>}]    \
#			[--preInstScript=<exec-file>  \
#			[--postInstScript=<exec-file> 
#
#	
#-------------------------------------------------------------------------------------------------------------------------------
import os
import sys
import yaml
import configargparse
import shutil

from subModules.itemToInstall import itemToInstall
from pathlib                  import Path

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


def checkForForbiddenData (yfile, forbKeys):
	#
	# Description:
	#	This function checks for the forbidden-keys in the YAML file. If at least one keyword is found then the procedure
	#	ends with error code
	err = 1
	try:
		fh = open(yfile, 'r')
		config = yaml.safe_load(fh)
		fh.close()

	except FileNotFoundError:
		print(f"ERROR! {yfile} configuration file not found")
		err = 0

	if (err == 1 and config != None):
		for item in forbKeys:
			if (item in config):
				print(f"ERROR! You cannot define \"{item}\" keyword in the configuration file")
				err = 0
				break
	
	return(err)


def checkAndCreateDir (folders, verbose, createFlag = True):
	#
	# Description:
	#	It accepts a folders list as argument and checks for them: if some folder does not exist then it try to create it
	#	
	# Arguments:
	#	folders=<list>         List of the folders you want to check
	#	verbose={True|False}   Verbose messages enabling
	#	[createFlag= False]    When is true, the function try to create the not found directories
	#	
	ec = 1
	if (verbose == True):
		printTitle("Checking for the required directories", 2):
		
	for item in folders:
		path = Path(item)
		if (not path.is_dir()):
			if (createFlag):
				try:
					os.mkdir(item)
					if (verbose == True):
						print(f"\"{item}\" folder has been created")
	
				except PermissionError:
					print(f"ERROR: I am nor allowed to create the {item} folder")
					ec = 0
	
				except OSError as e:
					print(f"Errore {e.errno}: {e.strerror}")
					ec = 0
			else:
				ec = 0
				
		elif (verbose):
			print(f"\"{item}\" folder found")
			
	return(ec)

#-------------------------------------------------------------------------------------------------------------------------------
#                                                      M A I N 
#-------------------------------------------------------------------------------------------------------------------------------
_DEBUG         = True
myPwd          = os.path.dirname(os.path.abspath(sys.argv[0]))
callerPWD      = os.getcwd()
CONFFILE       = callerPWD + "/winstall_conf.yaml"
DEPS_LIST_FILE = "depsListFile"
err            = 0
forbDataInCF   = {"cmd", "verbose"}   # The keywords cannot be defined in the configuration file
parser         = configargparse.ArgParser(default_config_files=[CONFFILE])

if (checkForForbiddenData(CONFFILE, forbDataInCF) != 1):
	sys.exit(127)

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


args = parser.parse_args()

if (args.prjName == "" and (args.cmd == "install" or args.cmd == "uninstall")):
	print("ERROR! undefined project name")


else:
	PREFIX = args.prefix
	
	# Main title
	printTitle("WINSTALL", 1)

	if (_DEBUG == True):
		if (args.verbose):
			print("VERBOSE: yes");
		print(f"CMD:            {args.cmd}")
		print(f"PRJNAME:        {args.prjName}")
		print(f"PREFIX:         {args.prefix}")
		print(f"TMPFOLDER:      {args.tmpFolder}")
		print(f"DATALOGFOLDER:  {args.dataLogFolder}")
		print(f"VERSION:        {args.version}")
		print(f"PREINSTSCRIPT:  {args.preInstScript}")
		print(f"POSTINSTSCRIPT: {args.postInstScript}")

	#
	# Initialization
	#
	if (args.cmd = "install"):
		if (checkAndCreateDir({args.prefix, args.tmpFolder, args.dataLogFolder}, args.verbose) != 1):
			sys.exit(129)
			
	elif (args.cmd == "pkg"):
		if (checkAndCreateDir({args.prefix, args.tmpFolder, args.dataLogFolder}, args.verbose) != 1):
			sys.exit(131)
		else:
			PREFIX = args.tmpFolder + "/" + args.prefix
			if (checkAndCreateDir({PREFIX}, args.verbose) != 1):
				sys.exit(131)

	elif (args.cmd == "uninstall"):
		if (checkAndCreateDir({args.dataLogFolder}, args.verbose, False) != 1):
			sys.exit(133)
		
		
	#
	# Action performing
	#		
	if (args.cmd == "uninstall"):
		logFile = "args.dataLogFolder" + "/" + args.prjName"
		if (not logFile.is_file()):	
			print("WARNING! the \"{args.prjName}\" software looks like a not installed one")
		
		else:
			try:
				fh = open(logFile , 'r')
			
			except PermissionError:
				print(f"ERROR: I am nor allowed to read the {logFile} file")
				sys.exit(135)
	
			except OSError as e:
				print(f"ERROR! I cannot open the {logFile} file (errno={e.errno}): {e.strerror}")
				sys.exit(137)
			
			ec = 1
			while (not fh.eof()):
				file = Path(fh.readlines())
				if (not file.is_dir()):
					try:
						os.remove(file)
						print("\"{file}\" removed")
						
					except PermissionError:
						print(f"ERROR: I am nor allowed to remove the {file} file")
						ec = 0
						
					except FileNotFoundError:
						print(f"WARNING! The \"{file}\" file does not exist")
						ec = 0
						
					except OSError as e:
						print(f"ERROR! I cannot remove the {file} (errno={e.errno}): {e.strerror}")
						ec = 0
			fh.close()
					
			if (ec == 1):		
				try:
					os.remove(logFile)
					
				except PermissionError:
					print(f"ERROR: I am nor allowed to remove the {logFile} file")
					sys.exit(139)

				except OSError as e:
					print(f"ERROR! I cannot remove the {file} (errno={e.errno}): {e.strerror}")
					sys.exit(141)
					
	if (args.cmd == "uninstall"):
					
#with open('config.yaml', 'r') as f:
#    config = yaml.safe_load(f)
#
#for item in config:
#	try:
#		obj = itemToInstall(item)
#
#	except ValueError as errstr:
#		print(errstr)
#
#	else:
#		obj.preInstallChecks()
#		obj.postInstallActions()
#		obj.install()

