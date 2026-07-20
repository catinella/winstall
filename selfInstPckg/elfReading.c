/*------------------------------------------------------------------------------------------------------------------------------
//                                             __        __   ___           _        _ _ 
//                                             \ \      / /  |_ _|_ __  ___| |_ __ _| | |
//                                              \ \ /\ / /____| || '_ \/ __| __/ _` | | |
//                                               \ V  V /_____| || | | \__ \ || (_| | | |
//                                                \_/\_/     |___|_| |_|___/\__\__,_|_|_|
//                                                                                       
//	
// Filename: elfReading.c
//
// Author: Silvano Catinella <catinella@yahoo.com>
//	
// Description:
//	This library provides functionality to read the currently-running executable file ELF header.
//
//		+---------------------+
//		| Elf64_Ehdr          | <-- Header del file
//		|---------------------|
//		| e_ident             |
//		| e_type              |
//		| e_machine           |
//		| e_version           |
//		| e_entry             |
//		| e_phoff ---------+  |
//		| e_shoff ------+  |  |
//		| e_phnum       |  |  |
//		| e_phentsize   |  |  |
//		| ...           |  |  |
//		+---------------|--|--+
//	                      |  |
//	                      |  |
//	                      |  +-----------------+
//	                      |                    |
//	                      ▼                    ▼
//		+----------------------+      +----------------------+
//		| Program Header Table |      | Section Header Table |
//		| Elf64_Phdr[0]        |      | Elf64_Shdr[0]        |
//		| Elf64_Phdr[1]        |      | Elf64_Shdr[1]        |
//		| ...                  |      | ...                  |
//		+----------------------+      +----------------------+
//
------------------------------------------------------------------------------------------------------------------------------*/
#include <elfReading.h>
#include <sys/stat.h>
#include <stdint.h>
#include <elf.h>

#define MYPROC "/proc/self/exe"

static FILE* __readProc() {
	struct stat st;
	FILE *out = NULL;
	if (stat(MYPROC, &st) == 0)
		out = fopen(MYPROC, "rb");
	return(out);
}

int get_elf_size() {
	//
	// Description:
	//	It returns the executable code size or -1 for eror
	//
	//	ELF standard format:
	//	====================
	//
	//		typedef struct {
	//			unsigned char e_ident[EI_NIDENT];  // qui dentro c'è EI_VERSION
	//			Elf64_Half    e_type;
	//			Elf64_Half    e_machine;
	//			Elf64_Word    e_version;           // seconda versione
	//			...
	//		} Elf64_Ehdr;
	//
	//		EI_NIDENT section:
	//			ident[0]  -> 0x7F
	//			ident[1]  -> 'E'
	//			ident[2]  -> 'L'
	//			ident[3]  -> 'F'
	//		 	dent[EI_CLASS]
	//			ident[EI_DATA]
	//			ident[EI_VERSION]
	//
	//
	//
          
	uint8_t ident[EI_NIDENT];
	FILE    *fh = __readProc();
	int     err = 0, size = 0;
	
	// Checking for function's arguments
	if (fh == NULL) {
		// ERROR!
		err = -2;

	// ELFMAG field reading
	} else if (fread(ident, sizeof(ident), 1, fh) != 1) {
		// ERROR!
		err = -1;

	// Rewind
	} else if (fseeko(fh, 0, SEEK_SET) != 0) {
		// ERROR!
		err = -1;

	} else {
		if (ident[EI_CLASS] == ELFCLASS64) {
			Elf64_Ehdr ehdr;

			// The whole Elf64_Ehdr structure reading
			if (fread(&ehdr, sizeof(ehdr), 1, fh) != 1) {
				// ERROR!
				err = -1;
			
			// Entry size
			} else if (ehdr.e_phentsize != sizeof(Elf64_Phdr)) {
				// ERROR!
				err = -1;

			// Number of entries
			} else if (ehdr.e_phnum == 0) {
				// ERROR!
				err = -1;

			// Moving to the PH sections
			} else if (fseeko(fh, ehdr.e_phoff, SEEK_SET) != 0) {
				// ERROR!
				err = -1;

			} else {
				off_t      elf_end = 0;
				Elf64_Phdr phdr;
				off_t      segment_end;

				for (Elf64_Half i = 0; i < ehdr.e_phnum; ++i) {

					if (fread(&phdr, sizeof(phdr), 1, fh) != 1) {
						// ERROR!
						err = -2;
						break;

					} else {
						segment_end = (off_t)phdr.p_offset + (off_t)phdr.p_filesz;
						if (segment_end > elf_end)
							elf_end = segment_end;
					}
				}
				size = (int)elf_end;
			}
			
		} else if (ident[EI_CLASS] == ELFCLASS32) {
			Elf32_Ehdr ehdr;

			// The whole Elf32_Ehdr structure reading
			if (fread(&ehdr, sizeof(ehdr), 1, fh) != 1) {
				// ERROR!
				err = -1;
			
			// Entry size
			} else if (ehdr.e_phentsize != sizeof(Elf32_Phdr)) {
				// ERROR!
				err = -1;

			// Number of entries
			} else if (ehdr.e_phnum == 0) {
				// ERROR!
				err = -1;

			// Moving to the PH sections
			} else if (fseeko(fh, ehdr.e_phoff, SEEK_SET) != 0) {
				// ERROR!
				err = -1;

			} else {
				off_t      elf_end = 0;
				Elf32_Phdr phdr;
				off_t      segment_end;

				for (Elf32_Half i = 0; i < ehdr.e_phnum; ++i) {

					if (fread(&phdr, sizeof(phdr), 1, fh) != 1) {
						// ERROR!
						err = -2;
						break;

					} else {
						segment_end = (off_t)phdr.p_offset + (off_t)phdr.p_filesz;
						if (segment_end > elf_end)
							elf_end = segment_end;
					}
				}
				size = (int)elf_end;
			}
		}
	}

	if (fh) fclose(fh);
	
	return(err < 0 ? -1 : size);
}
