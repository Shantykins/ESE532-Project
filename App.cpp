#include <iostream>
#include "App.h"

//
// DO NOT COMPILE. WILL NOT WORK.
//

void runApp(unsigned char* inputBuf, unsigned char* outputBuf, int length)
{
    //
    // Output chunk array. Max chunk size = 8k or 8192
    //
    unsigned char outputChunk[8192];

    //
    // Variable to keep track of chunk progress. return from function 
    // if we cross the length
    //

    int packetTracker = 0;

    while(packetTracker < length)
    {

        // RUN CDC
        int chunksize = runCDC(inputBuf, outputChunk, length);

        packetTracker += chunksize;

        // Pointer to keep track of current position in the output buffer
        int output_ptr = 0;

        //for (int i = 0; i < chunkCount; i++) {
        //int start_index = (i == 0) ? 0 : arrayOfChunkIndices[chunkCount - 1];
        //int end_index = arrayOfChunkIndices[chunkCount];
        // SHA table
        unsigned char SHAkey[20];
        SHA(outputChunk, chunksize, SHAkey);

        // De-Dup
        int index = dedup(SHAkey);

        if (index == -1) {
            // Chunk not found, run LZW
            unsigned char* tempBuf = (unsigned char*) malloc(sizeof(unsigned char) * (MAX_CHUNK_SIZE / 8) * 13);
            int count = 0;
            count = run_LZW(outputChunk, chunksize, tempBuf, count);

            // Write the header to the output
            uint32_t header = count << 1;
            outputBuf[(output_ptr)++] = header & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;

            // Write the compressed chunk to the output
            for (int i = 0; i < count; i++) {
                outputBuf[(output_ptr)++] = tempBuf[i];
            }
        } else {
            // Chunk found, simply write the index to the output in the header
            uint32_t header = index << 1;
            header = header | 0x01;
            outputBuf[(output_ptr)++] = header & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;
        }
        //}

    }
}
