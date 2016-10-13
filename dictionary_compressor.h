#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stdint.h>

#include "uthash.h"
#include "bit_io.h"
#include "lib_crc.h"

typedef struct DICTIONARY DICTIONARY;
typedef struct ENTRY ENTRY;
typedef struct KEY KEY;
