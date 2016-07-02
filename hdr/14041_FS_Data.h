#include <stdint.h>

#ifndef FSDATA_H
#define FSDATA_H

typedef struct FileSystem_DataBlock {
	uint32_t chunks [CHUNKS_PER_BLOCK];
} fsdata_t;

void init_fsdata (fsdata_t *);

void write_fsdata (int, fsdata_t *, uint64_t);
void read_fsdata (int, fsdata_t *, uint64_t);
void print_fsdata (fsdata_t *);

#endif