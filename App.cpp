#include "App.h"

int runApp(unsigned char* inputBuf, unsigned char* outputBuf, int length, int* runtime, int* bytes_dropped)
{ 
    //
    // Output chunk array. Max chunk size = 8k or 8192
    //
    unsigned char outputChunk[MAX_CHUNK_SIZE];

    //
    // Variable to keep track of chunk progress. return from function 
    // if we cross the length
    //
    char digest[SHA256_DIGEST_SIZE + 1];
    int packetTracker = 0;
    static std::unordered_map<std::vector<char>, int, VectorHasher> shaMap;
    static int curr=0; 

    int lastChunkIdx = 0;

    stopwatch time_cdc;
    stopwatch time_sha;
    stopwatch time_dedup;
    stopwatch time_lzw;
    stopwatch total_time;

    // Pointer to keep track of current position in the output buffer
    int output_ptr = 0;

    while(packetTracker < length)
    {
        total_time.start();
        // RUN CDC
        time_cdc.start();
        int chunksize = runCDC(inputBuf, outputChunk, length, &lastChunkIdx);
        time_cdc.stop();

        packetTracker += chunksize - 1;

        //for (int i = 0; i < chunkCount; i++) {
        //int start_index = (i == 0) ? 0 : arrayOfChunkIndices[chunkCount - 1];
        //int end_index = arrayOfChunkIndices[chunkCount];
        // SHA table
        // SHA(outputChunk, chunksize, SHAkey);
        time_sha.start();
        SHA_new((char *) outputChunk,digest);
        time_sha.stop();
        digest[SHA256_DIGEST_SIZE] = '\0';

        // De-Dup
        time_dedup.start();
        int index = dedup_hash(digest,shaMap,curr);
        time_dedup.stop();

        if (index == -1) {
            // Chunk not found, run LZW
            curr++;
            unsigned char* tempBuf = (unsigned char*) malloc(sizeof(unsigned char) * (MAX_CHUNK_SIZE / 8) * 13);
            int count = 0;
            time_lzw.start();
            count = run_LZW(outputChunk, chunksize, tempBuf, count);
            time_lzw.stop();

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
            *bytes_dropped += (chunksize - 1) - 4;
            // Chunk found, simply write the index to the output in the header
            uint32_t header = index << 1;
            header = header | 0x01;
            outputBuf[(output_ptr)++] = header & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;
        }
        //}
        total_time.stop();
    }

    std::cout << "Total latency of CDC is: " << time_cdc.latency() << " ms." << std::endl;
    std::cout << "Total latency of SHA is: " << time_sha.latency() << " ms." << std::endl;
    std::cout << "Total latency of Dedup is: " << time_dedup.latency() << " ms." << std::endl;
    std::cout << "Total latency of LZW is: " << time_lzw.latency() << " ms." << std::endl;
    std::cout << "Total time taken by the loop is: " << total_time.latency() << " ms." << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << "Average latency of CDC per loop iteration is: " << time_cdc.avg_latency() << " ms." << std::endl;
    std::cout << "Average latency of SHA per loop iteration is: " << time_sha.avg_latency() << " ms." << std::endl;
    std::cout << "Average latency of Dedup per loop iteration is: " << time_dedup.avg_latency() << " ms." << std::endl;
    std::cout << "Average latency of LZW per loop iteration is: " << time_lzw.avg_latency() << " ms." << std::endl;
    std::cout << "Average latency of each loop iteration is: " << total_time.avg_latency() << " ms." << std::endl;
    *runtime += total_time.latency();
    return output_ptr;
}
