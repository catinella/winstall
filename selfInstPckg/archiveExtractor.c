/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: archiveExtractor.c
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Description:
//	This module manages the TGZ archived data stored in the executable file tail space.
//
//	In order to manage the TGZ collected data lower level. I have used the UNIX common archive library. In the following
//	table you can read the exit code of the functions belong to the library
//
//		ARCHIVE_OK      = successo
//		ARCHIVE_WARN    = successo con warning
//		ARCHIVE_RETRY   = riprova l'operazione
//		ARCHIVE_FAILED  = operazione fallita, ma l'archivio può continuare
//		ARCHIVE_FATAL   = errore irreversibile, fermati
//		ARCHIVE_EOF     = fine archivio (solo per alcune API)
//
//	Debug messages:
//	===============
//	Because it is a library, the error/warning/debug messages are not printed, usually. But just for debug purpose, they are
//	available defining the DEBUG symbol
//
//	Debug levels:
//		0   Silence
//		1   It prints the error messages
//		2   It prints also the warning messages
//		3   It prints everything
//	
//	
//	
------------------------------------------------------------------------------------------------------------------------------*/
#include <archiveExtractor.h>
#include <archive.h>
#include <archive_entry.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <elfReading.h>

#define READ_BUFFER_SIZE (64 * 1024)

#define MYSELF "/proc/self/exe"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG > 0
#define DBGLOG(X, ...) _dbgLog(X, __LINE__, __VA_ARGS__)
#else
#define DBGLOG(...)    ;
#endif

static FILE           *fh      = NULL;
static struct archive *tgzArch = NULL;
bool                  isOpen   = false;


