#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>

#include "../hdr/14041_FS_Global.h"

#include "../hdr/14041_FS_Superblock.h"
#include "../hdr/14041_FS_Ibitmap.h"
#include "../hdr/14041_FS_Dbitmap.h"
#include "../hdr/14041_FS_Inode.h"
#include "../hdr/14041_FS_Data.h"
#include "../hdr/14041_Auxiliary.h"

/************************************************************************/

void init_fsdata (fsdata_t *data)
{
	int i;
	
	for (i = 0; i < CHUNKS_PER_BLOCK; i++)
	{
		data -> chunks [i] = 0;
	}
}

void write_fsdata (int fsdisk, fsdata_t *data, uint64_t dNumber)
{
	int rc;
	uint64_t writeAddress;

	writeAddress = ((dNumber * sizeof (fsdata_t)) + SUPERBLOCK.fsdata_begByte);

	rc = lseek (fsdisk, writeAddress, SEEK_SET);
	assert (rc != -1);

	rc = write (fsdisk, (void *)(data), sizeof (fsdata_t));
	assert (rc == sizeof (fsdata_t));

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void read_fsdata (int fsdisk, fsdata_t *data, uint64_t dNumber)
{
	int rc;
	uint64_t readAddress;

	readAddress = ((dNumber * sizeof (fsdata_t)) + SUPERBLOCK.fsdata_begByte);

	rc = lseek (fsdisk, readAddress, SEEK_SET);
	assert (rc != -1);

	rc = read (fsdisk, (void *)(data), sizeof (fsdata_t));
	assert (rc != -1);

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void print_fsdata (fsdata_t *data)
{
	int i;
	uint32_t tChunk;

	for (i = 0; i < CHUNKS_PER_BLOCK; i++)
	{
		tChunk = data -> chunks[i];
		printf ("%X" "\n", tChunk);
	}			
}

/************************************************************************/

void init_fsinode (fsinode_t *inode)
{
	int i;

	inode -> validBit = 0;
	inode -> inodeNum = 0;
	inode -> fileSize = 0;
	inode -> usedLinks = 0;

	for (i = 0; i < INODE_DDL; i++)
		inode -> ddl[i] = 0;
}

void write_fsinode (int fsdisk, fsinode_t *inode, uint64_t iNumber)
{
	int rc;
	uint64_t writeAddress;

	writeAddress = ((iNumber * sizeof (fsinode_t)) + SUPERBLOCK.fsinode_begByte);

	rc = lseek (fsdisk, writeAddress, SEEK_SET);
	assert (rc != -1);

	rc = write (fsdisk, (void *)(inode), sizeof (fsinode_t));
	assert (rc == sizeof (fsinode_t));

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void read_fsinode (int fsdisk, fsinode_t *inode, uint64_t iNumber)
{
	int rc;
	uint64_t readAddress;

	readAddress = ((iNumber * sizeof (fsinode_t)) + SUPERBLOCK.fsinode_begByte);

	rc = lseek (fsdisk, readAddress, SEEK_SET);
	assert (rc != -1);

	rc = read (fsdisk, (void *)(inode), sizeof (fsinode_t));
	assert (rc != -1);

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void print_fsinode (fsinode_t *inode)
{
	int i;

	printf ("inode -> validBit = \t" "%d" "\n", inode -> validBit);
	printf ("inode -> inodeNum = \t" "%ld" "\n", inode -> inodeNum);
	printf ("inode -> fileSize = \t" "%ld" "\n", inode -> fileSize);
	printf ("inode -> fileName = \t" "%s" "\n", inode -> fileName);
	printf ("inode -> usedLinks = \t" "%d" "\n\n", inode -> usedLinks);

	for (i = 0; i < INODE_DDL; i++)
		printf ("inode -> ddl[%d] = \t" "%ld" "\n", i, inode -> ddl[i]);

	printf ("\n");
}

/************************************************************************/

void calculate_fssuperblock (fssuperblock_t *superblock, uint64_t size_MB)
{
	uint64_t size_KB;

	size_KB = size_MB * 1024;

	superblock -> fssize_MB = size_MB;
	superblock -> fsavailblocks = (size_KB / BLOCKSIZE_KB);

	superblock -> fsinode_total = ceil ((double)(superblock -> fsavailblocks -3) / INODE_PER_BLOCK);
	superblock -> fsibitmap_total = ceil ((double)(superblock -> fsinode_total) / BITMAP_PER_BLOCK);
	superblock -> fsdbitmap_total = ceil ((double)((superblock -> fsavailblocks) - (superblock -> fsinode_total) - (superblock -> fsibitmap_total) -2) / BITMAP_PER_BLOCK);
	superblock -> fsdata_total = (superblock -> fsavailblocks) - (superblock -> fsinode_total) - (superblock -> fsibitmap_total) - (superblock -> fsdbitmap_total) - 1;

	superblock -> fsibitmap_beg = 1;
	superblock -> fsibitmap_ter = superblock -> fsibitmap_total + superblock -> fsibitmap_beg -1;

	superblock -> fsdbitmap_beg = superblock -> fsibitmap_ter +1;
	superblock -> fsdbitmap_ter = superblock -> fsdbitmap_total + superblock -> fsdbitmap_beg -1;

	superblock -> fsinode_beg = superblock -> fsdbitmap_ter +1;
	superblock -> fsinode_ter = superblock -> fsinode_total + superblock -> fsinode_beg -1;

	superblock -> fsdata_beg = superblock -> fsinode_ter +1;
	superblock -> fsdata_ter = superblock -> fsdata_total + superblock -> fsdata_beg -1;

	superblock -> fsibitmap_begByte = BLOCKSIZE_B;
	superblock -> fsdbitmap_begByte = superblock -> fsibitmap_begByte + (BLOCKSIZE_B * superblock -> fsibitmap_total);
	superblock -> fsinode_begByte = superblock -> fsdbitmap_begByte + (BLOCKSIZE_B * superblock -> fsdbitmap_total);
	superblock -> fsdata_begByte = superblock -> fsinode_total + (BLOCKSIZE_B * superblock -> fsinode_total);
}

/************************************************************************/

void init_fsibitmap (fsibitmap_t *iBitmap)
{
	int i, j;

	iBitmap -> size = SUPERBLOCK.fsibitmap_total;
	iBitmap -> Map = (fsdata_t *) malloc (sizeof (fsdata_t) * (iBitmap -> size));

	for (i = 0; i < (iBitmap -> size); i++)
	{
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			(iBitmap -> Map)[i].chunks[j] = 0;
		}
	}
}

void write_fsibitmap (int fsdisk, fsibitmap_t *iBitmap)
{
	int rc, i;
	uint64_t writeAddress;

	writeAddress = SUPERBLOCK.fsibitmap_begByte;

	rc = lseek (fsdisk, writeAddress, SEEK_SET);
	assert (rc != -1);

	for (i = 0; i < SUPERBLOCK.fsibitmap_total; i++)
	{
		rc = write (fsdisk, (void *)(&(iBitmap -> Map[i])), sizeof (fsdata_t));
		assert (rc == sizeof (fsdata_t));
	}

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void read_fsibitmap (int fsdisk, fsibitmap_t *iBitmap)
{
	int rc, i;
	uint64_t readAddress;

	readAddress = SUPERBLOCK.fsibitmap_begByte;

	rc = lseek (fsdisk, readAddress, SEEK_SET);
	assert (rc != -1);

	for (i = 0; i < SUPERBLOCK.fsibitmap_total; i++)
	{
		rc = read (fsdisk, (void *)(&(iBitmap -> Map[i])), sizeof (fsdata_t));
		assert (rc == sizeof (fsdata_t));
	}

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void print_fsibitmap (fsibitmap_t *iBitmap)
{
	int i, j, k;
	fsdata_t tMap;
	uint32_t tChunk, Mask;

	printf ("ibitmap -> size = \t" "%ld" "\n", iBitmap -> size);

	uint64_t chunkCount = 0;
	uint64_t validBits = SUPERBLOCK.fsdata_total;
	uint64_t fullChunks = floor (validBits / BLOCKCHUNKSIZE_b);
	uint64_t lastChunk_vb = (validBits - (fullChunks * BLOCKCHUNKSIZE_b));

	printf ("The following is Inode Bitmap in HEX: " "\n");

	for (i = 0; i < (iBitmap -> size); i++)
	{
		tMap = iBitmap -> Map[i];
		
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			chunkCount ++;
			if (chunkCount <= fullChunks)
			{
				tChunk = tMap.chunks[j];
				printf ("%X", tChunk);
			}
			else
			{
				tChunk = tMap.chunks[j];
				Mask = 0;
				for (k = 0; k < lastChunk_vb; k ++)
				{
					Mask |= 1 << (BLOCKCHUNKSIZE_b -1 -k);
				}
				tChunk &= Mask;
				printf ("%X", tChunk);
				break;
			}
		}
	}

	printf ("\n\n");
}

/************************************************************************/

void init_fsdbitmap (fsdbitmap_t *dBitmap)
{
	int i, j;

	dBitmap -> size = SUPERBLOCK.fsdbitmap_total;
	dBitmap -> Map = (fsdata_t *) malloc (sizeof (fsdata_t) * (dBitmap -> size));

	for (i = 0; i < (dBitmap -> size); i++)
	{
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			(dBitmap -> Map)[i].chunks[j] = 0;
		}
	}
}

void write_fsdbitmap (int fsdisk, fsdbitmap_t *dBitmap)
{
	int rc, i;
	uint64_t writeAddress;

	writeAddress = SUPERBLOCK.fsdbitmap_begByte;

	rc = lseek (fsdisk, writeAddress, SEEK_SET);
	assert (rc != -1);

	for (i = 0; i < SUPERBLOCK.fsibitmap_total; i++)
	{
		rc = write (fsdisk, (void *)(&(dBitmap -> Map[i])), sizeof (fsdata_t));
		assert (rc == sizeof (fsdata_t));
	}

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void read_fsdbitmap (int fsdisk, fsdbitmap_t *dBitmap)
{
	int rc, i;
	uint64_t readAddress;

	readAddress = SUPERBLOCK.fsdbitmap_begByte;

	rc = lseek (fsdisk, readAddress, SEEK_SET);
	assert (rc != -1);

	for (i = 0; i < SUPERBLOCK.fsibitmap_total; i++)
	{
		rc = read (fsdisk, (void *)(&(dBitmap -> Map[i])), sizeof (fsdata_t));
		assert (rc == sizeof (fsdata_t));
	}

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void print_fsdbitmap (fsdbitmap_t *dBitmap)
{
	int i, j, k;
	fsdata_t tMap;
	uint32_t tChunk, Mask;

	printf ("dbitmap -> size = \t" "%ld" "\n", dBitmap -> size);

	uint64_t chunkCount = 0;
	uint64_t validBits = SUPERBLOCK.fsdata_total;
	uint64_t fullChunks = floor (validBits / BLOCKCHUNKSIZE_b);
	uint64_t lastChunk_vb = (validBits - (fullChunks * BLOCKCHUNKSIZE_b));

	printf ("The following is Data Bitmap in HEX: " "\n");

	for (i = 0; i < (dBitmap -> size); i++)
	{
		tMap = dBitmap -> Map[i];
		
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			chunkCount ++;
			if (chunkCount <= fullChunks)
			{
				tChunk = tMap.chunks[j];
				printf ("%X", tChunk);
			}
			else
			{
				tChunk = tMap.chunks[j];
				Mask = 0;
				for (k = 0; k < lastChunk_vb; k ++)
				{
					Mask |= 1 << (BLOCKCHUNKSIZE_b -1 -k);
				}
				tChunk &= Mask;
				printf ("%X", tChunk);
				break;
			}
		}
	}

	printf ("\n\n");
}

/************************************************************************/

void findFreeInode (int fsdisk, fsinode_t *inode)
{
	int rc, i, j, k, found;
	fsibitmap_t iBitmap;
	fsdata_t tMap;
	uint32_t tChunk;
	uint64_t Mask, iNum;

	uint64_t chunkCount = 0;
	uint64_t validBits = SUPERBLOCK.fsdata_total;
	uint64_t fullChunks = floor (validBits / BLOCKCHUNKSIZE_b);
	uint64_t lastChunk_vb = (validBits - (fullChunks * BLOCKCHUNKSIZE_b));

	init_fsibitmap (&iBitmap);
	read_fsibitmap (fsdisk, &iBitmap);

	found = 0;

	for (i = 0; i < (iBitmap.size); i++)
	{
		tMap = iBitmap.Map[i];
		
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			chunkCount ++;
			tChunk = tMap.chunks[j];

			if (chunkCount <= fullChunks)
			{
				Mask = 0;
				for (k = 0; k < BLOCKCHUNKSIZE_b; k++)
				{
					Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
					if (!(tChunk & Mask))
					{
						printf ("FREE INODE FOUND" "\n\n");

						found = 1;
						iNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;
						inode -> validBit = 1;
						inode -> inodeNum = iNum;

						tChunk |= Mask;
						iBitmap.Map[i].chunks[j] |= Mask;

						break;
					}
				}
			}
			else
			{
				Mask = 0;
				for (k = 0; k < lastChunk_vb; k++)
				{
					Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
					if (!(tChunk & Mask))
					{
						printf ("FREE INODE FOUND" "\n\n");

						found = 1;
						iNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;
						inode -> validBit = 1;
						inode -> inodeNum = iNum;

						tChunk |= Mask;
						iBitmap.Map[i].chunks[j] |= Mask;

						break;
					}
				}
				break;
			}

			if (found)
				break;
		}

		if (found)
			break;
	}

	if (found)
		write_fsibitmap (fsdisk, &iBitmap);
}

uint64_t findFreeData (int fsdisk)
{
	int rc, i, j, k, found;
	fsdbitmap_t dBitmap;
	fsdata_t tMap;
	uint32_t tChunk;
	uint64_t Mask, dNum;

	uint64_t chunkCount = 0;
	uint64_t validBits = SUPERBLOCK.fsdata_total;
	uint64_t fullChunks = floor (validBits / BLOCKCHUNKSIZE_b);
	uint64_t lastChunk_vb = (validBits - (fullChunks * BLOCKCHUNKSIZE_b));

	init_fsdbitmap (&dBitmap);
	read_fsdbitmap (fsdisk, &dBitmap);

	found = 0;

	for (i = 0; i < (dBitmap.size); i++)
	{
		tMap = dBitmap.Map[i];
		
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			chunkCount ++;
			tChunk = tMap.chunks[j];

			if (chunkCount <= fullChunks)
			{
				Mask = 0;
				for (k = 0; k < BLOCKCHUNKSIZE_b; k++)
				{
					Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
					if (!(tChunk & Mask))
					{
						found = 1;
						dNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;

						tChunk |= Mask;
						dBitmap.Map[i].chunks[j] |= Mask;

						break;
					}
				}
			}
			else
			{
				Mask = 0;
				for (k = 0; k < lastChunk_vb; k++)
				{
					Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
					if (!(tChunk & Mask))
					{
						found = 1;
						dNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;

						tChunk |= Mask;
						dBitmap.Map[i].chunks[j] |= Mask;

						break;
					}
				}
				break;
			}

			if (found)
				break;
		}

		if (found)
			break;
	}

	if (found)
		write_fsdbitmap (fsdisk, &dBitmap);

	return dNum;
}

/************************************************************************/

void write_file (int fsdisk, char *filepath, char *filename)
{
	int ifdes, rc, i, j, k;
	long inputFileLength;
	uint64_t requiredChunks, readCount;
	char tChunk;
	fsdata_t *fileBuffer;
	fsinode_t inode;

	// Open input File
	ifdes = open (filepath, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	// Calculate Length
	inputFileLength = lseek (ifdes, 0, SEEK_END);
	assert (inputFileLength != -1);

	requiredChunks = ceil ((double) inputFileLength / BLOCKCHUNKSIZE_B);
	fileBuffer = (fsdata_t *) malloc (sizeof (fsdata_t) * requiredChunks);

	for (i = 0; i < requiredChunks; i++)
	{
		init_fsdata (&fileBuffer[i]);
	}

	rc = lseek (ifdes, 0, SEEK_SET);
	assert (rc != -1);

	// Read File
	for (i = 0; i < requiredChunks; i++)
	{
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			for (k = 0; k < BLOCKCHUNKSIZE_B; k++)
			{
				rc = read (ifdes, (void *)(&tChunk), sizeof (char));
				assert (rc != -1);

				if (rc)
				{
					fileBuffer[i].chunks[j] |= (uint32_t)(tChunk << (k * 8));
				}
			}
		}
	}
	close (ifdes);

	init_fsinode (&inode);
	findFreeInode (fsdisk, &inode);

	if (inode.validBit == 0)
	{
		printf ("NO INODE FOUND FREE" "\n"
			"EXITING .........." "\n\n");
		exit (0);
	}

	inode.fileSize = inputFileLength;
	strcpy (inode.fileName, filename);
	inode.usedLinks = requiredChunks;

	for (i = 0; i < (inode.usedLinks); i++)
	{
		int temp = findFreeData (fsdisk);
		if (temp == -1)
		{
			printf ("NOT ENOUGH DATA SPACE IN SYSTEM" "\n"
				"EXITING .........." "\n\n");
			exit (0);
		}
		inode.ddl[i] = temp;
	}

	write_fsinode (fsdisk, &inode, inode.inodeNum);
	
	for (i = 0; i < (inode.usedLinks); i++)
	{
		write_fsdata (fsdisk, &(fileBuffer[i]), inode.ddl[i]);
	}
}

void read_file (int fsdisk, char *filename)
{
	int rc, i, j, k, found;
	fsinode_t inode;
	fsibitmap_t iBitmap;
	fsdata_t tMap;
	uint32_t tChunk;
	uint64_t Mask, iNum;

	init_fsibitmap (&iBitmap);
	read_fsibitmap (fsdisk, &iBitmap);

	found = 0;

	for (i = 0; i < (iBitmap.size); i++)
	{
		tMap = iBitmap.Map[i];
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			tChunk = tMap.chunks[j];
			Mask = 0;
			for (k = 0; k < BLOCKCHUNKSIZE_b; k++)
			{
				Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
				if (tChunk & Mask)
				{
					iNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;
					init_fsinode (&inode);
					read_fsinode (fsdisk, &inode, iNum);

					if (inode.validBit)
					{
						if (strcmp (inode.fileName, filename) == 0)
						{
							printf ("FILE FOUND" "\n\n");

							found = 1;
							break;
						}	
					}
					else
					{
						printf ("SOMETHING IS VERY WRONG" "\n"
							"EXITING .........." "\n\n");
						exit (0);
					}
				}
			}

			if (found == 1)
				break;
		}

		if (found == 1)
			break;
	}

	if (found != 1)
	{
		printf ("FILE NOT FOUND" "\n\n");
		return;
	}

	printf ("Printing Contents:" "\n");

	for (i = 0; i < (inode.usedLinks); i++)
	{
		read_fsdata (fsdisk, &tMap, inode.ddl[i]);
		print_fsdata_char (&tMap);
	}

	printf ("\n");
}

void print_files (int fsdisk)
{
	int rc, i, j, k;
	fsinode_t inode;
	fsibitmap_t iBitmap;
	fsdata_t tMap;
	uint32_t tChunk;
	uint64_t Mask, iNum;

	init_fsibitmap (&iBitmap);
	read_fsibitmap (fsdisk, &iBitmap);

	for (i = 0; i < (iBitmap.size); i++)
	{
		tMap = iBitmap.Map[i];
		for (j = 0; j < CHUNKS_PER_BLOCK; j++)
		{
			tChunk = tMap.chunks[j];
			Mask = 0;
			for (k = 0; k < BLOCKCHUNKSIZE_b; k++)
			{
				Mask = 1 << (BLOCKCHUNKSIZE_b -1 -k);
				if (tChunk & Mask)
				{
					iNum = (i * CHUNKS_PER_BLOCK) + (j * BLOCKCHUNKSIZE_b) + k;
					init_fsinode (&inode);
					read_fsinode (fsdisk, &inode, iNum);

					if (inode.validBit)
					{
						printf ("FILE: %s" "\n", inode.fileName);
					}
					else
					{
						printf ("SOMETHING IS VERY WRONG" "\n"
							"EXITING .........." "\n\n");
						exit (0);
					}
				}
			}
		}
	}

	printf ("\n");
}

void print_fsdata_char (fsdata_t *data)
{
	int i, j;
	char buffer [BLOCKCHUNKSIZE_B +1];
	uint64_t size = BLOCKCHUNKSIZE_B;
	uint32_t tChunk;

	for (i = 0; i < CHUNKS_PER_BLOCK; i++)
	{
		tChunk = data -> chunks[i];

		j = 0;
		for (j = 0; j < BLOCKCHUNKSIZE_B; j++)
		{
			buffer[j] = (char)(tChunk >> (j * 8));
		}
		// buffer[j] = '\0';

		printf ("%s", buffer);
	}
}

void write_fssuperblock (int fsdisk, fssuperblock_t *superblock)
{
	int rc;
	uint64_t writeAddress;

	// Superblock is at start of the FileSystem
	writeAddress = 0;

	rc = lseek (fsdisk, writeAddress, SEEK_SET);
	assert (rc != -1);

	rc = write (fsdisk, (void *)(superblock), sizeof (fssuperblock_t));
	assert (rc == sizeof (fssuperblock_t));

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void read_fssuperblock (int fsdisk, fssuperblock_t *superblock)
{
	int rc;
	uint64_t readAddress;

	// Superblock is at start of the FileSystem
	readAddress = 0;

	rc = lseek (fsdisk, readAddress, SEEK_SET);
	assert (rc != -1);

	rc = read (fsdisk, (void *)(superblock), sizeof (fssuperblock_t));
	assert (rc != 1);

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

void print_fssuperblock_global ()
{
	printf ("SUPERBLOCK -> fssize_MB:\t" "%ld" "\n", SUPERBLOCK.fssize_MB);
	printf ("SUPERBLOCK -> fsavailblocks:\t" "%ld" "\n", SUPERBLOCK.fsavailblocks);
	printf ("SUPERBLOCK -> fsibitmap_beg:\t" "%ld" "\n", SUPERBLOCK.fsibitmap_beg);
	printf ("SUPERBLOCK -> fsibitmap_ter:\t" "%ld" "\n", SUPERBLOCK.fsibitmap_ter);
	printf ("SUPERBLOCK -> fsibitmap_total:\t" "%ld" "\n", SUPERBLOCK.fsibitmap_total);
	printf ("SUPERBLOCK -> fsibitmap_begByte\t" "%ld" "\n", SUPERBLOCK.fsibitmap_begByte);
	printf ("SUPERBLOCK -> fsdbitmap_beg:\t" "%ld" "\n", SUPERBLOCK.fsdbitmap_beg);
	printf ("SUPERBLOCK -> fsdbitmap_ter:\t" "%ld" "\n", SUPERBLOCK.fsdbitmap_ter);
	printf ("SUPERBLOCK -> fsdbitmap_total:\t" "%ld" "\n", SUPERBLOCK.fsdbitmap_total);
	printf ("SUPERBLOCK -> fsdbitmap_begByte\t" "%ld" "\n", SUPERBLOCK.fsdbitmap_begByte);
	printf ("SUPERBLOCK -> fsinode_beg:\t" "%ld" "\n", SUPERBLOCK.fsinode_beg);
	printf ("SUPERBLOCK -> fsinode_ter:\t" "%ld" "\n", SUPERBLOCK.fsinode_ter);
	printf ("SUPERBLOCK -> fsinode_total:\t" "%ld" "\n", SUPERBLOCK.fsinode_total);
	printf ("SUPERBLOCK -> fsinode_begByte\t" "%ld" "\n", SUPERBLOCK.fsinode_begByte);
	printf ("SUPERBLOCK -> fsdata_beg:\t" "%ld" "\n", SUPERBLOCK.fsdata_beg);
	printf ("SUPERBLOCK -> fsdata_ter:\t" "%ld" "\n", SUPERBLOCK.fsdata_ter);
	printf ("SUPERBLOCK -> fsdata_total:\t" "%ld" "\n", SUPERBLOCK.fsdata_total);
	printf ("SUPERBLOCK -> fsidata_begByte\t" "%ld" "\n\n", SUPERBLOCK.fsdata_begByte);
}

void acquire_fsfile (int fsdisk, uint64_t fssize_MB)
{
	int rc, i;
	uint64_t requiredBlocks;

	requiredBlocks = ((fssize_MB * 1024) / BLOCKSIZE_KB);
	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);

	fsdata_t spaceEater;
	init_fsdata (&(spaceEater));

	for (i = 1; i <= requiredBlocks; i++)
	{
		rc = write (fsdisk, (void *)(&spaceEater), sizeof (fsdata_t));
		assert (rc == sizeof (fsdata_t));
	}

	rc = lseek (fsdisk, 0, SEEK_SET);
	assert (rc != -1);
}

/************************************************************************/