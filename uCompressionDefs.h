#pragma once

  // This file is used by the assembly code, too.
  // Don't add anything except #define!

  // extended compression mode
  #define RLE_UNCOMPRESSED_DATA     0x00
  #define RLE_COMPRESSED_0xFF_DATA  0xC0
  #define RLE_COMPRESSED_0x00_DATA  0x80
  #define RLE_COMPRESSED_DATA       0x40
  #define RLE_COMPRESSION_MASK      0xC0 
