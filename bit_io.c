#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <inttypes.h>
#include "bit_io.h"

struct bitio{ 
	FILE * file_pointer;		
	uint64_t data;				
	u_int write_pointer;		
	u_int read_pointer;			
	u_int mode;     // 0 --> READ    1 --> WRITE
};

/* Creates a new bit_io opening a file f with the specificated mode */ 
bit_io* bit_open(const char * name, u_int mode){
	bit_io * temp;
	if(name == NULL || name[0]=='\0' || mode > 1)
	{
		errno = EINVAL;
		return NULL;
	}
	temp =(bit_io*) calloc (1,sizeof(bit_io)); 
	if(temp==NULL)
	{
		errno = ENOMEM;
		return NULL;	
	}
	temp->file_pointer = fopen( name , (mode == MODE_READ)? ACCESS_READ:ACCESS_WRITE);
	if(temp->file_pointer==NULL)
	{
		errno = ENOENT;  /* No such file or directory */
		free(temp);
		return NULL;	
	}
	temp->mode = mode;
	temp->write_pointer = 0;
	temp->read_pointer = 0;
	return temp;
	
}
 
 
/* Does a memory read */ 
int bit_read (bit_io* pointer, u_int size , uint64_t *data)
{
	int ret,space;
	if(pointer==NULL || pointer->mode!=MODE_READ || size>64)
	{
		errno = EINVAL;
		return -1;
	}
	*data =0;
	space = pointer->write_pointer - pointer->read_pointer; //remaining space in the buffer: pointer->data
	if(size==0) return 0;
	if(size <= space){ 
		//read from buffer and update the read_pointer
		*data = (pointer->data >> pointer->read_pointer) & ((1ULL<<size)-1);
		pointer->read_pointer += size;
	}else{
		//partial read from buffer and do a new read from the file
		*data = (pointer->data >> pointer->read_pointer) & ((1ULL<<space)-1);
		ret = fread((void*)& pointer->data, 1, 8, pointer->file_pointer); //read 8 objects of 1 byte
		pointer->data = le64toh(pointer->data);
		if(ret<0){
			errno = ENODATA;
			return -1;
		}
		pointer->write_pointer = ret * 8;
		if(pointer->write_pointer >= size-space){
			*data |= pointer->data << space;
			if(size<64) *data &= (1ULL << size) -1;
			pointer->read_pointer = size-space;
		}else{
			*data |= pointer->data << space;
			*data &= 1ULL << ((pointer->write_pointer + space) -1);
			pointer->read_pointer = pointer->write_pointer;
			return pointer->write_pointer + space;
		}
	}
	return size;
	
}
 
/* Does a memory write */ 
int bit_write(bit_io* pointer, u_int size , uint64_t data)
{	int ret;
	int space;
	if(pointer==NULL || pointer->mode!=MODE_WRITE || size > 64 )
	{
		errno = EINVAL;
		return -1;	
	}
	if(size==0)
		return 0;
	space = 64 - pointer->write_pointer;
	if(size<64) data &= (1ULL<<size)-1;
	if(space!=0) pointer->data |= data << pointer-> write_pointer;	
	if(size <= space )
	{	
		pointer->write_pointer += size;

	}
	else{
		pointer->data = htole64(pointer->data);
		ret=fwrite( (void*)&pointer->data,8,1,pointer->file_pointer);	
		if(ret==-1)
		{	
			printf("fwrite_err %d\n",ret);
			errno= ENOSPC;
			return -1;
		}		
		pointer->data = data >> space;
		pointer->write_pointer = size-space;
	}
	return 0;
}
 
/* Relases the resources allocated by the bit_io */ 
int bit_close(bit_io* pointer)
{
	int ret =0;
	if(pointer == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	if(pointer->mode == MODE_WRITE && pointer->write_pointer >0)
	{
		pointer->data = htole64(pointer->data);
		ret =fwrite( (void*)&pointer->data,1, (pointer->write_pointer+7) /8 ,pointer->file_pointer);		
		if(ret == -1)
		{
			errno = ENOSPC;			
			return -1;
		}
	}
	fclose(pointer->file_pointer);
	bzero(pointer, sizeof(*pointer));
	free(pointer); 
	return 0;
}



