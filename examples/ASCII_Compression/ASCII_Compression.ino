//
// Demo code for uCompression
//
// (c)2024 by Lorandil
//
// This code is just a quick demonstration how to use the library and requires
// at least 2kB of RAM and a serial interface to run.
//
// The library itself can even be used on an ATTiny25 when selecting an appropriate chunk size!
//

const uint16_t BUFFER_SIZE = 600;

#include "uCompression.h"

// some simple test data
const uint8_t PROGMEM testData[] = 
  "****************************************\n"
  "*                                      *\n"
  "* Welcome to the uCompression library! *\n"
  "*                                      *\n"
  "*       *                   *******    *\n"
  "*      ***         ***      *     *    *\n"
  "*     *****      *******    * *** *    *\n"
  "*    *******    *********   * * * *    *\n"
  "*   *********    *******    * *** *    *\n" 
  "*      ***         ***      *     *    *\n"
  "*      ***                  *******    *\n"
  "*                                      *\n"
  "****************************************\n";

void setup()
{
  // wait a moment for the serial port to become ready
  delay( 5000 );
  // initialize serial transmission
  Serial.begin( 115200 );
}

void loop() 
{
  uint16_t uncompressedSize = sizeof( testData );

  // print the uncompressed data
  Serial.print(F("Uncompressed data (size = ")); Serial.print( uncompressedSize ); Serial.println(F(")"));
  Serial.print( (const __FlashStringHelper *) testData );

  // allocate memory for the compressed data
  uint8_t compressedData[BUFFER_SIZE];

  // compress the test data
  Serial.print(F("Compressing data... "));
  uint16_t compressedSize = uCompression::pgm_RLEcompress( testData,         // pointer to original data
                                                           uncompressedSize, // original size
                                                           compressedData ); // pointer to result buffer (compressed data)

  // print the compressed size
  Serial.print(F("size of compressed data is ")); Serial.print( compressedSize ); Serial.println(F(" bytes"));

  // allocate buffer for decompressed data (and fill it with '0' to ensure the test data is zero terminated)
  uint8_t uncompressedData[BUFFER_SIZE];
  memset( uncompressedData, 0, sizeof( uncompressedData ) );
  // uncompress the data 
  uCompression::RLEdecompress( compressedData,     // pointer to the compressed data
                               uncompressedData,   // pointer to result buffer (decompressed data)
                               uncompressedSize ); // original size

  // and print the decompressed data for referrence
  Serial.println(F("\nDecompressed data"));
  Serial.print( (const char *) uncompressedData );

  // compare decompressed data to original data
  if ( memcmp_P( uncompressedData, testData, uncompressedSize ) == 0 )
  {
    Serial.println(F("The data has been decompressed correctly!"));
  }
  else
  {
    Serial.println(F("*** The decompressed data is different from the original data! ***"));
  }

  while ( true );
}