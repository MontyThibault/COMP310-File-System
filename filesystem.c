#include "disk_emu.h"

#define MAX_FILE_DESCRIPTORS 10
struct fileDescriptor fileDescriptorTable[MAX_FILE_DESCRIPTORS];
int fileDescriptorTableLength = -1; // Refers to the last array index

int block_size = 1024;


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

char *sfs_fread_all(int fileID) {
	struct FileDescriptor fd = fileDescriptorTable[fileID];
	char *buf = (char*) malloc(length);

	if(!sfs_fread(fildID, buf, fd->length)) {
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
