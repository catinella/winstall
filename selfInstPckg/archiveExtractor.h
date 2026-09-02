/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: archiveExtractor.h
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// SPDX-License-Identifier: LGPL-3.0-or-later
//	
// Description:
//	This module manages the TGZ archived data stored in the executable file tail space.
//	
//		+-----------------+
//		|   Binary code   | The packqage's brain
//		+-----------------+
//		|                 |
//		| tar.gz payload  | TGZ archive that contains the files to install
//		|                 |
//		+-----------------+
//	
//	
------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __ARCHIVEEXTRACTOR__
#define __ARCHIVEEXTRACTOR__

#include <sexiErrorCodes.h>
#include <glib.h>

sexiErrorCode_t archiveExtractor_open    ();
sexiErrorCode_t archiveExtractor_close   ();
sexiErrorCode_t archiveExtractor_extract (const char *file, GPtrArray *extrFilesList);

#endif
