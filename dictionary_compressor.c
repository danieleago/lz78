
#include "dictionary_compressor.h"

struct KEY
{
	 ENTRY* pointer_father;
	 char   value;				// current value
};

struct ENTRY
{
    	unsigned int father; 		// All the elements with father = 0 are children of root
   	unsigned int index;  		// index = 0 is the root of dictionary
	KEY key	;			// for hashable
	UT_hash_handle hh;          	// makes this structure hashable
};

struct DICTIONARY
{
    ENTRY* root ;				// pointer of root 
    ENTRY* current_pointer ;	// pointer of search
    int size; 					// max number of element
    int next_index;				// next index for next entry
};

/**
 * DICTIONARY * new_dictionary(int size)
 * It creates a new dictionary with a dimension specified by size
 * @param size The max number of elements which can be stored in the dictionary
 * @return The pointer to the dictionary
 */
 
static DICTIONARY* new_dictionary(int size) {
    DICTIONARY * d = (DICTIONARY *)calloc(1, sizeof(DICTIONARY));
    if (d == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return NULL;
    }
    d->size = size;
	d->next_index = 0;
	d->root=NULL;
	d->current_pointer=NULL;
    return d;
}


unsigned int index_entry(ENTRY* entry){
	return entry->index;
}


char value_entry(ENTRY* entry){
	return entry->key.value;
}


static void add_entry( DICTIONARY* d , char value, ENTRY* pointer_father) {
    
	ENTRY* temp;
	
	temp =(ENTRY *)malloc(sizeof(ENTRY));
	
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return;
	}
	memset(temp, 0, sizeof(ENTRY));
    temp->index = d->next_index;
	d->next_index++;
	temp->key.value = value;
	temp->key.pointer_father = pointer_father;
	if(pointer_father!=NULL)
		temp->father= pointer_father->index;
	else
		temp->father=0;
    HASH_ADD(hh, d->root, key,sizeof(KEY), temp );  				//index is name of unique key 
}


static void init_dictionary (DICTIONARY* d){
	char c;
	int i;
	d->next_index =0;
	add_entry( d , ' ' ,d->root);
	d->current_pointer = d->root;
	for (i=0; i<=255; i++)
	{
		c =(char)i;
		add_entry( d , c ,d->current_pointer);
	}
}


static void destroy_dictionary(DICTIONARY* d) {
 ENTRY* current_entry;
 ENTRY* tmp;
	HASH_ITER(hh, d->root, current_entry, tmp) {
		 HASH_DEL(d->root, current_entry);
     		 free(current_entry);
	}
}

static void reset_dictionary(DICTIONARY* d) {
  destroy_dictionary(d);
  init_dictionary(d);
}


ENTRY* find_entry (DICTIONARY* d,char value) {
	ENTRY* s;
	ENTRY temp;
	memset(&temp, 0, sizeof(ENTRY));
	temp.key.value = value;
	temp.key.pointer_father = d->current_pointer;
	HASH_FIND(hh, d->root, &temp.key, sizeof(KEY), s);
	
    if (s!=NULL)
	{
		d->current_pointer = s;
	}
	else
	{
		d->current_pointer = d->root;
	}
	return s;
}

int compressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){
	DICTIONARY* dictionary = new_dictionary(dictionary_size);
	init_dictionary(dictionary);
	ENTRY* entry=NULL;
	ENTRY* new_entry = NULL;
	unsigned long crc=0;	
	uint64_t symbol;
	uint64_t buffer;
	while(bit_read(bit_input,8,&symbol)>0){
		crc = update_crc(crc,(char*)&symbol,1);
		new_entry = find_entry(dictionary, (char)symbol);
		if(new_entry==NULL){
			buffer = htole32(index_entry(entry));
			bit_write(bit_output,32, buffer );
			if( dictionary->size >=  dictionary->next_index){
				add_entry(dictionary,(char)symbol,entry);
			}else{
				reset_dictionary(dictionary);
			}
			new_entry=find_entry(dictionary,(char)symbol);
		}
		entry = new_entry;
	}
	buffer = htole32(index_entry(entry));
	bit_write(bit_output,32,buffer);
	bit_write(bit_output,32,0);
	buffer = htole64(crc);
	bit_write(bit_output,64,buffer);
	printf("\n..compression terminated with success!\n");
	return 0;
}



