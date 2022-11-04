int run_LZW (unsigned char input_buf[], int start_index, int end_index, unsigned char output[], int* output_ptr) {
    (*output_ptr)
    // Placeholder compression - simply store every other byte.
    for (int i = 0; i < end_index - start_index; i += 2) {
        output[(*output_ptr)++] = input_chunk[i];
    }
}