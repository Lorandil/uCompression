#include "uCompression.h"

/*--------------------------------------------------------------*/
uint16_t uCompression::RLEcompress( const uint8_t *uncompressedData,
                                    uint16_t uncompressedByteCount,
                                    uint8_t *compressedData )
{
  uint16_t compressedByteCount = 0;

  const uint8_t *uncompressedEnd = uncompressedData + uncompressedByteCount;

  while ( uncompressedData < uncompressedEnd )
  {
    // wait for a repeated value
    uint8_t diffCount = 0;
    while (    ( uncompressedData[diffCount] != uncompressedData[diffCount + 1] ) 
            && ( uncompressedData + diffCount < uncompressedEnd ) 
            && ( diffCount < RLE_MAX_COMPRESSION_BLOCK_LENGTH )
          )
    {
      diffCount++; 
    }
    
    // any different bytes?
    if ( diffCount != 0 )
    {
      // output pointer valid?
      if ( compressedData != nullptr )
      { 
        // store count
        *compressedData++ = diffCount;
        // copy bytes
        memcpy( compressedData, uncompressedData, diffCount );
        compressedData += diffCount;
      }
      // increase output count
      compressedByteCount += diffCount + 1;
      // skip processed bytes
      uncompressedData += diffCount;
    }
    else
    {
      // no repeated bytes until now
      uint8_t repeatCount = 0;
      
      // get a byte
      uint8_t value = *uncompressedData;

      // count repeated values
      while (    ( value == uncompressedData[repeatCount] ) 
              && ( uncompressedData + repeatCount < uncompressedEnd ) 
              && ( repeatCount < RLE_MAX_COMPRESSION_BLOCK_LENGTH )
            )
      {
        repeatCount++; 
      }
      // found some data?
      if ( repeatCount > 0 )
      {
        // output pointer valid?
        if ( compressedData != nullptr )
        { 
          // special case value = 0xff
          if ( value == 0xff )
          {
            // set special bitcode for '0xff'
            *compressedData++ = ( RLE_COMPRESSED_0xFF_DATA | repeatCount );
            // just one byte
            compressedByteCount += 1;
          }
          else if ( value == 0x00 )
          {
            // set special bitcode for '0x00'
            *compressedData++ = ( RLE_COMPRESSED_0x00_DATA | repeatCount );
            // just one byte
            compressedByteCount += 1;
          }
          else
          {
            // any other value
            *compressedData++ = ( RLE_COMPRESSED_DATA | repeatCount );
            // store value
            *compressedData++ = value;
            // just 2 bytes
            compressedByteCount += 2;
          }
        }
        // skip repeated bytes
        uncompressedData += repeatCount;
      }
    }
  }

  return( compressedByteCount );
}

/*--------------------------------------------------------------*/
uint16_t uCompression::pgm_RLEcompress( const uint8_t *uncompressedData,
                                        uint16_t uncompressedByteCount,
                                        uint8_t *compressedData )
{
  uint16_t compressedByteCount = 0;

  const uint8_t *uncompressedEnd = uncompressedData + uncompressedByteCount;

  while ( uncompressedData < uncompressedEnd )
  {
    // wait for a repeated value
    uint8_t diffCount = 0;
    while (    ( pgm_read_byte( uncompressedData + diffCount ) != pgm_read_byte( uncompressedData + diffCount + 1 ) ) 
            && ( uncompressedData + diffCount < uncompressedEnd ) 
            && ( diffCount < RLE_MAX_COMPRESSION_BLOCK_LENGTH )
          )
    {
      diffCount++; 
    }
    
    // any different bytes?
    if ( diffCount != 0 )
    {
      // output pointer valid?
      if ( compressedData != nullptr )
      { 
        // store count
        *compressedData++ = diffCount;
        // copy bytes from PROGMEM
        for ( int n = 0; n < diffCount; n++ )
        {
          *compressedData++ = pgm_read_byte( uncompressedData + n );
        }
      }
      // increase output count
      compressedByteCount += diffCount + 1;
      // skip processed bytes
      uncompressedData += diffCount;
    }
    else
    {
      // no repeated bytes until now
      uint8_t repeatCount = 0;
      
      // get a byte
      uint8_t value = pgm_read_byte( uncompressedData );

      // count repeated values
      while (    ( value == pgm_read_byte( uncompressedData + repeatCount ) ) 
              && ( uncompressedData + repeatCount < uncompressedEnd ) 
              && ( repeatCount < RLE_MAX_COMPRESSION_BLOCK_LENGTH )
            )
      {
        repeatCount++; 
      }
      // found some data?
      if ( repeatCount > 0 )
      {
        // output pointer valid?
        if ( compressedData != nullptr )
        { 
          // special case value = 0xff
          if ( value == 0xff )
          {
            // set special bitcode for '0xff'
            *compressedData++ = ( RLE_COMPRESSED_0xFF_DATA | repeatCount );
            // just one byte
            compressedByteCount += 1;
          }
          else if ( value == 0x00 )
          {
            // set special bitcode for '0x00'
            *compressedData++ = ( RLE_COMPRESSED_0x00_DATA | repeatCount );
            // just one byte
            compressedByteCount += 1;
          }
          else
          {
            // any other value
            *compressedData++ = ( RLE_COMPRESSED_DATA | repeatCount );
            // store value
            *compressedData++ = value;
            // just 2 bytes
            compressedByteCount += 2;
          }
        }
        // skip repeated bytes
        uncompressedData += repeatCount;
      }
    }
  }

  return( compressedByteCount );
}

