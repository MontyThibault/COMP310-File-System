#include "structs.h"
#define MAX_FILENAME_LENGTH

struct aa_pair {
	char[MAX_FILENAME_LENGTH] filename;
	inode_index inode_ind;
}

struct aa_array {
	struct aa_pair array[];
	int max_length;
	int tail;
}

struct aa_array cast_to_aa(char *buff, int buff_length);
inode_index aa_lookup(struct aa_array *aa, char *filename);
int aa_insert(struct aa_array *aa, struct aa_pair pair);
int aa_remove(struct aa_array *aa, struct aa_pair pair);
