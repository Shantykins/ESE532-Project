#include "App.h"

int runApp(unsigned char* inputBuf, unsigned char* outputBuf, int length, int* runtime, int* bytes_dropped,
        cl::CommandQueue q, cl::Kernel kernel_lzw, cl::Buffer in_buf, cl::Buffer out_buf, unsigned char* outputChunk, 
        unsigned char* tempbuf)
{ 

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

    std::vector<cl::Event> write_event(1);
    std::vector<cl::Event> compute_event(1);
    std::vector<cl::Event> done_event(1);

    // Pointer to keep track of current position in the output buffer
    int output_ptr = 0;

    // Variables to store OpenCL profiling info
    unsigned long start, stop;
    unsigned long total_kernel_execution_time = 0;

    while(packetTracker < length)
    {
        total_time.start();
        // RUN CDC
        time_cdc.start();
        int chunksize = runCDC(inputBuf, outputChunk, length, &lastChunkIdx);
        time_cdc.stop();

        packetTracker += chunksize - 1;

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
            //unsigned char* tempBuf = (unsigned char*) malloc(sizeof(unsigned char) * (MAX_CHUNK_SIZE / 8) * 13);

            time_lzw.start();
            
            kernel_lzw.setArg(0, in_buf);
            kernel_lzw.setArg(1, chunksize);
            kernel_lzw.setArg(2, out_buf);

            //count = run_LZW(outputChunk, chunksize, tempBuf, count);

            q.enqueueMigrateMemObjects({in_buf}, 0 /* 0 means from host*/, NULL, &write_event[0]);
            q.enqueueTask(kernel_lzw, &write_event, &compute_event[0]);
            q.enqueueMigrateMemObjects({out_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &compute_event, &done_event[0]);
            clWaitForEvents(1, (const cl_event *)&done_event[0]);
            time_lzw.stop();

            compute_event[0].getProfilingInfo<unsigned long>(CL_PROFILING_COMMAND_START, &start);
            compute_event[0].getProfilingInfo<unsigned long>(CL_PROFILING_COMMAND_END, &stop);

            total_kernel_execution_time += (stop - start);

            int count = (tempbuf[3] << 24) | (tempbuf[2] << 16) | (tempbuf[1] << 8) | tempbuf[0];

            // Write the header to the output
            uint32_t header = count << 1;
            outputBuf[(output_ptr)++] = header & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
            outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;

            // Write the compressed chunk to the output
            for (int i = 0; i < count; i++) {
                outputBuf[(output_ptr)++] = tempbuf[i + 4];
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

    q.finish();
    delete[] fileBuf;

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
    std::cout << "Total Kernel Execution Time: " << total_kernel_execution_time / 1000000 << " ms." << std::endl;
    *runtime += total_time.latency();
    *kernel_time += total_kernel_execution_time;
    *non_lzw += time_cdc.latency() + time_sha.latency() + time_dedup.latency();
    return output_ptr;
}
