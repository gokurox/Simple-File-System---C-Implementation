#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

int main ()
{
	int fsdisk, rc;
	uint64_t choice1, inputInt;
	char inputString[1024], inputString_II[1024];
	fsibitmap_t iBitmap;
	fsdbitmap_t dBitmap;


	fsdisk = 0;

	printf ("1. Create a new FileSystem" "\n"
		"2. Work with an Existing FileSystem" "\n"
		"\n");
	scanf ("%ld", &choice1);

	switch (choice1)
	{
		case 1:	
			printf ("Enter new FileSystem Name: ");
			scanf ("%s", inputString);
			printf ("Enter FileSystem Size in MB: ");
			scanf ("%ld", &inputInt);

			fsdisk = create_SFS (inputString, inputInt);

			break;
		case 2:
			printf ("Enter FileSystem Name: ");
			scanf ("%s", inputString);

			fsdisk = open_SFS (inputString);

			break;
		default:
			printf ("EXITING .........." "\n\n");
			exit (0);
			break;
	}

	do
	{
		printf ("1. Write a file to FileSystem" "\n"
			"2. Read a file from FileSystem" "\n"
			"3. Print Inode Bitmap" "\n"
			"4. Print Data Bitmap" "\n"
			"5. Print all Files in the System" "\n\n");
		scanf ("%ld", &choice1);

		switch (choice1)
		{
			case 1:
				printf ("Enter location of the File: ");
				scanf ("%s", inputString);
				printf ("Enter Name of the file to be given in SFS: ");
				scanf ("%s", inputString_II);

				write_file (fsdisk, inputString, inputString_II);
				break;
			case 2:
				printf ("Enter Name of the file to read: ");
				scanf ("%s", inputString);

				read_file (fsdisk, inputString);
				break;
			case 3:
				init_fsibitmap (&iBitmap);
				read_fsibitmap (fsdisk, &iBitmap);
				print_fsibitmap (&iBitmap);
				break;
			case 4:
				init_fsdbitmap (&dBitmap);
				read_fsdbitmap (fsdisk, &dBitmap);
				print_fsdbitmap (&dBitmap);
				break;
			case 5:
				print_files (fsdisk);
				break;
			default:
				printf ("EXITING .........." "\n\n");
				break;
		}

	} while (choice1 >= 1 && choice1 <= 5);

	rc = close (fsdisk);
	assert (rc == 0);

	return 0;
}

/***************************************************************************/
/***************************************************************************/

int create_SFS (char *fsname, uint64_t fssize_MB)
{
	int fsdisk;
	char *prefix = "./FileSystem Disks/";		// NOTE: This is relative to Makefile

	// Putting prefix in front of fsname
	char *buffer = (char *) malloc (strlen (fsname) + strlen (prefix));
	sprintf (buffer, "%s%s", prefix, fsname);
	fsname = strdup (buffer);
	free (buffer);
	buffer = NULL;

	// Opening the FS file
	fsdisk = open (fsname, O_RDWR | O_CREAT);
	if (fsdisk == -1)
	{
		printf ("ERROR:" "\n"
			"\t" "FUNTION: create_SFS" "\n"
			"\t" "DESC: Couldn't Open FS file: %s" "\n"
			"EXITING .........." "\n\n",
			fsname);
		exit (0);
	}
	else
	{
		printf ("SUCCESS:" "\n"
			"\t" "FS file Opened Successfully" "\n\n");
	}

	// Performing Initialization of FileSystem
	acquire_fsfile (fsdisk, fssize_MB);
	calculate_fssuperblock (&(SUPERBLOCK), fssize_MB);
	write_fssuperblock (fsdisk, &(SUPERBLOCK));

	fsibitmap_t iBitmap;
	init_fsibitmap (&iBitmap);
	write_fsibitmap (fsdisk, &iBitmap);

	fsdbitmap_t dBitmap;
	init_fsdbitmap (&dBitmap);
	write_fsdbitmap (fsdisk, &dBitmap);

	return fsdisk;
}

/***************************************************************************/

int open_SFS (char *fsname)
{
	int fsdisk;
	char *prefix = "./FileSystem Disks/";		// NOTE: This is relative to Makefile

	// Putting prefix in front of fsname
	char *buffer = (char *) malloc (strlen (fsname) + strlen (prefix));
	sprintf (buffer, "%s%s", prefix, fsname);
	fsname = strdup (buffer);
	free (buffer);
	buffer = NULL;

	// Opening the FS file
	fsdisk = open (fsname, O_RDWR);
	if (fsdisk == -1)
	{
		printf ("ERROR:" "\n"
			"\t" "FUNTION: open_SFS" "\n"
			"\t" "DESC: Couldn't Open FS file: %s" "\n"
			"EXITING .........." "\n\n",
			fsname);
		exit (0);
	}
	else
	{
		printf ("SUCCESS:" "\n"
			"\t" "FS file Opened Successfully" "\n\n");
	}

	// Performing Initialization of FileSystem
	read_fssuperblock (fsdisk, &(SUPERBLOCK));
	print_fssuperblock_global ();

	return fsdisk;
}

/***************************************************************************/
/***************************************************************************/