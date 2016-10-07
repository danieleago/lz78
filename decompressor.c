#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>


#include "constant.h"
#include "bit_io.h"
#include "dictionary_decompressor.h"
#include "lib_crc.h"

int decompressor(bit_io* bit_input, bit_io* bit_output, unsigned int dictionary_size){

	DICTIONARY* dictionary = new_dictionary(dictionary_size);
	init_dictionary(dictionary);
	int i;
	unsigned long crc=0;
	char* branch;
	unsigned int index;
	uint64_t buffer;
	while(bit_read(bit_input,32,&buffer)>0){
		index = le32toh((uint32_t)buffer);
		printf("index %u\n",(unsigned int) index);
		if(index==0) goto eof;
		branch = find_code(dictionary,index);
		for(i=0;i<strlen(branch);i++){
			bit_write(bit_output,8, branch[i]);
			printf("symbol write %c\n",(char) branch[i]);
			crc = update_crc(crc,&branch[i],1);
		}
	}
	printf("error decompressor: index 0 not read\n");
	eof: printf("read EOF\n");
	bit_read(bit_input,64,&buffer);
	unsigned long crc_read = le64toh(buffer);
	if(crc_read==crc) printf("crc verified\n");
	else printf("crc not verified\n");
	return 0;
}
