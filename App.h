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
#include <wolfssl/wolfcrypt/sha256.h>
#include <vector>
//
// Put all Parameters here
//

#define CDC_OUTPUT_SIZE         NUM_ELEMENTS + HEADER


// LZW
#define NUM_BUCKETS 1000
#define MAX_CHUNK_SIZE 8192

struct VectorHasher {
    std::size_t operator()(const std::vector<char>& a) const {
        std::size_t h = 0;

        for (auto e : a) {
            h ^= std::hash<int>{}(e)  + 0x9e3779b9 + (h << 6) + (h >> 2); 
        }
        return h;
    }   
};

//
// Put Function Declarations Here
//

//
// Main Application
//
int runApp(unsigned char* inputBuf, unsigned char* outputBuf ,int length, int* runtime, int* bytes_dropped);

//
// CDC Function
//
int  runCDC(unsigned char* inputBuf, unsigned char* outputChunk, int length, int* last_chunk_idx);
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
int dedup_hash(char shaSum[],std::unordered_map<std::vector<char>, int, VectorHasher> &shaMap, int curr);


#endif
