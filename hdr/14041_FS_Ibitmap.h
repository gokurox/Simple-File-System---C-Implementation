#include <stdint.h>
#include "./14041_FS_Data.h"

#ifndef IBITMAP_H
#define IBITMAP_H

typedef struct FileSystem_Ibitmap {
	uint64_t size;
	fsdata_t *Map;
} fsibitmap_t;

void init_fsibitmap (fsibitmap_t *);

void write_fsibitmap (int, fsibitmap_t *);
void read_fsibitmap (int, fsibitmap_t *);
void print_fsibitmap (fsibitmap_t *);

#endif