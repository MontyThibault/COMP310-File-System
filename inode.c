#include "inode.h"

// #include "bitmask.h"


// Modified the char pointer and returns the index of the bit
// which was modified, i.e. 128 = 0, 64 = 1, 32 = 2, 16 = 3...
int fill_first_empty_bit_in_char(char *c) {

	int n = 0,
	    m = *c,
	    s = 128;

	while(m >= s) {
		m -= s;
		s /= 2;
		n++;
	}

	*c += s;
	return n;
}


block_ptr allocate_new_block() {

	char *info = sfs_fread_all(FD_FREE_BLOCKS);
	int len = fileDescriptorTable[FD_FREE_BLOCKS]->length;

	// Search for first non-full char
	int i = 0;
	while(info[i] == 255) { 
		i++;
	}

	// Insert a bit
	int n = fill_first_empty_bit_in_char(&info[i]);

	return (i * 8) + n;
}


// Converts virtual block number (block_num) to physical block number, and 
// allocates the indirection block if necessary
block_ptr *inode_corrresponding_physical_block(struct inode *inode, int block_num) {

	if(block_num < 12) {
		return &inode->direct_block[block_num];
	}

	// Filter if block_num > 12 + (int) (block_size / sizeof(int))

	block_ptr *this_sick_buffer = (inode*) malloc(block_size);

	// Create indirection block 
	if(!inode->indirect_block) {
		inode->indirect_block = allocate_new_block();
	}

	// Read the indirect block pointer
	read_blocks(inode->indirect_block, 1, this_sick_buffer);
	
	block_ptr bp = &this_sick_buffer[block_num - 12]
	free(this_sick_buffer);

	return bp;
}


// Returns the physical block corresponding to the given virtual block
// of this inode, and allocates the block if necessary.
block_ptr inode_mark_virtual_block(struct inode *inode, int block_num) {

	block_ptr *bp = inode_corresponding_physical_block(inode, block_num);

	if((*bp) == 0) {
		(*bp) = allocate_new_block();
	}
	
	return *bp;
}


struct inode inode_lookup_inode(char *buf, int buf_length, inode_ind ind) {

}

int inode_remove_inode(char *buf, int buf_length, inode_ind ind) {

}


