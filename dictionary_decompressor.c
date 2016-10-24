#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>
#include <math.h>

#include "bit_io.h"
#include "lib_crc.h"
#include "dictionary_decompressor.h"

typedef struct entry
{
    char value; 				// current value
    unsigned int father; 	   		// index of father

} entry;

typedef struct dictionary
{
    entry* current_pointer ;			// pointer of current node
    entry* array ;				// array of entry 
    int size; 					// max number of element
    int index_size;				//number of bits of index
    int next_index;				// next index for next entry
    char* buffer;				//buffer to store the string found
} dictionary;

/**
 * dictionary * new_dictionary(int size)
 * It creates a new dictionary with a dimension specified by size
 */
 

static dictionary* new_dictionary(int size) {
	
    dictionary * d = (dictionary *)calloc(1, sizeof(dictionary));
    if (d == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return NULL;
    }
    d->size = size;
    d->next_index = 0;
    d->index_size = ceil(log2(size));
    d->current_pointer=NULL;
    d->buffer = (char*)malloc(sizeof(char)*(size));
    if (d->buffer  == NULL) {
    	free(d);
        printf("\n  Error: array allocation failed. \n");
        return NULL;
    }

    d->array = (entry *)malloc(sizeof(entry)*(size));

    if (d->array  == NULL) {
    	free(d->buffer);
    	free(d);
        printf("\n  Error: array allocation failed. \n");
        return NULL;
     }
	

    return d;
}



static void add_entry( dictionary* d , char value , unsigned int father) {
   
    d->array[d->next_index].value = value;
    d->array[d->next_index].father = father;
    d->next_index++;

}



static void init_dictionary (dictionary* d){
	
	char c;
	int i;

	d->next_index = 0;
    	add_entry( d ,EOF , -1);

	d->current_pointer = &( d->array[0]);
	
	for (i=0; i<=255; i++)
	{
		c =(char)i;
		add_entry( d , c , 0);
	}

}


static void reset_dictionary(dictionary* d) {

  memset(d->array,0,sizeof(entry)*d->size);
  init_dictionary(d);

}


char* find_code(dictionary* d,int index, int* num){

	entry temp;
	int n;
 	temp = d->array[index];
	n = d->size;

	if(d->size != d->next_index)					// if the next index is not equals to last index
		d->array[d->next_index].father = index; 	// add a new index with father the actual index 

	if (d->current_pointer!=&(d->array[0])){ 		// If current pointer is not the root
		while(temp.father!=0)
		{											// find the parent of first level of the actual index 
			temp = d->array[temp.father];		
		}
		d->current_pointer->value = temp.value; 	// add the parent found value to previous index  
	}

 	temp = d->array[index];

 	memset(d->buffer,0,sizeof(char)*(d->size));

	while(temp.father!= -1)
	{
		n--;
		d->buffer[n] = temp.value;
		temp = d->array[temp.father];
	}
	
	d->current_pointer = &(d->array[d->next_index]);
	d->next_index++;
	*num = (d->size - n );

	return &d->buffer[n]; 
}	

int decompressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){

	dictionary* mydictionary = new_dictionary(dictionary_size);
	init_dictionary(mydictionary);
	int i;
	unsigned long crc=0;
	char* branch;
	int num;
	unsigned int index;
	uint64_t buffer;
	unsigned long crc_read;
	while(bit_read(bit_input,mydictionary->index_size,&buffer)>0){
		index = buffer;
		if(index==0) 
			goto eof;
		if(mydictionary->size < mydictionary->next_index)
			reset_dictionary(mydictionary);			
		branch = find_code(mydictionary,index,&num);
		for(i=0;i<num;i++){
			bit_write(bit_output,8, branch[i]);
			crc = update_crc(crc,branch[i],1);
		}
	
	}
	printf("error decompressor: index 0 not read\n");
	
	eof: printf("\n..Decompression terminated!\n");
	bit_read(bit_input,64,&buffer);
	crc_read = buffer;
	printf("payload crc read: %lu\n", crc_read);
	printf("payload crc calculated: %lu \n", crc);
	if(crc_read==crc)
		printf("crc verified!\n");
	else 
		printf("crc not verified \n");
	
	if(mydictionary!=NULL){
		free(mydictionary->array);
		free(mydictionary->buffer);
		bzero(mydictionary,sizeof(dictionary));
		free(mydictionary);
	}
	return 0;
}

