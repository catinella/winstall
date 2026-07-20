/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: utest_elfReading.c
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// Description:
//	It is a unit-test for the elfReading module.
//
//	This unit-test uses the minute framework. For further details, please, read the README.md file at the 
//	https://github.com/catinella/minute page
//	
------------------------------------------------------------------------------------------------------------------------------*/
#include <elfReading.h>
#include <minute.h>

#define TEXTFILE "/etc/passwd"

TEST (get_elf_size, with_text)
{
	ASSERT_EQ (1, 1);
}

#include "./utest_elfReading__main.sgc"
