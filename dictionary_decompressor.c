#include "dictionary.h"
#include "uthash.h"
#include <string.h>
#include <stdio.h>


// max numeber of element in to dictionary
#define DEFAULT_MAX_NUMBER_OF_ELEMENTS 512

typedef struct ENTRY
{
    unsigned char value; 		// current value
    unsigned int father; 		// All the elements with father = 0 are children of root
    unsigned int index;  		// index = 0 is the root of dictionary
	UT_hash_handle hh;          // makes this structure hashable
}ENTRY;




struct DICTIONARY
{
    ENTRY* root ;				// pointer of root 
	ENTRY* current_pointer ;				// pointer of root 
    int size; 					// max number of element
    int next_index;				// next index for next entry
};

/**
 * DICTIONARY * new_dictionary(int size)
 * It creates a new dictionary with a dimension specified by size
 * @param size The max number of elements which can be stored in the dictionary
 * @return The pointer to the dictionary
 */
 
DICTIONARY* new_dictionary(int size) {
    DICTIONARY * d = (DICTIONARY *)calloc(1, sizeof(DICTIONARY));
    if (d == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return NULL;
    }
    d->size = size;
	d->next_index = 0;
	d->root=NULL;
	d->current_pointer=NULL;
	
    /*d->root = (ENTRY *)calloc(1, sizeof(ENTRY));
    if (d->root == NULL) {
        printf("\n Error: dictionary allocation failed.\n");
        free(d);
        return NULL;
    }
	d->root->index =d->next_index;
	*/
	d->next_index++;
    return d;
}


void add_entry( DICTIONARY* d , char value , int father) {
    
	ENTRY* temp =(ENTRY *)calloc(1, sizeof(ENTRY));
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return;
	}
    temp->index = d->next_index;
	d->next_index++;
	temp->value = value;
	temp->father = father;
    HASH_ADD_INT( d->root, index , temp );  				//index is name of unique key 
	
}

void init_dictionary (DICTIONARY* d){
	char c;
	for (int i=0; i<=255; i++)
	{
		//printf("\n iteration %d",i);
		c =(char)i;
		add_entry( d , c , 0);
	}
}



/**
 * Clear table
 * @param table The table to clear
 */
void
clear_dictionary(DICTIONARY* d) {
  
}

/**
 * This function resets the hash table, deallocates it and its entries
 */
void
destroy_dictionary(DICTIONARY* d) {
  
}
void print_dictionary(DICTIONARY* d) {
    ENTRY* s;
	int i;
   /* for(s = d->root; s != NULL; s=s->hh.next) {
        printf("\n entry index %d: value %s\n", s->index, s->value);
    }*/
	for (i = 1; i<=270; i++)
	{
	
	 HASH_FIND_INT(d->root, &i, s);

		 if (s!=NULL)
		 {
		 printf("\n entry index %d: value %c\n", s->index, s->value);
		 }
		
	}
	
		  printf("\n fine printf \n");
}


int main ()
{
	printf("start\n");
	DICTIONARY* d = new_dictionary( 500 );
	printf("create\n");
	init_dictionary(d);
	printf("init\n");
	print_dictionary(d);
	return 0;
}