#include "disk_emu.h"

#define MAX_FILE_DESCRIPTORS 10
struct fileDescriptor fileDescriptorTable[MAX_FILE_DESCRIPTORS];
int fileDescriptorTableLength = 2; // Refers to the last array index

#define FD_ROOT_DIR 0
#define FD_INODE_TABLE 1
#define FD_FREE_BLOCKS 2

#define BLOCK_SIZE 1024
#define FILENAME "my_disk.disk"
#define NUM_BLOCKS 100

void mksfs(int fresh) {
	if(fresh) {
		init_fresh_disk(FILENAME, BLOCK_SIZE, NUM_BLOCKS);
	} else {
		init_disk(FILENAME, BLOCK_SIZE, NUM_BLOCKS);
	}

	char superblock_buffer[BLOCK_SIZE];
	read_blocks(0, 1, superblock_buffer);	

	struct superblock superblock = (struct superblock) superblock_buffer;
	

	if(fresh) {
		struct empty_inode empty_inode;
		empty_inode.size = 0;

		for(int i = 0; i < 12; i++) {
			empty_inode.direct_block[i] = 0;
		}

		empty_inode.indirect_block = 0;
	
		superblock.root_dir = empty_inode;
		superblock.inode_table = empty_inode;
		superblock.free_blocks = empty_inode;

		// More stuff here
	}
	

	fileDescriptorTable[FD_ROOT_DIR] = superblock.root_dir;
	fileDescriptorTable[FD_INODE_TABLE] = superblock.inode_table;
	fileDescriptorTable[FD_FREE_BLOCKS] = superblock.free_blocks;


	// Use this on inode table change, perhaps on sfs_open or sfs_save, inode_mark_block, etc.

	// superblock.root_dir = fileDescriptorTable[FD_ROOT_DIR].inode;
	// superblock.inode_table = fileDescriptorTable[FD_INODE_TABLE].inode;
	// superblock.free_blocks = fileDescriptorTable[FD_FREE_BLOCKS].inode;

	write_blocks(0, 1, superblock);
}


typedef struct compositeAddress {
	int block_num;
	int block_offset;
} cad;


// Converts virutal address in file to a combination of block number and block address
cad virtualAddresstoBlockAddress(void *ptr) {
	cad ca;

	ca.block_num = (int) ptr / block_size;
	ca.block_offset = (int) ptr % block_size;
}

// being generic over selective_write and selective_read
typedef int block_function(block_ptr block_num, int block_offset, void *buffer, int length);

int selective_write(block_ptr block_num, int block_offset, void *buffer, int length) {

	if(length == block_size) {
		return write_blocks(block_num, 1, buffer);
	}

	// Read contents of block into mycoolbuffer
	void *mycoolbuffer = (void*) malloc(block_size);
	read_blocks(block_num, 1, mycoolbuffer);

	// Fill selected portion of buffer
	for(int i = 0; i < length; i++) {
		mycoolbuffer[block_offset + i] = buffer[i];
	}

	int r = write_blocks(block_num, 1, mycoolbuffer);
	free(mycoolbuffer);
	return r;

}

int selective_read(block_ptr block_num, int block_offset, void *buffer, int length) {

	// Read contents of block into mycoolbuffer
	void *mycoolbuffer = (void*) malloc(block_size);
	read_blocks(block_num, 1, mycoolbuffer);

	// Selectively copy mycoolbuffer into buffer
	memcpy(buffer, mycoolbuffer + block_offset, length);

	return 1;
}

int apply_to_blocks(block_function f, int fileID, char *buf, int length) {

	struct FileDescriptor fd = fileDescriptorTable[fileID];
	
	cad caFrom = virtualAddressToBlockAddress(fd.write_ptr);
	cad caTo = virtualAddressToBlockAddress(fd.write_ptr + length);

	for(int block = caFrom.block_num; block <= caTo.block_num; block++) {

		block_ptr physical_block = inode_mark_virtual_block(fd.inode, block);

		if(block == caFrom.block_num) {
			f(physical_block, caFrom.block_offset, buf, block_size - block_offset);
			buf += block_size - block_offset;
		} else if(block == caTo.block_num) {
			f(physical_block, 0, buf, block_offset);
			buf += block_offset;
		} else {
			f(physical_block, 0, buf, block_size);
			buf += block_size;
		}
	}
}

