# uCompression
 For now uCompression provides RLE compression and decompression functions for microcontrollers implemented as static member functions, 
 so no instance of the class is required.

## Performance
 As with most compression algorithms, the decompression rate depends on the original data.
 On a 16Mhz Arduiono Leonardo decompression rates between 700kB/s and 1000kB/s have been 
 observed when using the C++ code. For some AVR chips (ATmega328, ATmega32U4, ATTiny85) assembly code
 is automatically used, resulting in roughly the double decompression Speed.

 ## Limitiations
 Under worst case conditions the resulting size might slightly exceed the original data size (by 1 byte for every
 63 bytes of source data).

## Sample Data
The sample data was taken from TinyInvaders bei Daniel C and my TinyMinez project.