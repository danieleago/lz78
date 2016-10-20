#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>
#include <math.h>

#include "uthash.h"
#include "bit_io.h"
#include "lib_crc.h"
#include "dictionary_compressor.h"

typedef struct key
{
	int    father;
	char   value;				// current value
}key;

typedef struct entry
{
   	unsigned int index;  		// index = 0 is the root of dictionary
	key mykey	;			        // for hashable
	UT_hash_handle hh;          // makes this structure hashable
} entry;

typedef struct dictionary
{
    entry* root ;				// pointer of root 
    entry* current_pointer ;	// pointer of search
    int size; 					// max number of element
    int next_index;				// next index for next entry
    int index_size; 			//number bits of index
} dictionary;



/**
 * dictionary * new_dictionary(int size)
 * It creates a new dictionary with a dimension specified by size
 * @param size The max number of elements which can be stored in the dictionary
 * @return The pointer to the dictionary
 */


static dictionary* new_dictionary(int size) {
    dictionary * d = (dictionary *)calloc(1, sizeof(dictionary));
    if (d == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return NULL;
    }
    d->size = size;
	d->next_index = 0;
	d->root=NULL;
	d->current_pointer=NULL;
	d->index_size = ceil(log2(size));
    return d;
}



static void add_entry( dictionary* d , char value, int father) {
    
	entry* temp;
	
	temp =(entry *)malloc(sizeof(entry));
	
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return;
	}
	memset(temp, 0, sizeof(entry));
    temp->index = d->next_index;
	d->next_index++;
	temp->mykey.value = value;
	temp->mykey.father = father;
    HASH_ADD(hh, d->root, mykey,sizeof(key), temp );  				//index is name of unique key 

}


static void init_dictionary (dictionary* d){
	char c;
	int i;
	d->next_index = 0;
	add_entry( d , EOF ,-1);
	d->current_pointer = d->root;
	for (i=0; i<=255; i++)
	{
		c =(char)i;
		add_entry( d , c ,0);
	}
}


static void destroy_dictionary(dictionary* d) {
 entry* current_entry;
 entry* tmp;
	HASH_ITER(hh, d->root, current_entry, tmp) {
		 HASH_DEL(d->root, current_entry);
     		 free(current_entry);
	}
}

static void reset_dictionary(dictionary* d) {
  destroy_dictionary(d);
  init_dictionary(d);
}


entry* find_entry (dictionary* d,char value) {

	entry* s;
	entry* temp;
	s = NULL;
	temp =(entry *)malloc(sizeof(entry));
	
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return NULL;
	}
	memset(temp, 0, sizeof(entry));
	
	temp->mykey.value = value;

	temp->mykey.father = d->current_pointer->index;

	HASH_FIND(hh, d->root, &(temp->mykey), sizeof(key), s);
	
    if (s!=NULL)
	{
		d->current_pointer = s;
	}
	else
	{
		d->current_pointer = d->root;
	}
	free(temp);
	return s;
}
 


int compressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){
	dictionary* mydictionary = new_dictionary(dictionary_size);
	entry* old_entry =NULL;
	entry* new_entry = NULL;
    init_dictionary(mydictionary);
	
	unsigned long crc=0;	
	uint64_t symbol;
	uint64_t buffer;
	while(bit_read(bit_input,8,&buffer)>0){
		crc = update_crc(crc,buffer,1);
		symbol = buffer;
		new_entry = find_entry(mydictionary, (char)symbol);
		if(new_entry==NULL){
			buffer = old_entry->index;
			bit_write(bit_output,mydictionary->index_size,buffer);
			if( mydictionary->size > mydictionary->next_index){
				add_entry(mydictionary,(char)symbol,old_entry->index);
			}else{
				reset_dictionary(mydictionary);
			}
			new_entry=find_entry(mydictionary,(char)symbol);
		}
		
		old_entry = new_entry;
	}
	buffer = old_entry->index;
	bit_write(bit_output,mydictionary->index_size,buffer);
	bit_write(bit_output,mydictionary->index_size,0);
	buffer = crc;
	bit_write(bit_output,64,buffer);
	printf("\n..compression terminated with success!\n");
	return 0;
}



