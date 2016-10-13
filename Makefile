main: main.o  lib_crc.o bit_io.o header.o  dictionary_compressor.o dictionary_decompressor.o 
	gcc -Wall main.o lib_crc.o bit_io.o header.o dictionary_compressor.o dictionary_decompressor.o -o main

main.o: main.c 
	gcc -Wall  -c main.c
lib_crc.o: lib_crc.c lib_crc.h 
	gcc -Wall  -c lib_crc.c
header.o: header.c
	gcc -Wall  -c header.c
dictionary_decompressor.o: dictionary_decompressor.c dictionary_decompressor.h  
	gcc -Wall  -c dictionary_decompressor.c 
dictionary_compressor.o: dictionary_compressor.c dictionary_compressor.h  
	gcc -Wall  -c dictionary_compressor.c 
bit_io.o: bit_io.c bit_io.h 
	gcc -Wall  -c bit_io.c

clean:
	rm -rf *.o
