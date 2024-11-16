#if defined(__AVR_ATtiny85__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)

#include "uCompressionDefs.h"

      .file "uDecompression.S"
      .text
	  .global extern_pgm_RLEdecompress
      .global extern_pgm_RLEdecompress256

/*-------- GCC registers ---------*/
#define compressedData_H        R25
#define compressedData_L        R24
#define uncompressedData_H      R23
#define uncompressedData_L      R22
#define uncompressedByteCount_H R21
#define uncompressedByteCount_L R20
/*--------------------------------*/
#define value                   R25
#define count                   R24
#define encoding                R19
#define temp                    R18
/*--------------------------------*/
#define ZERO_REG                 R1

; Parameters in GCC convention
; R0 is free
; R1 := 0
; R18..R27, R30..R31 are free
; T flag is free
; uint8_t *extern_pgm_RLEdecompress256( uint8_t *compressedData,        R25:R24
;                                       uint8_t *uncompressedData,      R23:R22
;                                       uint8_t  uncompressedByteCount  R20
;                                     )
;
; Decompression algorithm for simple RLE encoding with up to 63 subsequent bytes
extern_pgm_RLEdecompress256:
    movw ZL,compressedData_L
    movw XL,uncompressedData_L

pgm_decomp256_do:                             // do{
       
    lpm   count,Z+                            // uint8_t count = pgm_read_byte( compressedData++ );

    mov   encoding,count                      // copy count to extract encoding bits
    andi  encoding,RLE_COMPRESSION_MASK       // encoding = ( count & RLE2_COMPRESSION_MASK )
    breq  pgm_decomp256_uncompressed_data     // zero -> encoding == RLE2_UNCOMPRESSED_DATA

    mov   value,ZERO_REG                      // value = 0x00
    cpi   encoding,RLE_COMPRESSED_0x00_DATA
    breq  pgm_decomp256_start
    dec   value                               // value = 0xFF
    brcc  pgm_decomp256_start
    lpm  value,Z+                             // value = pgm_read_byte( compressedData++ );
pgm_decomp256_start:
    sub   count,encoding                      // remove encoding bits from count
    sub   uncompressedByteCount_L,count       // uncompressedByteCount -= count;

pgm_decomp256_compressed_data_fill:           // do {
    st   X+,value                             //   *uncompressedData++ = value;
    subi count,1                              //   count--
    brne pgm_decomp256_compressed_data_fill   // } while ( count > 0 )
    
    rjmp pgm_decomp256_while
    
pgm_decomp256_uncompressed_data:
    sub  uncompressedByteCount_L,count        //    uncompressedByteCount -= count;
pgm_decomp256_uncompressed_data_fill:         // do {
    lpm  value,Z+                             //   value = pgm_read_byte( compressedData++ );
    st   X+,value                             //     *uncompressedData++ = value;
    subi count,1                              //   count--
    brne pgm_decomp256_uncompressed_data_fill // } while ( count > 0 )
    
pgm_decomp256_while:
    cpse uncompressedByteCount_L,ZERO_REG     //} while ( uncompressedByteCount != 0 )
    rjmp pgm_decomp256_do

    movw compressedData_L,ZL

    ret

#endif