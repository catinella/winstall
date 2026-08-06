/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: utest_archiveExtractor.c
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
#include <archiveExtractor.h>
#include <minute.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

void _chomp (char *string_a) {
	unsigned int x = (strlen(string_a) - 1);
	while (x > 0 && string_a[x] == '\n') {
		string_a[x] = '\0';
		x--;
	}
}

TEST (archiveExtractor, openAndClose) {
	sexiErrorCode_t ec = SEXIEC_SUCCESS;
	
	// Warning: you are closing the archive but it was not open 
	ec = archiveExtractor_close();
	ASSERT_TRUE (SEXIEC_ISWARNING(ec));

	// TGZ archive opening...
	ec = archiveExtractor_open();
	ASSERT_TRUE (SEXIEC_ISERROR(ec) == false);

	// TGZ archive closing...
	ec = archiveExtractor_close();
	ASSERT_TRUE (SEXIEC_ISSUCCESS(ec));
}

TEST (archiveExtractor, fooDataExtraction) {
	struct stat     statbuf;
	sexiErrorCode_t ec = SEXIEC_SUCCESS;
	int             x = 0;

	// TGZ archive opening...
	ec = archiveExtractor_open();
	ASSERT_TRUE (SEXIEC_ISERROR(ec) == false);

	// File extraction
	ec = archiveExtractor_extract("fooData/d2", NULL);
	ASSERT_TRUE (SEXIEC_ISERROR(ec) == false);

	// checking for the extracted file
	x = stat("fooData/d2/due", &statbuf);
      ASSERT_EQ(x , 0);

	if (x == 0) {
		char buffer[1024];
		FILE *fh = fopen("fooData/d2/due", "r");
		memset(buffer, '\0', sizeof(buffer));

		if (fh) {
			if (fgets(buffer, sizeof(buffer), fh) != NULL) {
				_chomp(buffer);
				ASSERT_EQ(strcmp("22222222", buffer), 0);
			} else {
				// ERROR!
				fprintf(stderr, "TEST SKIPPED\n");
				fprintf(stderr, "ERROR! impossible to read \"%s\" file content\n", "fooData/d2/due");
			}
			fclose(fh);
		}
	} else {
		// ERROR!
		fprintf(stderr, "TEST SKIPPED\n");
		fprintf(stderr, "ERROR! I cannot open the \"%s\"\n", "fooData/d2/due");
	}

	// TGZ archive closing...
	ec = archiveExtractor_close();
	ASSERT_TRUE (SEXIEC_ISSUCCESS(ec));
}

#include "./utest_archiveExtractor__main.sgc"
