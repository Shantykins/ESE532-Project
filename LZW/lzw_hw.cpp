#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define LZW_NUM_BUCKETS 16381
#define LZW_BUCKET_DEPTH 4

int output_code (int code, unsigned char state, unsigned char output[], int* output_ptr) {
    code = code & 0x1FFF;
    switch (state) {
        case 0:
            // Output: 8 | 5 _ _ _
            code = code << 3;
            output[(*output_ptr)++] = (unsigned char) ((code >> 8) & 0xFF);
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 1;
            break;
        case 1:
            // Output: _ _ _ _ _ 3 | 8 | 2 _ _ _ _ _ _
            code = code << 6;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 16) & 0x07));
            output[(*output_ptr)++] = (unsigned char) ((code >> 8) & 0xFF);
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 2;
            break;
        case 2:
            // Output: _ _ 6 | 7 _
            code = code << 1;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 8) & 0x3F));
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 3;
            break;
        case 3:
            // Output: _ _ _ _ _ _ _ 1 | 8 | 4 _ _ _ _
            code = code << 4;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 16) & 0x01));
            output[(*output_ptr)++] = (unsigned char) ((code >> 8) & 0xFF);
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 4;
            break;
        case 4:
            // Output: _ _ _ _ 4 | 8 | 1 _ _ _ _ _ _ _
            code = code << 7;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 16) & 0x0F));
            output[(*output_ptr)++] = (unsigned char) ((code >> 8) & 0xFF);
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 5;
            break;
        case 5:
            // Output: _ 7 | 6 _ _
            code = code << 2;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 8) & 0x7F));
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 6;
            break;
        case 6:
            // Output: _ _ _ _ _ _ 2 | 8 | 3 _ _ _ _ _
            code = code << 5;
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 16) & 0x03));
            output[(*output_ptr)++] = (unsigned char) ((code >> 8) & 0xFF);
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 7;
            break;
        case 7:
            // Output: _ 5 | 8
            output[(*output_ptr) - 1] = (unsigned char) (output[(*output_ptr) - 1] | ((code >> 8) & 0x1F));
            output[(*output_ptr)++] = (unsigned char) (code & 0xFF);
            state = 0;
            break;
    }
    return state;
}

void put(uint64_t hash_table[][LZW_BUCKET_DEPTH], uint16_t curr, uint8_t input, uint16_t next) {
    uint64_t key = (curr << 8) | input;
    int hash = key % LZW_NUM_BUCKETS;
    key = (key << 13) | (next & 0x1FFF);
    for (char i = 0; i < LZW_BUCKET_DEPTH; i++) {
        if (hash_table[hash][i] == (uint64_t) 0) {
            hash_table[hash][i] = key;
            return;
        }
    }
}

uint16_t get(uint64_t hash_table[][LZW_BUCKET_DEPTH], uint16_t curr, uint8_t input) {
    uint64_t key = (curr << 8) | input;
    int hash = key % LZW_NUM_BUCKETS;
    for (char i = 0; i < LZW_BUCKET_DEPTH; i++) {
        if ((hash_table[hash][i] >> 13) == key) {
            return (hash_table[hash][i] & 0x1FFF);
        }
    }
    return 0x1FFF;
}

