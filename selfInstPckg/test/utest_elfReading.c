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
#include <stdbool.h>
#include <stdio.h>

#define TEXTFILE "/etc/passwd"
#define MYPROC   "/proc/self/exe"

TEST (get_elf_size, with_text)
{
	int   size = get_elf_size();
	FILE  *fhS = NULL;
	FILE  *fhT = NULL;
	bool  err  = false;

	ASSERT_TRUE (size > 0);

	if ((fhT = fopen(MYPROC, "r")) == NULL) {
		// ERROR
		fprintf(stderr, "ERROR! I cannot open the \"%s\" file\n", MYPROC);

	} else if (fseeko(fhT, size, SEEK_SET) < 0) {
		// ERROR
		fprintf(stderr, "ERROR! fseeco() failed\n");

	} else if ((fhS = fopen(TEXTFILE, "r")) == NULL) {
		// ERROR
		fprintf(stderr, "ERROR! I cannot open the \"%s\" file\n", TEXTFILE);
	
	} else {
		while (err == false) {
			// TODO: read a line from MYPROC
			// TODO: read a line from TEXTFILE
			// TODO: data comparing...
		}
		ASSERT_TRUE (err == false);
	}
}

#include "./utest_elfReading__main.sgc"
