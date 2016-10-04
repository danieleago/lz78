#include "dictionary_decompressor.h"
#include "uthash.h"
#include <string.h>
#include <stdio.h>


// max numeber of element in to dictionary
#define DEFAULT_MAX_NUMBER_OF_ELEMENTS 512

typedef struct ENTRY
{
    unsigned char value; 		// current value
    int father; 				// All the elements with father = 0 are children of root , -1 is the father of root
    unsigned int index;  		// index = 0 is the root of dictionary
	UT_hash_handle hh;          // makes this structure hashable
}ENTRY;




struct DICTIONARY
{
    ENTRY* root ;				// pointer of root 
	ENTRY* current_pointer ;	// pointer of root 
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
	
    return d;
}



void add_entry( DICTIONARY* d , char value, int father_index) {
    
	ENTRY* temp;
	
	temp =(ENTRY *)malloc(sizeof(ENTRY));
	
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return;
	}
	memset(temp, 0, sizeof(ENTRY));
    temp->index = d->next_index;
	d->next_index++;
	temp->father= father_index;
	temp->value=value;
    HASH_ADD_INT( d->root,index, temp);  				//index is name of unique key 
	printf("inserisco all'indice %d il valore %c con padre  %d \n",temp->index,value,temp->father);
}



void init_dictionary (DICTIONARY* d){
	char c;
	add_entry( d , ' ' ,-1);
	d->current_pointer = d->root;
	for (int i=0; i<=255; i++)
	{
		//printf("\n iteration %d",i);
		c =(char)i;
		add_entry( d , c ,0);
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
    ENTRY* current_entry;
    ENTRY* tmp;
   
	HASH_ITER(hh, d->root, current_entry, tmp) {
		 printf("\n entry index %d: value %c father %d \n", current_entry->index, current_entry->value ,current_entry->father);
	}
	
		  printf("\n fine printf \n");
}


ENTRY* find_entry (DICTIONARY* d,int index){
	ENTRY* tmp ;
	HASH_FIND_INT( d->root, &index, tmp );  
	return tmp;
}


char* find_code(DICTIONARY* d,int index){
	ENTRY* tmp;
	ENTRY* oldtmp;
	char* ret;
	int n;
	int i;
	HASH_FIND_INT( d->root, &index, tmp ); 
	printf("find \n");
	if(tmp!=NULL)
	{
		
		n = 1;
		
		//--------------possibile miglioria
		
		while(tmp->father != 0)
		{
			oldtmp= tmp;
			HASH_FIND_INT( d->root, &oldtmp->father, tmp ); 
			n++;
		}
		
		ret = (char*)malloc(sizeof(char)*(n+1)); 		// +1 end string
		memset(ret, 0,sizeof(char)*(n+1));
		
		printf("value   %d \n",n);
		tmp = NULL;
		oldtmp = NULL;
		i= index;
		
		HASH_FIND_INT( d->root, &i, tmp );
		oldtmp = tmp;
		while(n>0)
		{	 
			n--;
			printf("value   %c \n",tmp->value);
			ret[n]=tmp->value;
			i=tmp->father;
			if(tmp->father == 0 && d->current_pointer!=d->root)
			{
				add_entry(d ,tmp->value, d->current_pointer->index);
			}
			HASH_FIND_INT( d->root, &i, tmp );
		}
		d->current_pointer = oldtmp;
	}
	return ret; 
}



int main ()
{
	char* string ;
	char msg[124];
	printf("start\n");
	DICTIONARY* d = new_dictionary( 500 );
	printf("create\n");
	init_dictionary(d);
	//add_entry(d ,'d', 103);
	//add_entry(d ,'r', 257);
	printf("init\n");
	/*
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,99);
	if(string)
		printf("value of index 99 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,100);
	if(string)
		printf("value of index 100 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,257);
	if(string)
		printf("value of index 257 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,100);
	if(string)
		printf("value of index 100 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,102);
	if(string)
		printf("value of index 102 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
		
	string = find_code(d,102);
	if(string)
		printf("value of index 262 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,103);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	*/
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	
	string = find_code(d,257);
	if(string)
		printf("value of index 257 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,99);
	if(string)
		printf("value of index 99 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	
		printf("messaggio: %s \n",msg);
		
		//print_dictionary(d);
	return 0;
}