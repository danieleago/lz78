

/* Access mode for reading and writing */ 
#define ACCESS_READ "rb"
#define ACCESS_WRITE "wb" 
#define MODE_READ 0
#define MODE_WRITE 1 
 
typedef struct bitio { 
	FILE * file_pointer;		
	uint64_t data;				
	u_int write_pointer;		
	u_int read_pointer;			
	u_int mode;     // 0 --> READ    1 --> WRITE
} bit_io;

 
/* Creates a new bit_io opening a file f with the specificated mode */ 
bit_io* bit_open(const char * name, u_int mode); 
 
 
/* Does a memory read */ 
int bit_read (bit_io* pointer, u_int size , uint64_t *data); 
 
 
/* Does a memory write*/ 
int bit_write(bit_io* pointer, u_int size , uint64_t data); 

 
/* Relases the resources allocated by the bit_io */ 
int bit_close(bit_io* pointer); 
