#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>
#include <unistd.h>

#include "bit_io.h"
#include "lib_crc.h"

int write_header(char *input_file, bit_io* bit_output, unsigned int dictionary_size) {
	int i,ret;	
	uint64_t buffer;
	unsigned long header_crc = 0;	
	//open() file input to get stat info
	int fd_input = open(input_file, O_RDONLY);
	if(fd_input<0){
		printf("Error open() function of input file: %s\n",strerror(errno));
		return -1;
	}
	if(bit_output==NULL){
		printf("Error open file\n");			     
		return -1;
    }   

    
	uint8_t length_name = strlen(input_file);
	//printf("length name: %d\n",length_name);	
	ret = bit_write(bit_output, 8,(uint64_t)length_name);
	if (ret ==-1) {
        printf("Error writing file name length: %s\n",strerror(errno));
        return -1;
    }
	header_crc = update_crc(header_crc,(char*)&length_name,1);	


	for( i=0; i <  length_name ; i++){
		//printf("char to white: %c\n",input_file[i]);	
		ret = bit_write(bit_output, 8,(uint64_t)input_file[i]);
        	if (ret ==-1) {
            		printf("Error writing file name: %s\n",strerror(errno));
            		return -1;
        	}
		header_crc = update_crc(header_crc, &input_file[i],1);
	}
	

    uint32_t buf_dict = htole32((uint32_t)dictionary_size);   
	//printf("dictionary size: %u\n",buf_dict);
	ret = bit_write(bit_output, 32,(uint64_t) buf_dict);
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
	close(fd_input);
	uint64_t buf_size = htole64((uint64_t)file_info.st_size);  
	printf("size file: %llu\n", buf_size);
	ret = bit_write(bit_output, 64, buf_size);
    	if (ret ==-1) {
        	printf("Error writing file size: %s\n",strerror(errno));
        	return -1;
    	}
    header_crc =update_crc(header_crc, (char*) &buf_size, 8);
    

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


	ret = bit_read(bit_input, 8,(uint64_t*)&length_name);
        	if (ret != 8) {
            		printf("Error reading file name\n");
            		return -1;
        }
	header_crc = update_crc(header_crc,(char*)&length_name,1);

	
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
		//printf("%llu\n",buffer);
		//printf("%c\n",filename[i]);
		header_crc = update_crc(header_crc, &filename[i],1);
		
	}
	printf("original name file: %s \n", filename);


	ret = bit_read(bit_input, 32, &buffer);
    if (ret != 32) {
		//printf("%llu\n",buffer);
       	printf("Error reading dicionary size: %s\n",strerror(errno));
       	return -1;
   	}
   	header_crc =update_crc(header_crc, (char*) &buffer, 4);
   	*dictionary_size = le32toh((uint32_t)buffer);
	printf("dictionary size: %u\n", *dictionary_size);
   

	ret = bit_read(bit_input, 64, &buffer);
    	if (ret != 64) {
        	printf("Error writing file size %s \n",strerror(errno));
        	return -1;
    	}
    header_crc =update_crc(header_crc, (char*) &buffer, 8);
	uint64_t file_size = le64toh(buffer);
	printf("original file size: %llu\n", file_size);
	   
	ret = bit_read(bit_input, 64, &buffer);
    if (ret != 64) {
        printf("Error writing file size\n");
        return -1;
    }
    unsigned long h_crc = le64toh(buffer);
	printf("header crc read: %lu\n", h_crc);
	printf("header crc calculated: %lu \n", header_crc);
	if(h_crc==header_crc) printf("header crc correctly verify\n");

	bzero(filename,sizeof(char)*(length_name+1));
	free(filename);
	return 0;
} 
