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
#include <stdbool.h>
#include <elf.h>

#define MYPROC "/proc/self/exe"

typedef enum {
	x64bit,
	x32bit
} arch_t;

static FILE* __readProc() {
	struct stat st;
	FILE *out = NULL;
	if (stat(MYPROC, &st) == 0)
		out = fopen(MYPROC, "rb");
	return(out);
}

static sexiErrorCode_t __sizeCalculation (FILE *fh, arch_t arch, const void *elfStruct, unsigned int *size) {
	//
	// Description:
	//	This function reads the Program and Section header table's items and calcule the effective executable code size
	//
	//		+-------------------+
	//		| ELF Header        |
	//		+-------------------+
	//		| Program Header #0 |
	//		| Program Header #1 |
	//		| ...               |
	//		+-------------------+
	//		|                   |
	//		|   codice, dati    |
	//		|   sezioni ...     |
	//		|                   |
	//		+-------------------+
	//		| Section Header #0 |
	//		| Section Header #1 |
	//		| ...               |
	//		+-------------------+
	//
	// 	Header Section positions:
	//	=========================
	//		In the last diagram "Section Headers" section is after the "Program Headers" one. But the ELF format
	//		protocol does not specify this aspect. This is the reason because to find the last used address of
	//		the the two sectiuon is the only solution
	//
	//	How to calculate the size:
	//	==========================
	//		The Program and Section header's items can have padding or allineament spaces, so the best option is to
	//		to find the end of the last segment. Example of a hipotetical situation:
	//
	//			0        1000                 5000      5200        8000     8300
	//			|---------|////////////////////|---------|////////////|--------|
	//
	// Returned value:
	//	SEXIEC_SUCCESS
	//	SEXIEC_ERROR_IOOPFAILED
	//
	Elf64_Phdr      phdr64;
	Elf32_Phdr      phdr32;
	off_t           segment_end;
	off_t           elf_end = 0;
	sexiErrorCode_t ec = SEXIEC_SUCCESS;
	Elf64_Ehdr      *ehdr64 = NULL;
	Elf32_Ehdr      *ehdr32 = NULL;
	
	// Size reset
	*size = 0;

	if (arch == x64bit) 
		ehdr64 = (Elf64_Ehdr*)elfStruct;
	else
		ehdr32 = (Elf32_Ehdr*)elfStruct;
	
	
	// Moving to the PH sections
	if (
		(arch == x64bit && fseeko(fh, ehdr64->e_phoff, SEEK_SET) != 0) ||
		(arch == x32bit && fseeko(fh, ehdr32->e_phoff, SEEK_SET) != 0)
	) {
		// ERROR!
		ec = SEXIEC_ERROR_IOOPFAILED;

	} else {
		unsigned int prgHeadNum   = (arch == x64bit) ? ehdr64->e_phnum : ehdr32->e_phnum;
		off_t        ph_table_end = 0;
		off_t        sh_table_end = 0;

		if (arch == x64bit) {
			ph_table_end = (off_t)ehdr64->e_phoff + (off_t)ehdr64->e_phentsize * ehdr64->e_phnum;
			sh_table_end = (off_t)ehdr64->e_shoff + (off_t)ehdr64->e_shentsize * ehdr64->e_shnum;
		} else {
			ph_table_end = (off_t)ehdr32->e_phoff + (off_t)ehdr32->e_phentsize * ehdr32->e_phnum;
			sh_table_end = (off_t)ehdr32->e_shoff + (off_t)ehdr32->e_shentsize * ehdr32->e_shnum;
		}

		elf_end = (ph_table_end > sh_table_end) ? ph_table_end : sh_table_end;

		if (ph_table_end > sh_table_end) {
			segment_end = 0;

			for (unsigned int i = 0; i < prgHeadNum; ++i) {
				//
				// Program Header Table's item reading
				//
				if (
					(arch == x64bit && fread(&phdr64, sizeof(phdr64), 1, fh) != 1) ||
					(arch == x32bit && fread(&phdr32, sizeof(phdr32), 1, fh) != 1)
				) {
					// ERROR!
					ec = SEXIEC_ERROR_IOOPFAILED;
					break;

				} else {
					// Now, I look for the end of the last segment
					if (arch == x64bit) 
						segment_end = (off_t)phdr64.p_offset + (off_t)phdr64.p_filesz;
					else
						segment_end = (off_t)phdr32.p_offset + (off_t)phdr32.p_filesz;
						
					if (segment_end > elf_end)
						elf_end = segment_end;
				}
			}
			*size = elf_end;

		} else {
			segment_end = 0;

			if (
				(arch == x64bit && fseeko(fh, ehdr64->e_shoff, SEEK_SET) != 0) ||
				(arch == x32bit && fseeko(fh, ehdr32->e_shoff, SEEK_SET) != 0) 
			) {
				// ERROR!
				ec = SEXIEC_ERROR_IOOPFAILED;
				
			} else {
				Elf64_Shdr   shdr64;
				Elf32_Shdr   shdr32;
				unsigned int selHeadNum = (arch == x64bit) ? ehdr64->e_shnum : ehdr32->e_shnum;
		
				for (unsigned int i = 0; i < selHeadNum; ++i) {

					if (
						(arch == x64bit && fread(&shdr64, sizeof(shdr64), 1, fh) != 1) ||
						(arch == x32bit && fread(&shdr32, sizeof(shdr32), 1, fh) != 1)
					) {
						// ERROR!
						ec = SEXIEC_ERROR_IOOPFAILED;
						break;

					} else {
						if (
							(arch == x64bit && shdr64.sh_type != SHT_NOBITS) ||
							(arch == x32bit && shdr32.sh_type != SHT_NOBITS)
						) {
							if (arch == x64bit) 
								segment_end = (off_t)shdr64.sh_offset + (off_t)shdr64.sh_size;
							else
								segment_end = (off_t)shdr32.sh_offset + (off_t)shdr32.sh_size;
							
							if (segment_end > elf_end)
								elf_end = segment_end;
						}
				
					}
				}
				*size = elf_end;
			}
		}
	}

	return(ec);
}

