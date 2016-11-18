#include "structs.h"
#include "disk_emu.h"

int fill_first_empty_bit_in_char(char *c); 
block_ptr allocate_new_block(); 
block_ptr *inode_corrresponding_physical_block(struct inode *inode, int block_num); 
block_ptr inode_mark_virtual_block(struct inode *inode, int block_num);
struct inode inode_lookup_inode(char *buf, int buf_length, inode_ind ind);
int inode_remove_inode(char *buf, int buf_length, inode_ind ind); 
