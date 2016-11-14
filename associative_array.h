#include "structs.h"
#define MAX_FILENAME_LENGTH

struct aa_pair {
	char[MAX_FILENAME_LENGTH] filename;
	inode_index inode_ind;
}

inode aa_lookup(char *filename);
int aa_insert(char *filename, inode_index inode_ind);
aa_pair *aa_enumerate();
