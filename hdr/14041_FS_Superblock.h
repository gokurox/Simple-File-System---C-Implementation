#include <stdint.h>

#ifndef FSSUPERBLOCK_H
#define FSSUPERBLOCK_H

typedef struct FileSystem_SuperBlock {
	uint64_t fssize_MB;			// FileSystem Size in MB
	uint64_t fsavailblocks;			// Availaible Blocks in filesystem

	uint64_t fsibitmap_beg;			// Inode Bitmap beginning Block Number
	uint64_t fsibitmap_ter;			// Inode Bitmap terminating Block Number (Inclusive)
	uint64_t fsibitmap_total;		// Inode Bitmap Total Blocks Used
	uint64_t fsibitmap_begByte;

	uint64_t fsdbitmap_beg;			// Data Bitmap beginning Block Number
	uint64_t fsdbitmap_ter;			// Data Bitmap terminating Block Number (Inclusive)
	uint64_t fsdbitmap_total;		// Data Bitmap Total Blocks Used
	uint64_t fsdbitmap_begByte;

	uint64_t fsinode_beg;			// Inode Beginning Block Number
	uint64_t fsinode_ter;			// Inode Terminating Block Number (Inclusive)
	uint64_t fsinode_total;			// Inode Total Blocks Used
	uint64_t fsinode_begByte;

	uint64_t fsdata_beg;			// Data Beginning Block Number
	uint64_t fsdata_ter;			// Data Terminating Block Number (Inclusive)
	uint64_t fsdata_total;			// Data Total Blocks Used
						// All data blocks are valid to use
	uint64_t fsdata_begByte;

} fssuperblock_t;

void calculate_fssuperblock (fssuperblock_t *, uint64_t);
void calculate_fssuperblock_logged (fssuperblock_t *, uint64_t);

void write_fssuperblock (int, fssuperblock_t *);
void read_fssuperblock (int, fssuperblock_t *);

void print_fssuperblock_global ();

#endif