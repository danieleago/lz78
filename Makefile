main: main.o  lib_crc.o bit_io.o header.o  dictionary_compressor.o compressor.o
	gcc main.o lib_crc.o bit_io.o header.o dictionary_compressor.o compressor.o -o main

main.o: main.c 
	gcc -c main.c
lib_crc.o: lib_crc.c lib_crc.h 
	gcc -c lib_crc.c
header.o: header.c header.h bit_io.h
	gcc -c header.c
compressor.o: compressor.c compressor.h  
	gcc -c compressor.c
dictionary_compressor.o: dictionary_compressor.c dictionary_compressor.h  
	gcc -c dictionary_compressor.c
bit_io.o: bit_io.c bit_io.h 
	gcc -c bit_io.c

