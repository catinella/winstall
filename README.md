![Project's banner](images/winstall-1.x.x.png)

# WInstall - One installer for every project.

## 1.0 Files

|   Files/Dirs  |                     Description                           |
|---------------|-----------------------------------------------------------|
| images        | This folder contains picture used by the RADME.md files   |
| LICENSE       | GPL 3 licence                                             |
| selfInstPckg  | The C code to build a self-installer software package     |
| winstall.sh   | The bash-installer script                                 |

## 2.0 Decription

I have created **winstall** project because I wase sick to write the usual INSTALL.sh for every project. So, I wrote this software
with the following features:

1) Information acquiring from local config files easy to maintain.
2) Software installation (file copying) in defined prefix path
3) Pre-install and post-install scripts management
4) Software removing
5) Self installing package creation

This version (1.x.x) of Winstall is composed by two components: the main one is the **winstall.sh** BASH script (that manages all
the process); the second is the self-installer package creator one. In order to use winstall in your source code, I suggest you to
store it (as submodule for example) inside your project, and execute winstall.sh script with the following syntax:

	<path>/winstall.sh --cmd={install|uninstall|build|clean|pkg} \
		[--verbose] \
		[--tmpFolder=<dir>] \
		[--dataLogFolder=<dir>] \
		[--prefix=<dir>] \
		[--prjName=<string>]

### 2.1 How to write the main configuration file
As you can see in the previouse section, you can drive the file installation (or the package creation) using the bash-script
arguments. But you can also use the winstall's configuration file. Anyway, remember that the file's arguments override the
data defined in the configuration file. The configuration file must respect the following syntax:

	PRJNAME=<string>
	PREINST=<exec-file>
	POSTINST=<exec-file>
	PREFIX=<folder>
	DATALOGFOLDER=<folder>
	TMPFOLDER=<folder>

- **PRJNAME**:       Name of the software. It is used for the filename used to remove the installed files and for the software
                     packege file-name
- **PREINST**:       Pre-installing step script. The script to execute before the file installation. It it fails then no files
                     will be installed
- **POSTINST**:      Post-installing step script. This script will be executed after the file copying step. If it fails then
                     a warning message will be shown on the terminal
- **PREFIX**:        Installed files path prefix. Every installed files will be stored in a sub-folder of this defined prefix
                     (eg. for PREFIF=/usr/local, /bin/foo.bin --> /usr/local/bin/foo.bin)
- **DATALOGFOLDER**: This is the folder where the process will store all information to remove every software installed by
                     winstall
- **TMPFOLDER**:     A temporary folder used by winstall process to store the collected files

### 2.2 How to write a sub-folder's configuration file
If you are installing your own software, or creating a software package, then the first step is collecting the files you want
install on the target system. In order to complete this task uning winstall tool, you have to createb (one or more)
configuration file in every folder where the files are stored. Every winstall-configuration file must respect the following
syntax:

	BUILDER=<exec-file|command>
	FILES=<files>
	TGTPATH=<folder>
	CHMOD=<n><n><n>
	CHOWN=<user-name>
	CLEANER=<exec-file|command>

- **BUILDER**:   The executable file or command that to execute before to collect the files. Usually, it is used to make
                 binary files (eg. make all9) or to use template files (eg. m4 file.template > file.dat)
- **FILES**:     The files to be installed. You can also use wildchars (eg. myProject*.exe)
- **TGTPATH**:   The folder where the defined files will be stored
- **CHMOD**:     The files permissions
- **CHOWN**:     The files owner
- **CLEANER**:   The executable file or command that will clean the folder by dinamically created files (eg. make cleanall)

Everyone of these config files manages the installation of a particolar type of files (eg. binary, manpage, template...).
For this reason you can have more then one config-file in every folder, it depends by the files groups you want to handle.
More config files can exists in the same folder because their file-name must respect the following syntax:
**winstall_**<label>**.conf**

### 2.3 How to install a software
After you have created all the winstall's configuration-files, you an preoceed with your custom software installation, using
the following command:

	<path>/winstall.sh --cmd=install [--verbose] [--prefix=<dir>] [--prjName=<string>] [--dataLogFolder=<dir>] 

The process will perform the following steps:

1) If a pre-install script (**PREINST**) has been defined, then it will be executed. If t will fail the installation process
   will end.
2) The process will scan the current folder (and its sub-folders recoursively) to find all the winstall_<label>.conf files.
   Using the inforantion iw will install every file in the right place with the defined attributes. The target folder of
   every file depends also by the **PREFIX** configured path. Using "--prefix=<dir>" argument you will overwrite if.
3) If a post-install script (**POSTINST**) has been defined, then it will be executed. If t will fail the installation process
   will just print an error message
4) The process will store a file with the list of all installed files, in the folder defined by **DATALOGFOLDER** config-key.
   The name of this file will be iequal to the (**PRJNAME**) defined project-name 

### 2.4 How to remove a software
Using winstall you can install ad remove all software you need keeping the target system clean and safe. To achieve this result
you need to remove the software using the following syntax:

	<path>/winstall.sh --cmd=uninstall [--verbose] [--dataLogFolder=<dir>] [--prjName=<string>]

The process will perform the following steps:

1) The process will look for the installed files log file (**PRJNAME**) in the proper folder (**DATALOGFOLDER**). If it does
   exists then the process will ends with the warning message "The software is not installed".
2) The log-file will be loaded, and the process will remove all the listed files.
3) The log-file will be removed

### 2.5 What is and how to create a self-installer package
This software tool provides you the passibility to get a self-installing binary packege, too. Using this package you will be
able to install and remove the software in the target system without the request to have a package manager installed in that
system. It can be very usefull in environments where you don't want to have a package manage algready installed in your target
(like in some embedded software devices) or when you need to install the software in many different POSIX OSs or Linux distros..

The following diagram shows you how the self-installer package is buit insternally:

	+----------------+
	| The installer  |
	|  binary code   |
	+----------------+
	|////////////////|
	|////////////////|
	|                |
	| TAR-GZ archive |
	|                |
	|////////////////|
	|////////////////|
	+----------------+

To create your own self-installer  binary package, use the following command:

	<path>/winstall.sh --cmd=pkg [--verbose] [--tmpFolder=<dir>] [--dataLogFolder=<dir>] [--prefix=<dir>] [--prjName=<string>]

The process will perform the following steps:

1) In the temporary (**TMPFOLDER**), the process will collect all the files to be install with the proper path (**PREFIX**). Also
   the pre-install script (**PREINST**) and post-install scrpt (**POSTINST**) will be copyied in the temporary folder.
   All these files are used to create a TGZ archive.
2) The installer C source-code will be compiled
3) The package will be created glueing the binary code with the TGZ archive to obtain a single self installer package.

The executable bin package accept the following arguments: --install, --uninstall, --help

## 3.0 TODO
![TODO](TODO)

## 4.0 Changes
![CHANGES](Changes)

## 5.0 Licence
This project is a free software; you can redistribute it and/or modify it under the terms	of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3.0 of the License, or (at your option) any later version. 

For further details please read the full [LGPL text file](https://www.gnu.org/licenses/lgpl-3.0.txt).
You should find a copy of the GNU General Public License document in the root folder of the project; if not, write to the 

	Free Software Foundation, Inc.,
	59 Temple Place, Suite 330,
	Boston, MA  02111-1307  USA
