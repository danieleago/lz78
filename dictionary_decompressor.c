
#include "dictionary_decompressor.h"

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
		c =(char)i;
		add_entry( d , c ,temp->index , temp->level);
	}
}


static void destroy_dictionary(DICTIONARY* d) {
 ENTRY* current_entry;
    ENTRY* tmp;
   
	HASH_ITER(hh, d->root, current_entry, tmp) {
		HASH_DEL(d->root, current_entry);
		bzero(current_entry,sizeof(DICTIONARY));
    	free(current_entry);
	}
	
}


static void reset_dictionary(DICTIONARY* d) {
  destroy_dictionary(d);
  init_dictionary(d);
}


char* find_code(DICTIONARY* d,int index, int* num){
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
			*num = n = tmp->level;
		
			ret = (char*)malloc(sizeof(char)*(n+2)); 		// +1 end string
			memset(ret, 0,sizeof(char)*(n+2));
			tmp = NULL;
			oldtmp = NULL;
			i= index;
			HASH_FIND_INT( d->root, &i, tmp );
			oldtmp = tmp;
			while(n>0)
			{	 
				n--;
				ret[n]=tmp->value;
				i=tmp->father;
			
				if(tmp->father == 0 && d->current_pointer != d->root && flag==0)
				{
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
		if(index==0) goto eof;
		if(dictionary->size < dictionary->next_index)
			reset_dictionary(dictionary);			
		branch = find_code(dictionary,index,&num);
		for(i=0;i<num;i++){
			bit_write(bit_output,8, branch[i]);
			crc = update_crc(crc,&branch[i],1);
		}
		bzero(branch,sizeof(char)*(num+2));
		free(branch);
	}
	printf("error decompressor: index 0 not read\n");
	eof: printf("\n..Decompression terminated!\n");
	bit_read(bit_input,64,&buffer);
	unsigned long crc_read = le64toh(buffer);
	printf("payload crc read: %lu\n", crc_read);
	printf("payload crc calculated: %lu \n", crc);
	if(crc_read==crc) printf("crc verified!\n");
	else printf("crc not verified \n");
	bzero(dictionary,sizeof(DICTIONARY));
	free(dictionary);
	return 0;
}
/*

} */
