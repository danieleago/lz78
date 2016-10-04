#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>


#include "constant.h"
#include "header.h"
#include "bit_io.h"

int write_header(char *input_file, bit_io* bit_output, unsigned int dictionary_size, unsigned long crc) {
	int i,ret;	
	unsigned long header_crc = 0;	
	int fd_input = open(input_file, O_RDONLY);
	if(bit_output==NULL){
		printf("Error open file\n");			     
		return -1;
        	}   
	uint8_t length_name = strlen(input_file) < 255 ? strlen(input_file) : 255;
	printf("length name: %d\n",length_name);	
	bit_write(bit_output, 8,(uint64_t)length_name);
	header_crc = update_crc(header_crc,&length_name,1);	
	for( i=0; i <  length_name ; i++){
		printf("char to white: %c\n",input_file[i]);	
		ret = bit_write(bit_output, 8,(uint64_t)input_file[i]);
        	if (ret ==-1) {
            		printf("Error writing file name: %s\n",strerror(errno));
            		return -1;
        	}
		header_crc = update_crc(header_crc, &input_file[i],1);
	}
	uint64_t buf_dict = htole64((uint64_t)dictionary_size);   
	printf("dictionary size: %llu\n",buf_dict);
	ret = bit_write(bit_output, 32, buf_dict);
    	if (ret ==-1) {
        	printf("Error writing dictionary size: %s\n",strerror(errno));
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buf_dict, 4);

	struct stat file_info;
	if(fstat(fd_input,&file_info)<0){
		printf("error read size file\n");
		return -1;
	}
	
	uint64_t buf_size = htole64((uint64_t)file_info.st_size);  
	printf("size file: %llu\n", buf_size);
	ret = bit_write(bit_output, 64, buf_size);
    	if (ret ==-1) {
        	printf("Error writing file size: %s\n",strerror(errno));
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buf_size, 8);

	uint64_t buffer = htole64((uint64_t)crc);   
	printf("crc payload ( we write 0 for the moment )\n");
	ret = bit_write(bit_output, 64, buffer);
    	if (ret ==-1) {
        	printf("Error writing crc payload: %s\n",strerror(errno));
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buffer, 8);

	buffer = htole64((uint64_t)header_crc);   
	printf("header crc: %llu\n", buffer);
	ret = bit_write(bit_output, 64, buffer);
    	if (ret ==-1) {
        	printf("Error writing crc header: %s\n",strerror(errno));
        	return -1;
    	}

	return 0;
}

int read_header(bit_io* bit_input, unsigned int* dictionary_size) {
	int i=0,ret;	
	uint8_t length_name;
	uint64_t buffer;
	unsigned long header_crc = 0;	
	char filename[255];    
	ret = bit_read(bit_input, 8,(uint64_t*)&length_name);
        	if (ret != 8) {
            		printf("Error reading file name\n");
            		return -1;
        }
	header_crc = update_crc(header_crc, &length_name,1);
	for(;i<length_name;i++){
		ret = bit_read(bit_input, 8,&buffer);
        	if (ret != 8) {
            		printf("Error reading file name\n");
            		return -1;
        	}
		filename[i] = (char) buffer;
		printf("%llu\n",buffer);
		printf("%c\n",filename[i]);
		header_crc = update_crc(header_crc, &filename[i],1);
	}
	filename[length_name]='\0';
	printf("original name file: %s \n", filename);
   	
	ret = bit_read(bit_input, 32, &buffer);
    	if (ret != 32) {
		printf("%llu\n",buffer);
		printf("%d\n",ret);
        	printf("Error reading dicionary size: %s\n",strerror(errno));
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buffer, 4);
	*dictionary_size = le64toh((uint64_t)buffer);
	printf("dictionary size: %u\n", *dictionary_size);
   
	ret = bit_read(bit_input, 64, &buffer);
    	if (ret != 64) {
        	printf("Error writing file size %s \n",strerror(errno));
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buffer, 8);
	uint64_t file_size = le64toh(buffer);
	printf("file size: %llu\n", file_size);
	   
	ret = bit_read(bit_input, 64, &buffer);
    	if (ret != 64) {
        	printf("Error writing file size\n");
        	return -1;
    	}
    	header_crc =update_crc(header_crc, (char*) &buffer, 8);
	uint64_t original_crc = htole64(buffer);
	printf("original crc: %llu\n", original_crc);
	   
	ret = bit_read(bit_input, 64, &buffer);
    	if (ret != 64) {
        	printf("Error writing file size\n");
        	return -1;
    	}
    	unsigned long h_crc = htole64(buffer);
	printf("header crc read: %lu\n", h_crc);
	printf("header crc calculated: %lu \n", header_crc);
	if(h_crc==header_crc) printf("header crc correctly verify\n");
	return 0;
} 
