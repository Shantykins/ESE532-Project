#ifndef APP_H
#define APP_H

#include "Server/encoder.h"
//#include "Server/server.h"
#include "Server/stopwatch.h"
#include <iostream>
#include <unordered_map>
#include<string.h>
#include<stdio.h>
#include <stdlib.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha.h>
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
void SHA_new(char* message, char* digest);

//
// De-duplication
//
int dedup_hash(char shaSum[],std::unordered_map<std::string, int> &shaMap, int curr);


#endif
