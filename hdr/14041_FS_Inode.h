#include <stdint.h>

#ifndef FSINIODE_H
#define FSINIODE_H

typedef struct FileSystem_Inode {
	// Total Size = (4 + 8 + 8 + 120 + 4 + (8 * 14) Bytes) = (256 Bytes)
	uint32_t validBit;		// Valid when 1
	uint64_t inodeNum;
	
	uint64_t fileSize;
	char fileName[120];

	uint32_t usedLinks;
	uint64_t ddl [INODE_DDL];		// Direct Data Link
} fsinode_t;

void init_fsinode (fsinode_t *);
void init_fsinode_logged (fsinode_t *);

void write_fsinode (int, fsinode_t *, uint64_t);
void read_fsinode (int, fsinode_t *, uint64_t);
void print_fsinode (fsinode_t *);

#endif