#include <stdint.h>
#include "./14041_FS_Data.h"

#ifndef DBITMAP_H
#define DBITMAP_H

typedef struct FileSystem_Dbitmap {
	uint64_t size;
	fsdata_t *Map;
} fsdbitmap_t;

void init_fsdbitmap (fsdbitmap_t *);

void write_fsdbitmap (int, fsdbitmap_t *);
void read_fsdbitmap (int, fsdbitmap_t *);
void print_fsdbitmap (fsdbitmap_t *);

#endif