int run_LZW (unsigned char input_chunk[], int chunk_size, unsigned char output[]) {
    
    // Placeholder compression - simply store every other byte.
    for (int i = 0; i < chunk_size; i += 2) {
        output[i / 2] = input_chunk[i];
    }

}