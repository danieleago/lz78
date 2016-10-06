main: main.o  lib_crc.o bit_io.o header.o   dictionary_decompressor.o decompressor.o 
	gcc main.o lib_crc.o bit_io.o header.o dictionary_decompressor.o decompressor.o -o main

main.o: main.c 
	gcc -c main.c
lib_crc.o: lib_crc.c lib_crc.h 
	gcc -c lib_crc.c
header.o: header.c header.h bit_io.h
	gcc -c header.c
decompressor.o: decompressor.c decompressor.h
	gcc -c decompressor.c
dictionary_decompressor.o: dictionary_decompressor.c dictionary_decompressor.h  
	gcc -c dictionary_decompressor.c 
/*compressor.o: compressor.c compressor.h  
	gcc -c compressor.c
dictionary_compressor.o: dictionary_compressor.c dictionary_compressor.h  
	gcc -c dictionary_compressor.c */
bit_io.o: bit_io.c bit_io.h 
	gcc -c bit_io.c

