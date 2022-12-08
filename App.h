#ifndef APP_H
#define APP_H

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#define SHA256_DIGEST_SIZE 32

#include "Server/encoder.h"
//#include "Server/server.h"
#include "Server/stopwatch.h"
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <wolfssl/options.h>
//#include <wolfssl/wolfcrypt/sha256.h>
#include <vector>
#include <CL/cl2.hpp>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include "Utilities.h"

//
// Put all Parameters here
//

#define CDC_OUTPUT_SIZE         NUM_ELEMENTS + HEADER
#define LZW_HW_KER

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
int runApp(unsigned char* inputBuf, unsigned char* outputBuf ,int length, float* runtime, int* bytes_dropped, float* kernel_time, 
            float* cdc_time, float* sha_time, float* dedup_time, float* lzw_time, cl::CommandQueue q, cl::Kernel kernel_lzw, cl::Buffer in_buf, cl::Buffer out_buf, unsigned char* outputChunk, 
            unsigned char* tempbuf);

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
void SHA_new(char* message, uint64_t chunksize, char* digest);

//
// De-duplication
//
int dedup_hash(char shaSum[],std::unordered_map<std::vector<char>, int, VectorHasher> &shaMap, int curr);


#endif
