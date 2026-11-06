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

from subModules.messages      import *
from subModules.itemToInstall import itemToInstall
from pathlib                  import Path


FOLDER_CONF_YAML = "winstall_appFolder.yaml"
MAIN_CONF_YAML   = "winstall_conf.yaml"
DEPS_LIST_FILE   = "depsListFile"

#-------------------------------------------------------------------------------------------------------------------------------
#                                                       F U N C T I O N S
#-------------------------------------------------------------------------------------------------------------------------------


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
		printTitle("Checking for the required directories", 2)
		
	for item in folders:
		path = Path(item)
		if (not path.is_dir()):
			if (createFlag):
				try:
					os.mkdir(item)
					if (verbose == True):
						print(f"\"{item}\" folder has been created")
	
				except PermissionError:
					sysMessage(f"I am nor allowed to create the {item} folder", 128, False)
					ec = 0
	
				except OSError as e:
					sysMessage(f"{e.errno}: {e.strerror}", 128, False)
					ec = 0
			else:
				ec = 0
				
		elif (verbose):
			print(f"\"{item}\" folder found")
			
	return(ec)

def getRightTarget (yamlDiz):
	#
	# Description:
	#	It returns the right target to use for the current OS
	#
	myTgt = ""
	
	# TODO: target section acknowledge
	# TODO: Loop until found != true
	# TODO: Consider the os-id field

	return(myTgt)
	
def fileGlue (patha, pathb):
	#
	# Description:
	#	It glues the two argument defined paths considering the current OS
	#
	gpath = ""
	
	return(gpath)
	
#-------------------------------------------------------------------------------------------------------------------------------
#                                                      M A I N 
#-------------------------------------------------------------------------------------------------------------------------------
_DEBUG         = True
myPwd          = os.path.dirname(os.path.abspath(sys.argv[0]))
callerPWD      = os.getcwd()
CONFFILE       = callerPWD + "/" + MAIN_CONF_YAML
err            = 0
forbDataInCF   = {"cmd", "verbose"}   # The keywords cannot be defined in the configuration file

