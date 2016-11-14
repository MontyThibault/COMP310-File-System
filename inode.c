#include "inode.h"
#include "structs.h"
#include "disk_emu.h"

// #include "bitmask.h"

block_ptr allocate_new_block() {
	return 4; // fair dice roll
}

block_ptr *inode_corrresponding_physical_block(struct inode *inode, int block_num) {

	// Filter if block_num > 12 + (int) (block_size / sizeof(int))

	if(block_num < 12) {
		return &inode->direct_block[block_num];
	}

	block_ptr *this_sick_buffer = (inode*) malloc(block_size);

	// Read the indirect block pointer
	read_blocks(inode->indirect_block, 1, this_sick_buffer);
	
	block_ptr bp = this_sick_buffer[block_num - 12]
	free(this_sick_buffer);

	return bp;
}


// Returns the physical block corresponding to the given virtual block
// of this inode, and allocates the block if necessary.
block_ptr inode_mark_virtual_block(struct inode *inode, int block_num) {
	
	inode->
	
}



