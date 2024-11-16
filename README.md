# uCompression
uCompression provides RLE compression and decompression functions for microcontrollers implemented as static member functions, 
 so no instance of the class is required.

## Compressing data

```javascript
uint16_t uCompression::RLEcompress( const uint8_t *uncompressedData, uint8_t *compressedData, uint16_t chunkSize );
```
or

```javascript
uint16_t uCompression::pgm_RLEcompress( const uint8_t *uncompressedData, uint8_t *compressedData, uint16_t chunkSize );
```
if you need to compress data stored in the program memory (e.g. on AVR systems).


## Uncompressing data
If the compressed data is stored in RAM, it can be uncompressed by

```javascript
const uint8_t *uCompression::RLEdecompress( const uint8_t *compressedData, uint8_t *uncompressedData, uint16_t chunkSize );
```

If the compressed data is stored in the program memory, the data can be decompressed with

```javascript
const uint8_t *uCompression::pgm_RLEdecompress( const uint8_t *compressedData, uint8_t *uncompressedData, uint16_t chunkSize );
```

In the common case that the chunk size doesn't exceed 256 bytes, a slightly smaller and faster version can be used:

```javascript
const uint8_t *uCompression::pgm_RLEdecompress256( const uint8_t *compressedData, uint8_t *uncompressedData, uint8_t chunkSize );
```


## Performance
 As with most compression algorithms, the decompression rate depends on the original data.
 On a Arduino Leonardo (ATmega32U4@16Mhz) decompression rates between 700kB/s and 1000kB/s have been 
 observed when using the C++ code. For some AVR chips (ATmega328, ATmega32U4, ATTiny85) assembly code
 is automatically used, resulting in roughly the double decompression Speed.

## Limitiations
RLE algorithms are specialized on efficiently compressing constant data areas. Random data cannot be compressed and will result in the compressed data being slightly larger than the original data.
 Under worst case conditions the resulting size will exceed the original data size (by 1 byte for every
 63 bytes of source data in a chunk).

## Sample Data
The sample data was taken from my updated version of TinyInvaders bei Daniel C and my TinyMinez project.