if (args.prjName == "" and (args.cmd == "install" or args.cmd == "uninstall")):
	sysMessage("undefined project name", 129)


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
	if (args.cmd == "install"):
		if (checkAndCreateDir({args.prefix, args.tmpFolder, args.dataLogFolder}, args.verbose) != 1):
			sys.exit(131)
			
	elif (args.cmd == "pkg"):
		if (checkAndCreateDir({args.prefix, args.tmpFolder, args.dataLogFolder}, args.verbose) != 1):
			sys.exit(131)
		else:
			PREFIX = args.tmpFolder + "/" + args.prefix
			if (checkAndCreateDir({PREFIX}, args.verbose) != 1):
				sys.exit(131)

	elif (args.cmd == "uninstall"):
		if (checkAndCreateDir({args.dataLogFolder}, args.verbose, False) != 1):
			sys.exit(131)
		
		
	#
	# Action: uninstall
	#		
	if (args.cmd == "uninstall"):
		logFile = Path(args.dataLogFolder + "/" + args.prjName)
		if (not logFile.is_file()):	
			print(f"WARNING! the \"{args.prjName}\" software looks like a not installed one")
		
		else:
			try:
				fh = open(logFile , 'r')
			
			except PermissionError:
				sysMessage(f"I am nor allowed to read the {logFile} file", 135)
	
			except OSError as e:
				sysMessage(f"I cannot open the {logFile} file (errno={e.errno}): {e.strerror}", 137)
			
			ec = 1
			filesList = fh.readlines();
			
			for item in filesList:
				file = Path(item.strip())
				
				if (not file.is_dir()):
					try:
						os.remove(file)
						if (args.verbose):
							print(f"\"{file}\" removed")
						
					except PermissionError:
						sysMessage(f"I am nor allowed to remove the {file} file", 128, False)
						ec = 0
						
					except FileNotFoundError:
						sysMessage(f"The \"{file}\" file does not exist", 32)
						ec = 0
						
					except OSError as e:
						sysMessage(f"I cannot remove the {file} (errno={e.errno}): {e.strerror}", 128, False)
						ec = 0
			fh.close()
					
			if (ec == 1):		
				try:
					os.remove(logFile)
					if (args.verbose):
						print(f"The \"{args.prjName}\" software has been removed")
					
				except PermissionError:
					sysMessage(f"I am not allowed to remove the {logFile} file", 139)

				except OSError as e:
					sysMessage(f"I cannot remove the {file} (errno={e.errno}): {e.strerror}", 141)
			
			#---
		#---
		
	#
	# Action: install, pkg, build, clean
	#		
	else:
		IF_RepoFile = ""
		
		# TODO: Self calculated version number

		if (args.cmd == "install"):
			# pre-installation script execution
			
			try:
				myFile = fileGlue(args.dataLogFolder, args.prjName)
				IF_RepoFile = open(myFile, 'w')

			except PermissionError:
				sysMessage(f"I am not allowed to write the {myFile} file", 143)
	
			except OSError as e:
				sysMessage(f"I cannot open the {myFile} file (errno={e.errno}): {e.strerror}", 143)
				
				
		if (args.verbose):
			printTitle("Collecting data step...", 2)

		for file in Path(callerPWD).rglob(FOLDER_CONF_YAML):
			try:
				# Sub-folder entering
				os.chdir(file.parent)
				if (args.verbose):
					printTitle(f"Entering in the \"{file.parent}\" folder", 2)

			except OSError as e:
				sysMessage(f"I cannot change directory: {e}", 143)


			try:
				# ASCII file loading
				fh = open(FOLDER_CONF_YAML, 'r')
				
			except PermissionError:
				sysMessage(f"I am nor allowed to read the {FOLDER_CONF_YAML} file", 145)
	
			except OSError as e:
				sysMessage(f"I cannot open the {FOLDER_CONF_YAML} file (errno={e.errno}): {e.strerror}", 147)

				
			try:
				#
				# YAML data loading
				#
				#	[!] This YAML section can contains many sub-sections. Every one of them is responible for a
				#	    single file or a files-group installation
				#
				appConf = yaml.safe_load(fh)
			
			except yaml.YAMLError as e:
				sysMessage(f"Wrong or unsupported YAML syntax in the \"{file}\" file: {e}", 149)
				
			except UnicodeDecodeError:
				sysMessage(f"Character decoding failed in the \"{file}\" file", 151) 
				
			except OSError as e:
				sysMessage(f"Text to YAML interpretation failed (errno={e.errno}): {e.strerror}", 153)
				
			envVars = {}
			
			for fileSection in appConf:
				# Message is an otional field
				if ("message" in fileSection):
					printTitle(fileSection["message"], 2)
						
				if (args.cmd == "build"):
					if ("build" in fileSection):
						envVars["VERSION"] = VERSION
						doit(envVars, fileSection["build"])
					elif (args.verbose):
						sysMessage("Nothing to do", 32)
						
				if (args.cmd == "clean"):
					if ("clean" in fileSection):
						doit(envVars, fileSection["clean"])
					elif (args.verbose):
						sysMessage("Nothing to do", 32)
				
				if (args.cmd == "install" or args.cmd == "pkg"):
						
					if ("target" in fileSection and "source" in fileSection):
						# beforeCopy section
						if ("beforeCopy" in fileSection):
							# TODO: bcpFatory
							pass
						
						#
						# File copying procedure
						#
						source = fileSection["source"]
						target = getRightTarget(fileSection["target"])
						if (target != ""):
							target = fileGlue(PREFIX, target)
							shutil.copy2(source, target)
						else:
							sysMessage("Undefined target folder", 157)
							
						
						# Installed files list updating...
						if (args.cmd == "install"):
							IF_RepoFile.write(f"{target}\n")
							IF_RepoFile.flush()
	
						# afterCopy section
						if ("afterCopy" in fileSection):
							# TODO: acpFatory
							pass
					else:
						sysMessage(f"Required fields (source, target) missing in {file} YAML file", 155)
					
				# --- files installation is completed ---
			# --- YAML sub-section terminated
		# --- All sources tree has been analyzed
			
		if (args.cmd == "pkg"):
			if (args.verbose):
				printTitle("pkg construction...", 2)
				
		elif (args.cmd == "install"):
			# TODO: close the software's log file
		
			if (args.postInstScript != ""):
				# TODO: Post-installation script execution
				pass
				
			if (args.verbose):
				print(f"The {args.prjName} software has been correctly installed")
				
	# --- install, pkg, build, and clean options

sys.exit(err)
