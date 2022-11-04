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
    unsigned int arrayOfChunkIndices[CDC_OUTPUT_SIZE];

    // RUN CDC
    int chunkCount = runCDC(inputBuf, arrayOfChunkIndices, length);

    // Pointer to keep track of current position in the output buffer
    int output_ptr = 0;

    for (int i = 0; i < chunkCount; i++) {
        int start_index = (i == 0) ? 0 : arrayOfChunkIndices[chunkCount - 1];
        int end_index = arrayOfChunkIndices[chunkCount];
        // SHA table
        unsigned char SHAkey[20];
        SHA(inputBuf, start_index, end_index, SHAkey);

        // De-Dup
        int index = dedup(SHAkey);

        if (index == -1) {
            // Chunk not found, run LZW and write the compressed chunk to the output
            run_LZW(inputBuf, start_index, end_index, outputBuf, &output_ptr);
        } else {
            // Chunk found, simply write the index to the output
            outputBuf[(output_ptr)++] = (index >> 24) & 0xFF;
            outputBuf[(output_ptr)++] = (index >> 16) & 0xFF;
            outputBuf[(output_ptr)++] = (index >> 8) & 0xFF;
            outputBuf[(output_ptr)++] = index & 0xFF;
        }
    }
}
