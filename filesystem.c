#include "disk_emu.h"

#define MAX_FILE_DESCRIPTORS 10
struct fileDescriptor fileDescriptorTable[MAX_FILE_DESCRIPTORS];
int fileDescriptorTableLength = -1; // Refers to the last array index

int block_size;


void mksfs(int fresh) {
	if(fresh) {

	} else {

	}
}


typedef struct compositeAddress {
	int block_num;
	int block_offset;
} cad;


// Converts virutal address in file to a combination of block number and block address
cad fileAddresstoBlockAddress(void *ptr) {
	cad ca;

	ca.block_num = (int) ptr / block_size;
	ca.block_offset = (int) ptr % block_size;
}

int overwrite_block(block_ptr block_num, int block_offset, void *buffer, int length) {

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


int sfs_fwrite(int fileID, char *buf, int length) {
	struct FileDescriptor fd = fileDescriptorTable[fileID];
	
	cad caFrom = fileAddressToBlockAddress(fd.write_ptr);
	cad caTo = fileAddressToBlockAddress(fd.write_ptr + length);

	// This can be refactored into something that makes a at most thr
	for(int block = caFrom.block_num; block <= caTo.block_num; block++) {

		block_ptr physical_block = inode_mark_virtual_block(fd.inode, block);

		if(block == caFrom.block_num) {
			overwrite_block(block, caFrom.block_offset, buf, block_size - block_offset);
			buf += block_size - block_offset;
		} else if(block == caTo.block_num) {
			overwrite_block(block, 0, buf, block_offset);
			buf += block_offset;
		} else {
			overwrite_block(block, 0, buf, block_size);
			buf += block_size;
		}
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

int sfs_open(char *name) {
	return 0;

	// Gets the directory inode, performs a lookup, then initializes file descriptor, and returns the array index
}

int sfs_getnextfilename(char *fname_buffer) {

}

int sfs_getfilesize(const char* path) {

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
	return 0;
}