int sfs_fwrite(int fileID, char *buf, int length) {
	return apply_to_blocks(selective_write, fileID, buf, length);
}

int sfs_fread(int fileID, char *buf, int length) {
	return apply_to_blocks(selective_read, fileID, buf, length);
}

// Remember to free the return pointer
char *sfs_fread_all(int fileID) {

	sfs_frseek(fileID, 0);

	struct FileDescriptor fd = fileDescriptorTable[fileID];
	char *buf = (char*) malloc(length);

	if(!sfs_fread(fildID, buf, fd.inode.size)) {
		return 0;
	} else {
		return buf;
	}
}

int sfs_fwseek(int fileID, int loc) {
	if(fileID > fileDescriptorTableLength) {
		return 0;
	}

	struct FileDescriptor fd = fileDescriptorTable[fileID];
	fd.write_ptr = loc;
	
	return 1;
}

int sfs_frseek(int fileID, int loc) {
	if(fileID > fileDescriptorTableLength) {
		return 0;
	}
	
	struct FileDescriptor fd = fileDescriptorTable[fileID];
	fd.read_ptr = loc;

	return 1;
}

inode_index lookup_file(char *file) {

	char *buf = sfs_fread_all(FD_ROOT_DIR);
	int length = fileDescriptorTable[FD_ROOT_DIR].inode.size;

	struct aa_array aa = cast_to_aa(buf, length);
	return aa_lookup(&aa, file);
}

int sfs_open(char *name) {

	inode_index ind = lookup_file(path);
	if(ind == -1) return -1;

	buf = sfs_fread_all(FD_INODE_TABLE);	
	length = fileDescriptorTable[FD_INODE_TABLE].inode.size;

	struct inode inode = inode_lookup_inode(buf, length, ind);

	// create file descriptor
	struct FileDescriptor fd;
	fd.inode = inode;
	fd.read_ptr = 0;
	fd.write_ptr = 0;

	// push to file descriptor table
	fileDescriptorTableLength++;
	fileDescriptorTable[fileDescriptorTableLength] = fd;

	return fileDescriptorTableLength;
}



inode_index file_enumerator = 0;

int sfs_getnextfilename(char *fname_buffer) {
	
	//// Not valid
	if(file_enumerator > aa_get_size(file)) {
		return 0;
	}
	//// Not valid

	
	buf = sfs_fread_all(FD_INODE_TABLE);	
	length = fileDescriptorTable[FD_INODE_TABLE].inode.size;

	return inode_lookup_inode(buff, length, file_enumerator++);

}

int sfs_getfilesize(const char* path) {

	inode_index ind = lookup_file(path);
	if(ind == -1) return -1;

	buf = sfs_fread_all(FD_INODE_TABLE);	
	length = fileDescriptorTable[FD_INODE_TABLE].inode.size;

	struct inode inode = inode_lookup_inode(buf, length, ind);
	return inode.size;

}

int sfs_fclose(int fileID) {

	if(fileID > fileDescriptorTableLength) {
		return 0;
	}

	for(int i = fileID; i < fileDescriptorTableLength - 1; i++) {
		fileDescriptorTable[i] = fileDescriptorTable[i + 1];
	}
	
	fileDescriptorTableLength -= 1;

	return 1;

}

int sfs_remove(char *file) {

	inode_index ind = lookup_file(file);
	if(ind == -1) return -1;

	buf = sfs_fread_all(FD_INODE_TABLE);	
	length = fileDescriptorTable[FD_INODE_TABLE].inode.size;

	return inode_remove_inode(buf, length, ind);

}