//------------------------------------------------------------------------------------------------------------------------------
//                                           P U B L I C   F U N C T I O N S
//------------------------------------------------------------------------------------------------------------------------------
sexiErrorCode_t elfReading_getSize (unsigned int *binSize) {
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
	// Returned value:
	//	SEXIEC_SUCCESS               The binary file's sectionsize has been correctly calculated
	//	SEXIEC_ERROR_IOOPFAILED      Some error encountered in reading/seeking operations
	//	SEXIEC_ERROR_CORRUPTEDDATA   Unknown of incoherent internal ELF structures met
	//
          
	uint8_t ident[EI_NIDENT];
	FILE    *fh = __readProc();
	sexiErrorCode_t ec = SEXIEC_SUCCESS;
	
	// Checking for function's arguments
	if (binSize == NULL)
		// ERROR!
		ec = SEXIEC_ERROR_ILLEGALARG;

	// Checking for MYPROC file
	if (fh == NULL) {
		// ERROR!
		ec = SEXIEC_ERROR_IOOPFAILED;

	// ELFMAG field reading
	} else if (fread(ident, sizeof(ident), 1, fh) != 1) {
		// ERROR!
		ec = SEXIEC_ERROR_IOOPFAILED;

	// Rewind
	} else if (fseeko(fh, 0, SEEK_SET) != 0) {
		// ERROR!
		ec = SEXIEC_ERROR_IOOPFAILED;

	} else {
		*binSize = 0;
		
		if (ident[EI_CLASS] == ELFCLASS64) {
			Elf64_Ehdr ehdr;

			// The whole Elf64_Ehdr structure reading
			if (fread(&ehdr, sizeof(ehdr), 1, fh) != 1) {
				// ERROR!
				ec = SEXIEC_ERROR_IOOPFAILED;
			
			// Entry size
			} else if (ehdr.e_phentsize != sizeof(Elf64_Phdr)) {
				// ERROR!
				ec = SEXIEC_ERROR_CORRUPTEDDATA;

			// Number of entries
			} else if (ehdr.e_phnum == 0) {
				// ERROR!
				ec = SEXIEC_ERROR_CORRUPTEDDATA;

			} else { 
				ec = __sizeCalculation (fh, x64bit, (void*)&ehdr, binSize);
				//printf("DEBUG(%d)! arch = 64bit; size = %d bytes\n", __LINE__, size);
			}
			
		} else if (ident[EI_CLASS] == ELFCLASS32) {
			Elf32_Ehdr ehdr;

			// The whole Elf32_Ehdr structure reading
			if (fread(&ehdr, sizeof(ehdr), 1, fh) != 1) {
				// ERROR!
				ec = SEXIEC_ERROR_IOOPFAILED;
			
			// Entry size
			} else if (ehdr.e_phentsize != sizeof(Elf32_Phdr)) {
				// ERROR!
				ec = SEXIEC_ERROR_CORRUPTEDDATA;

			// Number of entries
			} else if (ehdr.e_phnum == 0) {
				// ERROR!
				ec = SEXIEC_ERROR_CORRUPTEDDATA;

			} else {
				ec = __sizeCalculation (fh, x32bit, (void*)&ehdr, binSize);
				// printf("DEBUG(%d)! arch = 32bit; size = %d bytes\n", __LINE__, size);
			}
		}
	}

	if (fh) fclose(fh);
	
	return(ec);
}
