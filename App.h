#ifndef APP_H
#define APP_H

#include "Server/encoder.h"
#include "Server/server.h"
#include "Server/stopwatch.h"

//
// Put all Parameters here
//

#define CDC_OUTPUT_SIZE         NUM_ELEMENTS + HEADER



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
int runCDC(unsigned char* inputBuf, unsigned int* arrayOfChunkIndices, int length);

//
// LZW declaration
//
void run_LZW(unsigned char input_buf[], int start_index, int end_index, unsigned char output[], int* output_ptr) ;

//
// SHA 
//
void SHA(unsigned char Input[] ,int CHUNK, unsigned char SHAkey[]) ;

//
// De-duplication
//
int dedup(unsigned char SHAkey[]);


#endif