int run_LZW (unsigned char input_chunk[], int chunkSize, unsigned char output[], int output_ptr) {
    // Key = 13b current character + 8b next character
    // Value = 13b new character, or -1 if none exists
    // Total = 34b per entry
    uint64_t hash_table [LZW_NUM_BUCKETS][LZW_BUCKET_DEPTH];
    for (int i = 0; i < LZW_NUM_BUCKETS; i++) {
        for (int j = 0; j < LZW_BUCKET_DEPTH; j++) {
            hash_table[i][j] = 0;
        }
    }
    int num;
    int index;

    // Insert the first 256 entries into the dictionary
    for (num = 0; num < 256; num++) {
        put(hash_table, 0x1FFF, num, num); // Encode the current character as -1, next as itself
    }

    // Setup memory for string processing
    uint16_t prev_str = 0x1FFF;
    uint8_t input_char = input_chunk[0];
    uint8_t next_char = 0;
    uint16_t curr_str = get(hash_table, prev_str, input_char);
    unsigned char state = 0;

    // Compress the input chunk
    for (index = 0; index < chunkSize - 1; index++) {
        next_char = input_chunk[index + 1];
        uint16_t temp_str = get(hash_table, curr_str, next_char);
        if (temp_str != 0x1fff) {
            prev_str = curr_str;
            curr_str = temp_str;
        } else {
            // Output the code for curr_str
            state = output_code(curr_str, state, output, &output_ptr);

            // Insert the new string into the dictionary
            put(hash_table, curr_str, next_char, (uint16_t) num++);
            prev_str = 0x1FFF;
            curr_str = next_char;
        }
    }

    // Output the last code
    uint16_t code = get(hash_table, prev_str, next_char);
    output_code(code, state, output, &output_ptr);
    
    printf("Num: %d\n", num);

    return output_ptr;
}


// int main(int argc, char** argv) {
//     //char* input_txt = "itty bitty bit bin";
//     //unsigned char* buff = (unsigned char*) input_txt;
//     // unsigned char output [100];
//     // int output_ptr = 0;
//     // output_ptr = run_LZW((unsigned char*) input, 18, output, output_ptr);
//     // printf("%d\n", output_ptr);
//     // int i;
//     // for (i = 0; i < output_ptr; i++) {
//     //     printf("0x%.2X\n", output[i]);
//     // }
//     FILE* fp = fopen("../LittlePrince.txt", "r");
// 	if (fp == NULL) {
// 		perror("invalid file");
// 		exit(EXIT_FAILURE);
// 	}

	
// 	fseek(fp, 0, SEEK_END); // seek to end of file
// 	int file_size = ftell(fp); // get current file pointer
// 	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

	
// 	unsigned char* buff = (unsigned char*) (malloc(
// 			sizeof(unsigned char) * file_size));
// 	if (buff == NULL) {
// 		perror("not enough space");
// 		fclose(fp);
// 		exit(EXIT_FAILURE);
// 	}

	
// 	int bytes_read = fread(&buff[0], sizeof(unsigned char), file_size, fp);
// 	printf("bytes_read %d\n", bytes_read);

//         unsigned char* input = buff;
//         unsigned char* outputBuf = (unsigned char*) (malloc(sizeof(unsigned char) * file_size));
//         int output_ptr = 0;
//         int index = -1;

//         if (index == -1) {
//             // Chunk not found, run LZW
//             unsigned char* tempBuf = (unsigned char*) malloc(sizeof(unsigned char) * (8192 / 8) * 13);
//             int count = 0;
//             count = run_LZW((unsigned char*) input, file_size, tempBuf, count);
//             printf("done running LZW, count = %d\n", count);
//             // Write the header to the output
//             uint32_t header = count << 1;
//             outputBuf[(output_ptr)++] = header & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;

//             // Write the compressed chunk to the output
//             for (int i = 0; i < count; i++) {
//                 outputBuf[(output_ptr)++] = tempBuf[i];
//             }
//         } else {
//             // Chunk found, simply write the index to the output in the header
//             uint32_t header = index << 1;
//             header = header | 1;
//             outputBuf[(output_ptr)++] = header & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 8) & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 16) & 0xFF;
//             outputBuf[(output_ptr)++] = (header >> 24) & 0xFF;
//         }

//         printf("Output ptr: %d\n", output_ptr);

//         FILE *outfd = fopen("output_cpu.bin", "wb");
// 	    int bytes_written = fwrite(outputBuf, 1, output_ptr, outfd);
// 	    printf("write file with %d\n", bytes_written);
// 	    fclose(outfd);
// }