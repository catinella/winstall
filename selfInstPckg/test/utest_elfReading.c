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
#include <errno.h>
#include <unistd.h>

#ifndef TEXTFILE
#error "\"TEXTFILE\" is undefined symbol"
#endif

#define MYPROC     "/proc/self/exe"
#define MAXLINELEN 1024

TEST (get_elf_size, with_text) {
	unsigned int size = get_elf_size();
	FILE         *fhS = NULL, *fhT = NULL;

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
		char         *rcA = NULL;
		char         *rcB = NULL;
		unsigned int counter = 0;
		bool         err  = false;

		memset(lineFileA, '\0', sizeof(char) * MAXLINELEN);
		memset(lineFileB, '\0', sizeof(char) * MAXLINELEN);

		while (err == false && feof(fhS) == 0 && feof(fhT) == 0) {
			rcA = fgets(lineFileA, MAXLINELEN, fhS);
			rcB = fgets(lineFileB, MAXLINELEN, fhT);
			if (rcA != NULL && rcB != NULL) {
				if (strcmp(lineFileA, lineFileB) != 0) {
					// ERROR! 
					fprintf(stderr, "ERROR(%d)! Text data corrupted\n", __LINE__);
					//printf("DEBUG(%d): \"%s\"\n", __LINE__, lineFileA);
					//printf("DEBUG(%d): \"%s\"\n", __LINE__, lineFileB);
					err = true;

				} else if (fileArgumentsDb_get("verbose", NULL)) {
					counter++;
					fprintf(stdout, "\rPassed: %d ", counter);
					fflush(stdout);
					usleep(10000);
				}

			} else if (errno == 0 && feof(fhS) && feof(fhT)) {
				//printf("DEBUG(%d): Normal end\n", __LINE__);

			} else {
				// ERROR! 
				fprintf(stderr, "ERROR(%d)! %s", __LINE__, strerror(errno));
				err = true;
			}
		}
		if (fileArgumentsDb_get("verbose", NULL))
			printf("\n");

		ASSERT_TRUE (err == false);
	}
}

#include "./utest_elfReading__main.sgc"
