/*
JPEGDecoder.h

JPEG Decoder for Arduino
Public domain, Makoto Kurauchi <http://yushakobo.jp>

Adapted by Bodmer for use with a TFT screen

Latest version here:
https://github.com/Bodmer/JPEGDecoder

*/

#ifndef JPEGDECODER_H
  #define JPEGDECODER_H

  #include "Arduino.h"

  #ifdef __AVR__
    #include <avr/pgmspace.h>
    #undef PROGMEM
    #define PROGMEM  __attribute__((section(".fini2")))
  #endif

  #if defined (ESP8266) || defined (ESP32)

    //#include "arduino.h"
    #include <pgmspace.h>

// If the sketch has included FS.h without setting FS_NO_GLOBALS first then it is likely
// there will be a redefinition of 'class fs::File' error due to conflict with the
// SD library, so we can't load the SD library. (At 12/1/18 this appears to be fixed)
    //#if !defined (FS_NO_GLOBALS) && defined (FS_H)
      //#undef LOAD_SD_LIBRARY
      //#undef LOAD_SDFAT_LIBRARY
    //#endif

    #ifdef ESP32  // SDFAT library not compatible with ESP32
      //#undef LOAD_SD_LIBRARY
      #undef LOAD_SDFAT_LIBRARY
    #endif

    #define LOAD_SPIFFS
    #define FS_NO_GLOBALS
    #include <FS.h>

    #ifdef ESP32
      #include "SPIFFS.h" // ESP32 only
    #endif

  #endif

  #if defined (LOAD_SD_LIBRARY) || defined (LOAD_SDFAT_LIBRARY)
    #ifdef LOAD_SDFAT_LIBRARY
      #include <SdFat.h> // Alternative where we might need to bit bash the SPI
    #else
      #include <SD.h>    // Default
    #endif
  #endif


#include "picojpeg.h"

enum {
  JPEG_ARRAY = 0,
  JPEG_FS_FILE,
  JPEG_SD_FILE,
  JPEG_STREAM
};

//#define DEBUG

//------------------------------------------------------------------------------
#ifndef jpg_min
  #define jpg_min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;
//------------------------------------------------------------------------------

class JPEGDecoder {

private:
#if defined (LOAD_SD_LIBRARY) || defined (LOAD_SDFAT_LIBRARY)
  File g_pInFileSd;
#endif
#ifdef LOAD_SPIFFS
  fs::File g_pInFileFs;
#endif
  Stream *g_pStream;

  pjpeg_scan_type_t scan_type;
  pjpeg_image_info_t image_info;

  int is_available;
  int mcu_x;
  int mcu_y;
  uint g_nInFileSize;
  uint g_nInFileOfs;
  uint row_pitch;
  uint decoded_width, decoded_height;
  uint row_blocks_per_mcu, col_blocks_per_mcu;
  uint8 status;
  uint8 jpg_source = 0;
  uint8_t* jpg_data;

  static uint8 pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
  uint8 pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
  int decode_mcu(void);
  int decodeCommon(void);
public:

  uint16_t *pImage;
  JPEGDecoder *thisPtr;


  int width;
  int height;
  int comps;
  int MCUSPerRow;
  int MCUSPerCol;
  pjpeg_scan_type_t scanType;
  int MCUWidth;
  int MCUHeight;
  uint16_t outWidth;
  uint16_t outHeight;
  int MCUx;
  int MCUy;

  JPEGDecoder();
  ~JPEGDecoder();

  int available(void);
  int read(void);
  int readSwappedBytes(void);

  int decodeFile (const char *pFilename);
  int decodeFile (const String& pFilename);

#if defined (LOAD_SD_LIBRARY) || defined (LOAD_SDFAT_LIBRARY)
  int decodeSdFile (const char *pFilename);
  int decodeSdFile (const String& pFilename);
  int decodeSdFile (File g_pInFile);
#endif

#ifdef LOAD_SPIFFS
  int decodeFsFile (const char *pFilename);
  int decodeFsFile (const String& pFilename);
  int decodeFsFile (fs::File g_pInFile);
#endif

  int decodeFsFile( fs::FS &fs, const char* pFilename );

  int decodeStream( Stream *dataSource, uint32_t array_size );

  int decodeArray(const uint8_t array[], uint32_t  array_size);
  void abort(void);

};

extern JPEGDecoder JpegDec;

#endif // JPEGDECODER_H