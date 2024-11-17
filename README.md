# uCompression
Flash memory is valuable resource, especially when using graphical displays on small micro controllers like an ATTiny85 or the ATMega328PB (Arduino UNO). This is where data compression might save the day!

**uCompression** provides fast RLE compression and decompression functions for micro controllers.
It's easy to use, as it is implemented as static member functions, so no instance of the class is required.

*The main design goals were code size and speed.*

## Compressing data

```javascript
uint16_t uCompression::RLEcompress( const uint8_t* uncompressedData,
                                    uint8_t*       compressedData,
                                    uint16_t       chunkSize );
```
or when compressing data already stored in the program memory:

```javascript
uint16_t uCompression::pgm_RLEcompress( const uint8_t* uncompressedData,
                                        uint8_t*       compressedData,
                                        uint16_t       chunkSize );
```

## Uncompressing data
If the compressed data is stored in RAM, it can be uncompressed by

```javascript
const uint8_t* uCompression::RLEdecompress( const uint8_t* compressedData,
                                            uint8_t*       uncompressedData,
                                            uint16_t       chunkSize );
```

If the compressed data is stored in the program memory, the data can be decompressed with

```javascript
const uint8_t* uCompression::pgm_RLEdecompress( const uint8_t* compressedData,
                                                uint8_t*       uncompressedData,
                                                uint16_t       chunkSize );
```

In the common case that the chunk size doesn't exceed 256 bytes, a slightly smaller and faster version can be used:

```javascript
const uint8_t* uCompression::pgm_RLEdecompress256( const uint8_t* compressedData,
                                                   uint8_t*       uncompressedData,
                                                   uint8_t chunkSize );
```


## Performance
 As with most compression algorithms, the decompression rate depends on the original data.
 On a Arduino Leonardo (ATmega32U4@16Mhz) decompression rates between 700kB/s and 1000kB/s have been 
 observed when using the C++ code. For some AVR chips (ATmega328, ATmega32U4, ATTiny85) assembly code
 is automatically used, resulting in roughly the double decompression Speed.

## Limitiations
The first and most important limitation is, that there is **NO error checking**!
If a target buffer is too small or the chunk sizes for encoding is larger than for decoding, the behaviour is undefined, most likely the MCU will crash or reboot.

RLE algorithms are specialized on efficiently compressing constant data areas. Random data cannot be compressed and will result in the compressed data being slightly larger than the original data.
 Under worst case conditions the resulting size will exceed the original data size (by 1 byte for every
 63 bytes of source data in a chunk).

## Sample Data
The sample data was taken from my updated version of TinyInvaders bei Daniel C and my TinyMinez project.