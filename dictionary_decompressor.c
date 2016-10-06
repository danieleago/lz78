#include "dictionary_decompressor.h"
#include "uthash.h"
#include <string.h>
#include <stdio.h>


// max numeber of element in to dictionary
#define DEFAULT_MAX_NUMBER_OF_ELEMENTS 512

typedef struct ENTRY
{
    unsigned char value; 		// current value
    unsigned int father; 		// All the elements with father = 0 are children of root , -1 is the father of root
    unsigned int index;  		// index = 0 is the root of dictionary
	unsigned int level; 		// number of level in tree
	UT_hash_handle hh;          // makes this structure hashable
}ENTRY;




struct DICTIONARY
{
    ENTRY* root ;				// pointer of root 
	ENTRY* current_pointer ;	// pointer of current node 
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



void add_entry( DICTIONARY* d , char value,unsigned int father_index , unsigned int level) {
    
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
	temp->value = value;
	temp->level = level + 1 ;
    HASH_ADD_INT( d->root,index, temp);  				//index is name of unique key 
	printf("inserisco all'indice %d il valore %c con padre  %d \n",temp->index,value,temp->father);
}



void init_dictionary (DICTIONARY* d){
	char c;
	ENTRY* temp;
	temp =(ENTRY *)malloc(sizeof(ENTRY));
	
	if (temp == NULL) {
        printf("\n  Error: dictionary allocation failed. \n");
        return;
	}
	memset(temp, 0, sizeof(ENTRY));
    temp->index = d->next_index;
	d->next_index++;
	temp->father= 0;
	temp->value=' ';
	temp->level=0;
	
    HASH_ADD_INT( d->root,index, temp);
	
	d->current_pointer = d->root;
	int i;
	for (i=0; i<=255; i++)
	{
		//printf("\n iteration %d",i);
		c =(char)i;
		add_entry( d , c ,temp->index , temp->level);
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
	int flag = 0;
	ret = NULL;
	if(index == d->next_index){
		add_entry(d ,d->current_pointer->value, d->current_pointer->index ,d->current_pointer->level );
		flag=1;
	}
	
	HASH_FIND_INT( d->root, &index, tmp ); 
	if(tmp!=NULL)
	{
		
		printf("find \n");
		n = tmp->level;
		
		ret = (char*)malloc(sizeof(char)*(n+1)); 		// +1 end string
		memset(ret, 0,sizeof(char)*(n+1));
		
		printf("n ==   %d \n",n);
		tmp = NULL;
		oldtmp = NULL;
		i= index;
		
		HASH_FIND_INT( d->root, &i, tmp );
		oldtmp = tmp;

		while(n>0)
		{	 
			n--;
			printf("value   %c   father  %d  \n",tmp->value,tmp->father);
			printf("value father  %c   father  %d  \n",oldtmp->value,oldtmp->father);
			ret[n]=tmp->value;
			i=tmp->father;
			
			if(tmp->father == 0 && d->current_pointer != d->root && flag==0)
			{
				printf("aggiungo value %c   index current  %d  \n", tmp->value , d->current_pointer->index);
				add_entry(d ,tmp->value, d->current_pointer->index ,d->current_pointer->level );
			}
			HASH_FIND_INT( d->root, &i, tmp );
		}
		
		d->current_pointer = oldtmp;
		
		
	}
	else
	{
			printf(" not find \n");
	}
	return ret; 
}


/*
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
	
	//abcabceeef
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
		
	string = find_code(d,262);
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
	
	//aaaaba
	
	/*
	printf("1 \n\n");
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	
	printf("2 \n\n");
	string = find_code(d,257);
	if(string)
		printf("value of index 257 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	printf("3 \n\n");
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	
	printf("4 \n\n");
	string = find_code(d,99);
	if(string)
		printf("value of index 99 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	printf("5 \n\n");
	string = find_code(d,98);
	if(string)
		printf("value of index 98 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	*/
	
	
	//abcdaaaaaaaabcdgf
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
	
	string = find_code(d,101);
	if(string)
		printf("value of index 257 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,98);
	if(string)
		printf("value of index 100 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,261);
	if(string)
		printf("value of index 102 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
		
	string = find_code(d,262);
	if(string)
		printf("value of index 262 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,261);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,258);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,101);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,104);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
	string = find_code(d,103);
	if(string)
		printf("value of index 103 %s \n",string);
	else 
		printf("string null \n");
	strcat(msg, string);
	
		
		
	printf("messaggio: %s \n",msg);
		
		//print_dictionary(d);
	return 0;
} */