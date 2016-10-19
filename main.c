#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <stdlib.h>

#include "constant.h"
#include "bit_io.h"
#include "header.h"

extern int compressor(bit_io*, bit_io*,unsigned int);
extern int decompressor(bit_io*, bit_io*, unsigned int);

typedef struct datainput{
	int option;
	int compress;
	int decompress;
	int dictionary_size;
	char* input_file;
	char* output_file;
	char* data_file;
} data_input;

void datainput_init(data_input* d){
	d->option = 0;
	d->compress = -1;
	d->decompress = -1;
	d->dictionary_size = -1;
	d->input_file= NULL;
	d->output_file= NULL;
	d->data_file = 0;
}
// funzione complessiva per la stampa degli errori
void print_error(int tipo) {
	printf("\nError:\n");
    switch (tipo)
	{
		case ERROR_PARAMITERS :	
			printf("Usage: lz78 [-c]||[-d] -l dict size -i input_file -o output_file\n");
		break;
		case ERROR_SIZE :	
			printf("miss -l option \n");
		break;
		case ERROR_DICT_NOT_REQUIRED :
			printf("dictionary size not required during decompression\n");
		break;
		case ERROR_INPUT_FILE :		
			printf("miss -i option\n");
		break;
		case ERROR_OUTPUT_FILE :	
			printf("miss -o option\n");
		break;
		case ERROR_OPEN :	
			printf("NULL File descriptor\n");
		break;
		default:	
			printf("default printf \n");
		break;
	}
}

	
void check_command_line(int num_arg, char *arg_value[], data_input* d)
{
	while ((d->option = getopt(num_arg, arg_value,"cdl:i:o:")) != -1) {
        switch (d->option) {
            case 'c' : d->compress= 0;
       		    break;
            case 'd' : d->decompress = 0;
                break;
            case 'l' : d->dictionary_size = atoi(optarg); 
                break;
            case 'i' : d->input_file = optarg;
                break;
	     	case 'o' : d->output_file = optarg;
		 		break;
            default: break;
        }
    }
    if ((d->compress == -1 && d->decompress ==-1)||(d->compress == 0 && d->decompress ==0))  {
        print_error( ERROR_PARAMITERS );
        exit(EXIT_FAILURE);
    }

    if(d->compress==0 && d->dictionary_size==-1){ 
		 print_error( ERROR_SIZE );
		 exit(EXIT_FAILURE);
	}
		
	if(d->decompress==0 && d->dictionary_size!=-1){
		print_error ( ERROR_DICT_NOT_REQUIRED);
		exit(EXIT_FAILURE);
	}
	
    if(d->input_file==NULL){
		print_error( ERROR_INPUT_FILE );
		exit(EXIT_FAILURE);
	}
		
    if(d->output_file==NULL){
		print_error( ERROR_OUTPUT_FILE );
		exit(EXIT_FAILURE);
	} 
	
	printf("\n\n" );
	if (d->compress == 0) {
        printf("Start compression..\n\n");
        printf("dictionary size: %d\n",d->dictionary_size);
    }
	
    if (d->decompress == 0) {
        printf("Start decompression..\n\n");
    }

	printf("input file: %s\n",d->input_file);
	printf("output file: %s\n",d->output_file);
	
}

uint64_t file_size(char* file){
	struct stat file_info;
	if(lstat(file,&file_info)<0){
		printf("error read size file\n");
		return -1;
	}
	return file_info.st_size;
}

int main(int num_arg, char *arg_value[]) {
	data_input* d = (data_input*)calloc(1,sizeof(data_input));
	datainput_init(d);
	check_command_line(num_arg,arg_value,d);
		
	if ( d->compress == 0){
		bit_io* bit_output = bit_open(d->output_file,1);
		write_header(d->input_file, bit_output, d->dictionary_size);
		bit_io* bit_input = bit_open(d->input_file,0);
		compressor(bit_input, bit_output, d->dictionary_size);
		bit_close(bit_input);
		bit_close(bit_output);
	}
	
	if ( d->decompress == 0){
		bit_io* bit_input = bit_open(d->input_file,0);
		bit_io* bit_output = bit_open(d->output_file,1);
		unsigned int dictionary_size;
		uint64_t filesize;
		filesize = read_header(bit_input,&dictionary_size);
		decompressor(bit_input, bit_output, dictionary_size);
		bit_close(bit_input);
		bit_close(bit_output);
		if(filesize==file_size(d->output_file)) printf("size of decompressed file verified\n");
		else printf("size of decompressed file verified\n");
	}

    return 0;
}
