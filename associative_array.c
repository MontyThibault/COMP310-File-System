#include "associative_array.c"
#include <string.h>

// What follows is an easy array implementation of an associative array.


// Convert from raw buffer data (extracted from an inode) into an 
// associative array struct.
struct aa_array cast_to_aa(char *buff, int buff_length, int buff_tail) {
	struct aa_array aa;

	aa.array = (struct aa_pair*) buff;
	aa.max_length = buff_length / sizeof(aa_pair);
	aa.tail = buff_tail / sizeof(aa_pair);
	
	return aa;
}


inode_index aa_lookup(struct aa_array *aa, char *filename) {
	
	for(int i = 0; i < aa->tail; i++) {
		struct aa_pair pair = aa->array[i];
		if(strcmp(pair.filename, filename) == 0) {
			return pair.inode_ind;
		}
	}

	return -1;
}


int aa_insert(struct aa_array *aa, struct aa_pair pair) {

	if(aa->tail + 1 == aa->length) {
		return 1;
	}

	aa->array[aa->tail + 1] = pair;
	aa->tail++;
	
	return 0;
}

int aa_remove(struct aa_array *aa, char *filename) {

	int index_of_entry = -1;

	for(int i = 0; i < aa->tail; i++) {

		if(index_of_entry != -1) {
			aa->array[i - 1] = aa->array[i];
		}

		struct aa_pair pair = aa->array[i];
		if(strcmp(pair.filename, filename) == 0) {
			index_of_entry = i;
		}
	}

	if(index_of_entry == -1) {
		return 1;
	} else {
		aa->tail--;
		return 0;
	}
}

