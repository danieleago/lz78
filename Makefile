CC = cc
CFLAGS = -c -Wall -O2
LIBS = -lm
main: main.o  lib_crc.o bit_io.o header.o  dictionary_compressor.o dictionary_decompressor.o 
	$(CC)  $^ -o $@ -o main $(LIBS)

main.o: constant.h header.h dictionary_decompressor.h dictionary_compressor.h bit_io.h
lib_crc.o: lib_crc.h 
header.o: header.h lib_crc.h bit_io.h
dictionary_decompressor.o: dictionary_decompressor.h lib_crc.h bit_io.h uthash.h
dictionary_compressor.o: dictionary_compressor.h lib_crc.h bit_io.h uthash.h
bit_io.o: bit_io.h 

clean:
	rm -rf *.o
