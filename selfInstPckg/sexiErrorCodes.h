/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: sexiErrorCodes.h
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// Description:
//	Self EXtractor & Installer ErrorCodes
//	This file should just reports all success/warning/error codes used inside the project6
//
//
//	Exit code convention:
//		0..15      Success / informational
//		16..127    Non-fatal conditions (warnings)
//		128..255   Fatal errors 
//
------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __SEXIERRORCODES__
#define __SEXIERRORCODES__

#define SEXIEC_LASTSUCCESS 15
#define SEXIEC_LASTWARNING 127

typedef enum {
	SEXIEC_SUCCESS             = 1,

	SEXIEC_WARNING_GENERIC     = 19,
	SEXIEC_WARNING_PKGNOTFOUND = 21,
	SEXIEC_WARNING_RMOPFAILED  = 23,
	SEXIEC_WARNING_PKGINSTD    = 25,

	SEXIEC_ERROR_ILLEGALARG    = 129,
	SEXIEC_ERROR_ILLEGALCONF   = 131,
	SEXIEC_ERROR_FILENOTFOUND  = 133,
	SEXIEC_ERROR_IOOPFAILED    = 135,
	SEXIEC_ERROR_SYSRESOURCES  = 137,
	SEXIEC_ERROR_SYSCALLFAILED = 139,
	SEXIEC_ERROR_UNKNOWNFORMAT = 141,
	SEXIEC_ERROR_CORRUPTEDDATA = 143,
	SEXIEC_ERROR_ARCHIVELIB    = 145,
	SEXIEC_ERROR_MALFORMEDPKG  = 147,
	SEXIEC_ERROR_EXTEXEFAILED  = 149,

} sexiErrorCode_t;

#define SEXIEC_ISERROR(X)   (X > SEXIEC_LASTWARNING)
#define SEXIEC_ISWARNING(X) (X <= SEXIEC_LASTWARNING && X > SEXIEC_LASTSUCCESS)
#define SEXIEC_ISSUCCESS(X) (X <= SEXIEC_LASTSUCCESS)

#endif
