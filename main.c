#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h> 
#include <stdlib.h>

#include "constant.h"
#include "bit_io.h"
//extern int make_compress();
//extern int make_decompress();

int option = 0;
int compress = -1;
int decompress = -1;
int dictionary_size = -1;
char* input_file= 0;
char* output_file= 0;
char* data_file = 0;

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
	/*	case 6 :	
			printf("Usage: lz78 [-c]||[-d] -l dict size -i input_file -o output_file\n");
		break;
	*/	default:	
			printf("default printf \n");
		break;
	}
}

	
void check_command_line(int num_arg, char *arg_value[])
{
	while ((option = getopt(num_arg, arg_value,"cdl:i:o:")) != -1) {
        switch (option) {
             case 'c' : compress= 0;
                 break;
             case 'd' : decompress = 0;
                 break;
             case 'l' : dictionary_size = atoi(optarg); 
                 break;
             case 'i' : input_file=malloc(strlen(optarg));
			strcpy(input_file,optarg);
                 break;
	     case 'o' : output_file=malloc(strlen(optarg));
			strcpy(output_file,optarg);
		 break;
             default: break;
        }
    }
    if ((compress == -1 && decompress ==-1)||(compress == 0 && decompress ==0))  {
        print_error( ERROR_PARAMITERS );
        exit(EXIT_FAILURE);
    }

    if(compress==0 && dictionary_size==-1){ 
		 print_error( ERROR_SIZE );
		 exit(EXIT_FAILURE);
	}
		
	if(decompress==0 && dictionary_size!=-1){
		print_error ( ERROR_DICT_NOT_REQUIRED);
		exit(EXIT_FAILURE);
	}
	
    if(input_file==0){
		print_error( ERROR_INPUT_FILE );
		exit(EXIT_FAILURE);
	}
		
    if(output_file==0){
		print_error( ERROR_OUTPUT_FILE );
		exit(EXIT_FAILURE);
	} 
	
	printf("ok check\n" );
	if (compress == 0) {
        printf("compress\n");
        printf("dictionary size: %d\n",dictionary_size);
    }
	
    if (decompress == 0) {
        printf("decompress\n");
    }

	printf("input file: %s\n",input_file);
	printf("output file: %s\n",output_file);
	
}


int main(int num_arg, char *arg_value[]) {
	int result = 0;
	
	check_command_line(num_arg,arg_value);
		
	if ( compress == 0){
		bit_io* bit_output = bit_open(output_file,1);
		result = write_header(input_file, bit_output, dictionary_size, 0);
		bit_io* bit_input = bit_open(input_file,0);
		compressor(bit_input, bit_output, dictionary_size);
		bit_close(bit_input);
		bit_close(bit_output);
	}
	
	if ( decompress == 0){
		bit_io* bit_input = bit_open(input_file,0);
		bit_io* bit_output = bit_open(output_file,1);
		unsigned int dictionary_size;
		read_header(bit_input,&dictionary_size);
		//decompressor(bit_input, bit_output, dictionary_size);
		bit_close(bit_input);
		bit_close(bit_output);
	}

    return result;
}
