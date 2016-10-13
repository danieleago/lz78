#include "lib_crc.h"
/*
 *   The CRC's are computed using polynomials. The  coefficients     
 *   for the algorithms are defined by the following constants.      
 */

#define                 P_32        0xEDB88320L

/*  
 *   The algorithms use tables with precalculated  values.  This     
 *   speeds  up  the calculation dramaticaly. The first time the     
 *   CRC function is called, the table for that specific  calcu-     
 *   lation  is set up. The ...init variables are used to deter-     
 *   mine if the initialization has taken place. The  calculated     
 *   values are stored in the crc_tab... arrays.                     
 */

static int              crc_tab32_init          = FALSE;
static unsigned long    crc_tab32[256];
static void             init_crc32_tab( void );

static void init_crc32_tab( void ) {
    int i, j;
    unsigned long crc;
    for (i=0; i<256; i++) {
        crc = (unsigned long) i;
        for (j=0; j<8; j++) {
            if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ P_32;
            else                     crc =   crc >> 1;
        }
        crc_tab32[i] = crc;
    }
    crc_tab32_init = TRUE;
} 


/*                                                             
 *   The function update_crc_32 calculates a  new  CRC-32  value     
 *   based  on  the  previous value of the CRC and the next byte     
 *   of the data to be checked.                                      
 */
static unsigned long update_crc_32( unsigned long crc, char c ) {

    unsigned long tmp, long_c;

    long_c = 0x000000ffL & (unsigned long) c;

    if ( ! crc_tab32_init ) init_crc32_tab();

    tmp = crc ^ long_c;
    crc = (crc >> 8) ^ crc_tab32[ tmp & 0xff ];

    return crc;

} 

unsigned long update_crc( unsigned long crc, char* buffer,unsigned int num ) {
	unsigned int i = 0;    
	unsigned long tmp = crc;
	while( i < num ){
		tmp= update_crc_32(tmp, buffer[i]);
		i++; 
    }
    return tmp;
}  

 


