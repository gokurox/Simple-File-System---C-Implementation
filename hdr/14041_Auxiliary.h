#include <stdint.h>

#ifndef AUX_H
#define AUX_H

void acquire_fsfile (int , uint64_t);
void findFreeInode (int, fsinode_t *);
uint64_t findFreeData (int);
void write_file (int , char *, char *);
void read_file (int , char *);
void print_fsdata_char (fsdata_t *);

#endif