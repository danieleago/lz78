//the following functions read and write the header of the compressed file

int write_header(char *, bit_io*, unsigned int);
int read_header(bit_io*,unsigned int*);