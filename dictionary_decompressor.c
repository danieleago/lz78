
#include "dictionary_decompressor.h"

// max numeber of element in to dictionary
#define DEFAULT_MAX_NUMBER_OF_ELEMENTS 512

struct ENTRY
{
    unsigned char value; 		// current value
    unsigned int father; 		// All the elements with father = 0 are children of root , -1 is the father of root
    unsigned int index;  		// index = 0 is the root of dictionary
	unsigned int level; 		// number of level in tree
	UT_hash_handle hh;          // makes this structure hashable
};

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



static void add_entry( DICTIONARY* d , char value,unsigned int father_index , unsigned int level) {
    
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
	//printf("inserisco all'indice %d il valore %c con padre  %d \n",temp->index,value,temp->father);
	

}



static void init_dictionary (DICTIONARY* d){
	char c;
	ENTRY* temp;
	d->next_index =0;
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
	//printf("fine init \n");
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




/*
static void print_dictionary(DICTIONARY* d) {
    ENTRY* current_entry;
    ENTRY* tmp;
   
	HASH_ITER(hh, d->root, current_entry, tmp) {
		 printf("\n entry index %d: value %c father %d \n", current_entry->index, current_entry->value ,current_entry->father);
	}
	
		  printf("\n fine printf \n");
}
*/

/*ENTRY* find_entry (DICTIONARY* d,int index){
	ENTRY* tmp ;
	HASH_FIND_INT( d->root, &index, tmp );  
	return tmp;
}
*/

char* find_code(DICTIONARY* d,int index, int* num){
	ENTRY* tmp;
	ENTRY* oldtmp;
	char* ret;
	int n;
	int i;
	int flag = 0;
	ret = NULL;
	
		if(index == d->next_index){
			
			//printf("add 1\n");
			add_entry(d ,d->current_pointer->value, d->current_pointer->index ,d->current_pointer->level );
			flag=1;
		}
	
		HASH_FIND_INT( d->root, &index, tmp ); 
		if(tmp!=NULL)
		{
		
			//printf("find \n");
			*num = n = tmp->level;
		
			ret = (char*)malloc(sizeof(char)*(n+2)); 		// +1 end string
			memset(ret, 0,sizeof(char)*(n+2));
		
			//printf("n ==   %d \n",n);
			tmp = NULL;
			oldtmp = NULL;
			i= index;
		
			HASH_FIND_INT( d->root, &i, tmp );
			oldtmp = tmp;
			while(n>0)
			{	 
				n--;
				//printf("value   %c   father  %d  \n",tmp->value,tmp->father);
				//printf("value father  %c   father  %d  \n",oldtmp->value,oldtmp->father);
				ret[n]=tmp->value;
				i=tmp->father;
			
				if(tmp->father == 0 && d->current_pointer != d->root && flag==0)
				{
					//printf("aggiungo value %c   index current  %d  \n", tmp->value , d->current_pointer->index);
					
					//printf("add 2\n");
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

int decompressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){

	DICTIONARY* dictionary = new_dictionary(dictionary_size);
	init_dictionary(dictionary);
	int i;
	unsigned long crc=0;
	char* branch;
	int num;
	unsigned int index;
	uint64_t buffer;
	while(bit_read(bit_input,32,&buffer)>0){
		index = le32toh((uint32_t)buffer);
		//printf("index %u\n",(unsigned int) index);
		if(index==0) goto eof;
		if(dictionary->size < dictionary->next_index)
			reset_dictionary(dictionary);			

		branch = find_code(dictionary,index,&num);
		//printf("symbol write: ");
		for(i=0;i<num;i++){
			bit_write(bit_output,8, branch[i]);
			//printf("%c",(char) branch[i]);
			crc = update_crc(crc,&branch[i],1);
		}
		//printf("\n");
	}
	printf("error decompressor: index 0 not read\n");
	eof: printf("read EOF\n");
	bit_read(bit_input,64,&buffer);
	unsigned long crc_read = le64toh(buffer);
	printf("payload crc read: %lu\n", crc_read);
	printf("payload crc calculated: %lu \n", crc);
	if(crc_read==crc) printf("crc verified\n");
	else printf("crc not verified\n");

	return 0;
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
	
	
	//aaaaba
	
	
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
	
	
	
	//abcdaaaaaaaabcdgf
	
	
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
