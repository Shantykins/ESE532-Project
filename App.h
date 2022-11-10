#ifndef APP_H
#define APP_H

#include "Server/encoder.h"
//#include "Server/server.h"
#include "Server/stopwatch.h"
#include <iostream>

//
// Put all Parameters here
//

#define CDC_OUTPUT_SIZE         NUM_ELEMENTS + HEADER


// LZW
#define NUM_BUCKETS 1000
#define MAX_CHUNK_SIZE 8192

//
// Put Function Declarations Here
//

//
// Main Application
//
void runApp(unsigned char* inputBuf, unsigned char* outputBuf ,int length);

//
// CDC Function
//
int  runCDC(unsigned char* inputBuf, unsigned char* outputChunk, int length);
//
// LZW declaration
//
int run_LZW (unsigned char input_chunk[], int chunkSize, unsigned char output[], int output_ptr);

//
// SHA 
//
void SHA(unsigned char inputChunk[] ,int chunkSize, unsigned char SHAkey[])  ;

//
// De-duplication
//
int dedup(unsigned char SHAkey[]);


#endif