void _dbgLog (uint8_t dbgLev, unsigned int line, const char *fmt, ...) {
	//
	// Description:
	//	It rpints a debug messag if the current debug-level is equal or higher then the argument defined one
	//
	//
	va_list args;

	if (dbgLev <= DEBUG) {
		printf("[DEBUG(%d)]: ", line);
		if      (dbgLev == 1) printf("ERROR!:  ");
		else if (dbgLev == 2) printf("WARNING: ");
		else if (dbgLev == 3) printf("INFO:    ");
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//                                    P U B L I C   F U N C T I O N S 
//------------------------------------------------------------------------------------------------------------------------------
sexiErrorCode_t archiveExtractor_open () {
	//
	// Description:
	//	This function open the channel to use the archived data. It implements the following steps:
	//		1) It checks out the ELF binary code section size
	//		2) It opens the binary file in read-only mode
	//		3) It moves the reference to the TGZ archived data section
	//		4) It instances a new archived data manager
	//		5) It checks for the supported formats by the manager
	//
	// Returnde value:
	//	SEXIEC_SUCCESS
	//	SEXIEC_ERROR_IOOPFAILED
	//	SEXIEC_ERROR_SYSRESOURCES
	//	SEXIEC_ERROR_UNKNOWNFORMAT
	//	(error codes from elfReading_getSize)()
	//	
	unsigned int    elfSize = 0;
	sexiErrorCode_t ec = elfReading_getSize(&elfSize);
	
	// Get data section offset
	if (SEXIEC_ISERROR(ec)) {
		// ERROR!
		DBGLOG(1, "elfReading_getSize() failed\n");
		
	// Open myself
	} else if ((fh = fopen(MYSELF, "rb")) == NULL) {
		// ERROR!
		DBGLOG(1, "fopen(\"%s\") failed\n", MYSELF);
		ec = SEXIEC_ERROR_IOOPFAILED;
			
	// Seek for data section
	} else if (fseeko(fh, elfSize, SEEK_SET) != 0) {
		// ERROR!
		DBGLOG(1, "fseek() failed\n");
		ec = SEXIEC_ERROR_IOOPFAILED;

	} else {
		tgzArch = archive_read_new();

		// Checking for archive manager
		if (tgzArch == NULL) {
			// ERROR!
			DBGLOG(1, "archive_read_new() failed\n");
			ec = SEXIEC_ERROR_SYSRESOURCES;

		// Cchecking for TGZ format
		} else if (
			archive_read_support_filter_gzip(tgzArch) != ARCHIVE_OK ||
			archive_read_support_format_tar(tgzArch)  != ARCHIVE_OK

		) {
			// ERROR!
			DBGLOG(1, "Not supported data format\n");
			ec = SEXIEC_ERROR_UNKNOWNFORMAT;

		} else if (archive_read_open_FILE(tgzArch, fh) != ARCHIVE_OK) {
			// ERROR!
			DBGLOG(1, "archive_read_open_FILE() failed\n");
			ec = SEXIEC_ERROR_ARCHIVELIB;
			
		} else {
			// SUCCESS
			DBGLOG(3, "archiveExtractor_open() terminated with SUCCESS\n");
			isOpen = true;
		}
	}

	if (SEXIEC_ISERROR(ec))
		archiveExtractor_close();
		
	return(ec);
}

sexiErrorCode_t archiveExtractor_close () {
	//
	// Description:
	//	It release all used memory resources and close the channel to the executable binary file
	//
	// Returned code:
	//	SEXIEC_SUCCESS
	//	SEXIEC_WARNING_NOTHINGTODO
	//
	sexiErrorCode_t ec = SEXIEC_SUCCESS;

	if (isOpen) {
		archive_read_close(tgzArch);
		archive_read_free(tgzArch);
		fclose(fh);
		isOpen = false;
		tgzArch = NULL;
		fh = NULL;

	} else
		// WARNING
		ec = SEXIEC_WARNING_NOTHINGTODO;
		
	return(ec);
}

sexiErrorCode_t archiveExtractor_extract (const char *file, GPtrArray *extrFilesList) {
	//
	// Description:
	//	This function extract the argument defined file (or directory). If the argument is not secyfied (file == NULL),
	//	then all archived files will be extracted
	//	
	//	[!] In order to get a more tollerant the file matching, all the dot-slash relative paths are converted in 
	//	    blank version one. (./file --> file)
	//	    The convetion is performed in the two direction (function's arg and TGZ extracted file name) both
	//
	// Returned code:
	//	SEXIEC_SUCCESS
	//	SEXIEC_ERROR_FILENOTFOUND
	//	SEXIEC_ERROR_SYSRESOURCES
	//	SEXIEC_ERROR_ARCHIVELIB
	//	SEXIEC_ERROR_IOOPFAILED
	//
	sexiErrorCode_t ec = SEXIEC_SUCCESS;
	struct archive  *disk = archive_write_disk_new();
	unsigned int    filesCounter = 0;

	if (disk == NULL) {
		// ERROR
		ec = SEXIEC_ERROR_SYSRESOURCES;
		DBGLOG(1, "No enugh refourse for archive_write_disk_new() hinstance\n");
		
	// It sets how to restore data files
	} else if (archive_write_disk_set_options(
		disk, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL  | ARCHIVE_EXTRACT_FFLAGS
	) != ARCHIVE_OK) {
		// ERROR!
		ec = SEXIEC_ERROR_ARCHIVELIB;
		DBGLOG(1, "archive_write_disk_set_options() call failed\n");
		
	// It installs a standard set of user and group lookup functions.
	} else if (archive_write_disk_set_standard_lookup(disk) != ARCHIVE_OK) {
		// ERROR!
		ec = SEXIEC_ERROR_ARCHIVELIB;
		DBGLOG(1, "archive_write_disk_set_standard_lookup() call failed\n");

	} else {
		struct archive_entry *entry = NULL;
		int                  result = ARCHIVE_OK;
		char                 fileDir[PATH_MAX];
		char                 tgtFile[PATH_MAX];
		char                 curFile[PATH_MAX];

		if (file != NULL) {
			// Absolute path
			if (file[0] == '/') {
				strcpy(fileDir, (file+1));
				strcpy(tgtFile, (file+1));
			
			// Dot-slash relative path 
			} else if (file[0] == '.' && file[1] == '/') {
				strcpy(fileDir, (file+2));
				strcpy(tgtFile, (file+2));
			
			// Relative path
			} else {
				strcpy(fileDir, file);
				strcpy(tgtFile, file);
			}
			strcat(fileDir, "/");
		}
		
		while ((result = archive_read_next_header(tgzArch, &entry)) == ARCHIVE_OK) {
			const char *tPpath = archive_entry_pathname(entry);

			// Dot-slash relative path 
			if (tPpath[0] == '.' && tPpath[1] == '/')
				strcpy(curFile, (tPpath+2));
			else
				strcpy(curFile, tPpath);

			if (file == NULL || strncmp(curFile, fileDir, strlen(fileDir)) == 0 || strcmp(curFile, tgtFile) == 0) {
				// file extraction
				result = archive_write_header(disk, entry);
				filesCounter++;

				// === SUCCESS ===
				if (result == ARCHIVE_OK || result == ARCHIVE_WARN) {
					const void *data = NULL;
					size_t      size = 0;
					la_int64_t  offset = 0;

					DBGLOG(3, "%s extracting...\n", curFile);
					
					// Reading file content
					while ((result = archive_read_data_block(tgzArch, &data, &size, &offset)) == ARCHIVE_OK) {

						// Writing file content
						result = archive_write_data_block(disk, data, size, offset);

						if (result != ARCHIVE_OK && result != ARCHIVE_WARN) {
							// ERROR!
							ec = SEXIEC_ERROR_ARCHIVELIB;
							DBGLOG(1, "Unable to extract '%s': %s", curFile, archive_error_string(disk));
							break;
						}
					}

					// Checking for operation's exit-code
					ec = archive_write_finish_entry(disk);
					if (ec != ARCHIVE_OK && ec != ARCHIVE_WARN) {
						// ERROR!
						ec = SEXIEC_ERROR_ARCHIVELIB;
						DBGLOG(1, "Unable to complete '%s': %s", curFile, archive_error_string(disk));
					
					} else if (extrFilesList != NULL) {
						// Adding file to the list of the installed ones
						printf("+ /%s\n", curFile);
						g_ptr_array_add(extrFilesList, g_strdup(curFile));
					}
    
				} else if (result != ARCHIVE_EOF) {
					// ERROR!
					ec = SEXIEC_ERROR_IOOPFAILED;
					DBGLOG(
						1, "archive_write_header() failed for '%s': ret-code=%d; errno=%d; error='%s'\n", curFile, result,
						archive_errno(disk), archive_error_string(disk)
					);
				}
			}
		} // === WHILE LOOP ===

		printf("\n");
		if (result == ARCHIVE_OK || result == ARCHIVE_EOF) {
			DBGLOG(3, "%d file matched\n", filesCounter);
		} else {
			DBGLOG(1, "archive_read_next_header() failed: ret-code=%d; errno=%d\n", result, archive_errno(tgzArch));
		}
	}

	if (SEXIEC_ISERROR(ec))
		archiveExtractor_close();

	else if (filesCounter == 0)
		// ERROR!
		ec = SEXIEC_ERROR_FILENOTFOUND;
	
	return(ec);
}

