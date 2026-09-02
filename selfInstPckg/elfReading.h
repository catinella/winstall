/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: elfReading.h
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// SPDX-License-Identifier: LGPL-3.0-or-later
//	
// Description:
//	This library provides functionality to read the currently-running executable file ELF header.
//
------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __ELFREADING__
#define __ELFREADING__

#include <sexiErrorCodes.h>
#include <stdio.h>

sexiErrorCode_t elfReading_getSize (unsigned int *binSize);

#endif
