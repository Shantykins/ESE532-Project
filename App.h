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
void runCDC(unsigned char* inputBuf, unsigned int* outputBuf ,int length);


#endif