##########################################################################################

src_dir = ./src
hdr_dir = ./hdr
exe_dir = ./exec
fsdisk_dir = ./FileSystem Disks

##########################################################################################

source =  $(src_dir)/14041_Auxillary.c
source += $(src_dir)/14041_Main.c
source += $(hdr_dir)/14041_Auxiliary.h
source += $(hdr_dir)/14041_FS_Data.h
source += $(hdr_dir)/14041_FS_Dbitmap.h
source += $(hdr_dir)/14041_FS_Global.h
source += $(hdr_dir)/14041_FS_Ibitmap.h
source += $(hdr_dir)/14041_FS_Inode.h
source += $(hdr_dir)/14041_FS_Superblock.h

obj = $(exe_dir)/exe.out

##########################################################################################

run: 
	$(exe_dir)/exe.out

compile:
	gcc -o $(exe_dir)/exe.out $(src_dir)/14041_Auxiliary.c $(src_dir)/14041_Main.c -lm

clean:
	rm -rf ./exec/* ./FileSystem Disks/*