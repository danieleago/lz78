#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>


#include "constant.h"
#include "bit_io.h"
#include "dictionary_compressor.h"

int compressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){

	DICTIONARY* dictionary = new_dictionary(dictionary_size);
	init_dictionary(dictionary);
	ENTRY* entry=NULL;
	ENTRY* new_entry = NULL;
	
	uint64_t symbol;
	uint64_t buffer;
	while(bit_read(bit_input,8,&symbol)>0){
		printf("symbol %c\n",(char)symbol);
		new_entry = find_entry(dictionary, (char)symbol);
		if(new_entry==NULL){
			buffer = (int64_t) index_entry(entry);
			bit_write(bit_output,sizeof(unsigned int), buffer );
			printf("index write %u\n",(unsigned int)buffer);
			add_entry(dictionary,(char)symbol,entry);
			new_entry=find_entry(dictionary,(char)symbol);
		}
		entry = new_entry;
	}
	buffer = value_entry(entry);
	bit_write(bit_output,sizeof(unsigned int),buffer);
	printf("index write last symbol %u\n",(unsigned int)buffer);
	bit_write(bit_output,sizeof(unsigned int),0);
	printf("write EOF\n");
}