/*--------------------------------------------------------------*/
// Decompressor for uncompressed data size of up to 64kB
// Input:  compressedData        - pointer to compressed data in RAM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
const uint8_t *uCompression::RLEdecompress( const uint8_t *compressedData, 
                                            uint8_t *uncompressedData,
                                            uint16_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = *compressedData++;

    uint8_t encoding = ( count & RLE_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = *compressedData++;
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE_COMPRESSED_0x00_DATA ) ? 0xFF
                                                              : ( encoding < RLE_COMPRESSED_0x00_DATA ) ? *compressedData++
                                                                                                        : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}

/*--------------------------------------------------------------*/
// Decompressor for uncompressed data size of up to 256 bytes
// Input:  compressedData        - pointer to compressed data in RAM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
const uint8_t *uCompression::RLEdecompress256( const uint8_t *compressedData, 
                                               uint8_t *uncompressedData,
                                               uint8_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = *compressedData++;

    uint8_t encoding = ( count & RLE_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = *compressedData++;
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE_COMPRESSED_0x00_DATA ) ? 0xFF
                                                              : ( encoding < RLE_COMPRESSED_0x00_DATA ) ? *compressedData++
                                                                                                        : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}

#ifndef USE_ASSEMBLY_RLE_DECOMPRESS
/*--------------------------------------------------------------*/
// Decompressor for extended encoding with up to 63 subsequent values
// Input:  pCompressedData       - pointer to compressed data in PROGMEM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
const uint8_t *uCompression::pgm_RLEdecompress( const uint8_t *compressedData,
                                                uint8_t *uncompressedData,
                                                uint16_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = pgm_read_byte( compressedData++ );

    uint8_t encoding = ( count & RLE_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = pgm_read_byte( compressedData++ );
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE_COMPRESSED_0x00_DATA ) ? 0xFF
                                                              : ( encoding < RLE_COMPRESSED_0x00_DATA ) ? pgm_read_byte( compressedData++ )
                                                                                                        : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}
#else
const uint8_t *uCompression::pgm_RLEdecompress( const uint8_t *compressedData,
                                                uint8_t *uncompressedData,
                                                uint16_t uncompressedByteCount )
{
  return( extern_pgm_RLEdecompress( compressedData, uncompressedData, uncompressedByteCount ) );
}
#endif


#ifndef USE_ASSEMBLY_RLE_DECOMPRESS
/*--------------------------------------------------------------*/
// Faster routine with max block size of 256 bytes (replaced by assembly routine on some AVRs)
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
const uint8_t *uCompression::pgm_RLEdecompress256( const uint8_t *compressedData,
                                                   uint8_t *uncompressedData,
                                                   uint8_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = pgm_read_byte( compressedData++ );

    uint8_t encoding = ( count & RLE_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = pgm_read_byte( compressedData++ );
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE_COMPRESSED_0x00_DATA ) ? 0xFF
                                                              : ( encoding < RLE_COMPRESSED_0x00_DATA ) ? pgm_read_byte( compressedData++ )
                                                                                                        : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}
#else
const uint8_t *uCompression::pgm_RLEdecompress256( const uint8_t *compressedData,
                                                  uint8_t *uncompressedData,
                                                  uint8_t uncompressedByteCount )
{
  return( extern_pgm_RLEdecompress256( compressedData, uncompressedData, uncompressedByteCount ) );
}
#endif