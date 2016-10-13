#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>

#include "bit_io.h"
#include "lib_crc.h"
#include "uthash.h"


typedef struct DICTIONARY DICTIONARY;
typedef struct ENTRY ENTRY;
