typedef int block_ptr;
typedef int inode_index;

struct inode {
	int mode;
	int lnk_cnt;
	int uid;
	int gid;
	int size;

	block_ptr direct_block[12];
	block_ptr indirect_block;
}


struct superblock {
	int magic;
	int block_size;
	int num_blocks;

	// Here we deviate slightly from the assignment specs.
	// Everything is stored in 3 inodes within the superblock.
	// Then all block trickiness/caching is handled generally.
	inode root_directory;
	inode inode_table;
	inode free_blocks;
}


// We need an in-memory file-descriptor table

struct file_descriptor {
	inode file;
	int read_ptr;
	int write_ptr;
}





//
//inode_table.h
//	-> getNextFilename
//	-> newINode
//
//inode_table.c
//
//
//
//inode-table inode
//    -> Read everything into buffer
//    -> Index as array
//
//    -> Could house optimizations
//
//
//
//free_bitmap.h
//	-> isBlockFree
//	-> getNextFreeBlock
//free_bitmap.c
//
//
//free_blocks inode
//    -> Does it span multiple blocks? (it could)
//
//
//
//associative directory table within root_directory
//
//associative_array.h - done
//
//
//
//Some sort of caching abstaction on inodes:
//
//directory cache
//i-node cache (load inode root_directory blocks into memory)
//disk block cache
//
//
//malloc_cache() -> cached_inode
//returns new inode with data blocks allocated in memory
