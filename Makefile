main: main.o  lib_crc.o bit_io.o header.o 
	gcc main.o lib_crc.o bit_io.o header.o -o main

main.o: main.c constant.h
	gcc -c main.c
lib_crc.o: lib_crc.c lib_crc.h 
	gcc -c lib_crc.c
header.o: header.c header.h bit_io.h 
	gcc -c header.c
bit_io.o: bit_io.c bit_io.h 
	gcc -c bit_io.c

