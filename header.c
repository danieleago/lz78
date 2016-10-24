#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include "header.h"
#include "bit_io.h"
#include "lib_crc.h"

int write_header(char *input_file, bit_io* bit_output, unsigned int dictionary_size) {
	int i,ret;	
	uint64_t buffer;
	unsigned long header_crc = 0;	
	if(bit_output==NULL){
		printf("Error open file\n");			     
		return -1;
    }   

    //write the length of the name of the file
	uint32_t length_name = strlen(input_file);
	buffer = length_name;	
	ret = bit_write(bit_output, 32,buffer);
	if (ret ==-1) {
        printf("Error writing file name length\n");
        return -1;
    }
	header_crc = update_crc(header_crc,buffer,4);	

	//write the name char to char 
	for( i=0; i <  length_name ; i++){
		buffer = input_file[i];
		ret = bit_write(bit_output, 8,buffer);
        if (ret ==-1) {
           		printf("Error writing file name\n");
           		return -1;
       	}
		header_crc = update_crc(header_crc, buffer,1);
	}
	
	//write the size of the dictionary
    buffer = dictionary_size;   
	ret = bit_write(bit_output, 32,buffer);
    	if (ret ==-1) {
        	printf("Error writing dictionary size\n");
        	return -1;
    	}
   	header_crc =update_crc(header_crc, buffer, 4);

   	//write the size of the original file
	struct stat file_info;
	if(lstat(input_file,&file_info)<0){
		printf("error read size file\n");
		return -1;
	}
	buffer = file_info.st_size;  
	printf("size file: %llu\n", buffer);
	ret = bit_write(bit_output, 64, buffer);
    	if (ret ==-1) {
        	printf("Error writing file size\n");
        	return -1;
    	}
    header_crc =update_crc(header_crc, buffer, 8);
    
    //write the crc of the header
	buffer = header_crc;   
	printf("header crc: %llu\n", buffer);
	ret = bit_write(bit_output, 64, buffer);
    	if (ret ==-1) {
        	printf("Error writing crc header\n");
        	return -1;
    	}

	return 0;
}

int read_header(bit_io* bit_input, unsigned int* dictionary_size) {
	int i=0,ret;	
	uint32_t length_name;
	uint64_t buffer;
	unsigned long header_crc = 0;	


	ret = bit_read(bit_input, 32,&buffer);
        	if (ret != 32) {
            		printf("Error reading file name\n");
            		return -1;
        }
	header_crc = update_crc(header_crc,buffer,4);
	length_name = buffer;

	char* filename;
	filename =(char*) malloc(sizeof(char)*(length_name+1));
	memset(filename,0,sizeof(char)*(length_name+1));
	for(;i<length_name;i++){
		ret = bit_read(bit_input, 8,&buffer);
        	if (ret != 8) {
            		printf("Error reading file name\n");
            		return -1;
        	}
		filename[i] = (char) buffer;
		header_crc = update_crc(header_crc, buffer,1);
		
	}
	printf("original name file: %s \n", filename);


	ret = bit_read(bit_input, 32, &buffer);
    if (ret != 32) {
       	printf("Error reading dicionary size\n");
       	return -1;
   	}
   	header_crc =update_crc(header_crc, buffer, 4);
   	*dictionary_size = buffer;
	printf("dictionary size: %u\n", *dictionary_size);
   

	ret = bit_read(bit_input, 64, &buffer);
    if (ret != 64) {
        printf("Error writing file size \n");
       	return -1;
    }
    header_crc =update_crc(header_crc, buffer, 8);
	uint64_t file_size = buffer;
	printf("original file size: %llu\n", file_size);
	   
	ret = bit_read(bit_input, 64, &buffer);
    if (ret != 64) {
        printf("Error writing header crc\n");
        return -1;
    }
    unsigned long h_crc = buffer;
	printf("header crc read: %lu\n", h_crc);
	printf("header crc calculated: %lu \n", header_crc);
	if(h_crc==header_crc) printf("header crc correctly verify\n");

	bzero(filename,sizeof(char)*(length_name+1));
	free(filename);
	return file_size;
} 
