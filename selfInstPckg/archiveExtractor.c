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
#include <elfReading.h>

#define READ_BUFFER_SIZE (64 * 1024)

#define MYSELF "/proc/self/exe"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG > 0
#define DBGLOG(LEV, ROW, ...) _dbgLog(X, ROW, __VA_ARGS__)
#else
#define DBGLOG(...)      ;
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

	va_start(args, fmt);
	if (dbgLev <= DEBUG) {
		printf("[DEBUG(%d)]: ", line);
		if      (dbgLev == 1) printf("ERROR!:  ");
		else if (dbgLev == 2) printf("WARNING: ");
		else if (dbgLev == 3) printf("INFO:    ");
		printf(fmt, args);
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
	if (SEXIEC_ISERROR(ec))
		// ERROR!
		_dbgLog (1, __LINE__, "elfReading_getSize() failed\n");
		
	// Open myself
	else if ((fh = fopen(MYSELF, "rb")) == NULL) {
		// ERROR!
		_dbgLog (1, __LINE__, "fopen(\"%s\") failed\n", MYSELF);
		ec = SEXIEC_ERROR_IOOPFAILED;
			
	// Seek for data section
	} else if (fseeko(fh, elfSize, SEEK_SET) != 0) {
		// ERROR!
		_dbgLog (1, __LINE__, "fseek() failed\n");
		ec = SEXIEC_ERROR_IOOPFAILED;

	} else {
		tgzArch = archive_read_new();

		// Checking for archive manager
		if (tgzArch == NULL) {
			// ERROR!
			_dbgLog (1, __LINE__, "archive_read_new() failed\n");
			ec = SEXIEC_ERROR_SYSRESOURCES;

		// Cchecking for TGZ format
		} else if (
			archive_read_support_filter_gzip(tgzArch) != ARCHIVE_OK ||
			archive_read_support_format_tar(tgzArch)  != ARCHIVE_OK

		) {
			// ERROR!
			_dbgLog (1, __LINE__, "Not supported data format\n");
			ec = SEXIEC_ERROR_UNKNOWNFORMAT;
			archive_read_close(tgzArch);
			archive_read_free(tgzArch);

		} else {
			// SUCCESS
			_dbgLog (3, __LINE__, "archiveExtractor_open() terminated with SUCCESS\n");
			isOpen = true;
		}
	}

	if (SEXIEC_ISERROR(ec) && fh != NULL)
		fclose(fh);
		
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

	} else
		// WARNING
		ec = SEXIEC_WARNING_NOTHINGTODO;
		
	return(ec);
}

sexiErrorCode_t archiveExtractor_extract (const char *file) {
	//
	// Description:
	//	This function extract the argument defined file (or directory). If the argument is not secyfied (file == NULL),
	//	then all archived files will be extracted
	//
	sexiErrorCode_t ec = SEXIEC_SUCCESS;

	return(ec);
}

