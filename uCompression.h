#ifndef U_COMPRESSION_H
#define U_COMPRESSION_H

#include <Arduino.h>
#include "uCompressionDefs.h"

// max. compression block size (6 bits using only 1..63 for code size and speed)
#define RLE_MAX_COMPRESSION_BLOCK_LENGTH 63

// enable assembly code on special AVR chips
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  #define USE_ASSEMBLY_RLE_DECOMPRESS
#else
  #undef  USE_ASSEMBLY_RLE_DECOMPRESS
#endif

#ifdef USE_ASSEMBLY_RLE_DECOMPRESS
  extern "C" const uint8_t *extern_pgm_RLEdecompress256( const uint8_t *compressedData,
                                                         uint8_t *uncompressedData, uint8_t uncompressedByteCount );
#endif

class uCompression
{
public:
  uCompression() = default;
  static uint16_t RLEcompress( const uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                               uint8_t *compressedData, uint16_t compressedBufferSize );
  static uint16_t pgm_RLEcompress( const uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                                   uint8_t *compressedData, uint16_t compressedBufferSize );
  static const uint8_t *RLEdecompress( const uint8_t *compressedData, 
                                       uint8_t *uncompressedData, uint16_t uncompressedByteCount );
  static const uint8_t *RLEdecompress256( const uint8_t *compressedData, 
                                          uint8_t *uncompressedData, uint8_t uncompressedByteCount );
  static const uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                                           uint8_t *uncompressedData, uint16_t uncompressedByteCount );
  static const uint8_t *pgm_RLEdecompress256( const uint8_t *compressedData,
                                              uint8_t *uncompressedData, uint8_t uncompressedByteCount );
};

#endif