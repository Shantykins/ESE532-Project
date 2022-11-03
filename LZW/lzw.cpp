int run_LZW (unsigned char [MAX_CHUNK_SIZE] input_chunk, int chunk_size, unsigned char [MAX_CHUNK_SIZE] output) {
    
    // Placeholder compression - simply store every other byte.
    for (int i = 0; i < chunk_size; i += 2) {
        output[i / 2] = input_chunk[i];
    }

}