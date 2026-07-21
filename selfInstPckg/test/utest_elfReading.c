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

#ifndef TEXTFILE
#error "\"TEXTFILE\" is undefined symbol"
#endif

#define MYPROC     "/proc/self/exe"
#define MAXLINELEN 1024

TEST (get_elf_size, with_text) {
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
		char         lineFileA[MAXLINELEN];
		char         lineFileB[MAXLINELEN];
		unsigned int counter = 0;

		memset(lineFileA, '\0', sizeof(char) * MAXLINELEN);
		memset(lineFileB, '\0', sizeof(char) * MAXLINELEN);

		while (err == false && feof(fhS) == 0 && feof(fhT) == 0) {
			if (fgets(lineFileA, MAXLINELEN, fhS) != NULL && fgets(lineFileB, MAXLINELEN, fhT) != NULL) {
				if (strcmp(lineFileA, lineFileB) != 0) {
					// ERROR! 
					fprintf(stderr, "ERROR! Text data corrupted");
					err = true;

				} else if (fileArgumentsDb_get("verbose", NULL) == true) {
					counter++;
					printf("\rPassed: %d", counter);
				}
			}
		}
		if (fileArgumentsDb_get("verbose", NULL) == true)
			printf("\n");

		ASSERT_TRUE (err == false);
	}
}

#include "./utest_elfReading__main.sgc"
