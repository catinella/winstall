
#-------------------------------------------------------------------------------------------------------------------------------
#                                             __        __   ___           _        _ _ 
#                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
#                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
#                                               \ V  V /_____| || | | \__ \ || (_| | | |
#                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
#                                                                                       
#	
# Filename: itemToInstall.py
#
# Author: Silvano Catinella <catinella@yahoo.com>
#	
# Description:
#	
#	
#-------------------------------------------------------------------------------------------------------------------------------

class itemToInstall:
	def __init__(self, yText):
		
		# check for default fields
		if ("source" in yText) and ("target" in yText):
			self.yaml = yText
		else:
			raise ValueError("Wrong item definition")
		
	def preInstallChecks(self):
		if ("beforeCopy" in self.yaml):
			for submod in self.yaml["beforeCopy"]:
				print(f"Check: {submod}")

	def postInstallActions(self):
		if ("afterCopy" in self.yaml):
			for submod in self.yaml["beforeCopy"]:
				print(f"Action: {submod}")

	def install(self):
		print("copy " + self.yaml["source"] + " to " + self.yaml["target"])

