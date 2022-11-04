#include <iostream>
#include "App.h"

//
// DO NOT COMPILE. WILL NOT WORK.
//

void runApp(unsigned char* inputBuf, unsigned char* outputBuf ,int length)
{
    //
    // This is what CDC returns. 
    // Declared statically, since malloc adds to latency. 
    // NUM_ELEMENTS + HEADER : Max size of each packet, hence max size of chunks
    // CDC_OUTPUT_SIZE TO BE MODIFIED LATER
    //
    int arrayOfChunkIndices[CDC_OUTPUT_SIZE];

    //
    // RUN CDC
    //
    int chunkCount = runCDC(inputBuf, arrayOfChunkIndices, length);

    //
    // SHA table
    //
    build_SHA();

    //
    // De-Dup
    //

    run_DeDup();

    run_LZW();

}