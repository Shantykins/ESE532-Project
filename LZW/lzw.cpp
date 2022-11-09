#include <stdio.h>
#include "hashtable.h"
#include "../App.h"

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
        default:
            printf("ERROR IN STATE\n");
            return -1;
    }
    return state;
}

int run_LZW (unsigned char input_buf[], int start_index, int end_index, unsigned char output[], int output_ptr) {
    hash_table* dict = create_table(NUM_BUCKETS);
    int num;
    int index;

    // Insert the first 256 entries into the dictionary
    for (num = 0; num < 256; num++) {
        put(dict, (char*) &num, num);
    }

    // Setup memory for string processing
    char* curr_str = (char*) malloc(sizeof(char) * MAX_CHUNK_SIZE);
    curr_str[0] = input_buf[start_index];
    curr_str[1] = '\0';
    char* temp_str = (char*) malloc(sizeof(char) * MAX_CHUNK_SIZE);

    unsigned char state = 0;

    // Compress the input chunk
    for (index = start_index; index < end_index; index++) {
        char next_char[2];
        next_char[0] = input_buf[index + 1];
        next_char[1] = '\0';
        strcpy(temp_str, curr_str);
        strcat(temp_str, next_char);
        if (get(dict, temp_str) != (uint16_t) -1) {
            strcpy(curr_str, temp_str);
        } else {
            // Output the code for curr_str
            int code = get(dict, curr_str);
            state = output_code(code, state, output, &output_ptr);

            // Insert the new string into the dictionary
            put(dict, temp_str, num++);
            strcpy(curr_str, next_char);
        }
    }

    // Output the last code, if necessary
    if (strlen(curr_str) > 0) {
        int code = get(dict, curr_str);
        output_code(code, state, output, &output_ptr);
    }
    
}

// int main(int argc, char** argv) {
// //     char* input = "itty bitty bit bin";
// //     unsigned char output [100];
// //     int output_ptr = 0;
// //     output_ptr = run_LZW((unsigned char*) input, 0, 18, output, output_ptr);
// //     printf("%d\n", output_ptr);
// //     int i;
// //     for (i = 0; i < output_ptr; i++) {
// //         printf("0x%.2X\n", output[i]);
// //     }
//     FILE* fp = fopen("../LittlePrince.txt", "r");
// 	if (fp == NULL) {
// 		perror("invalid file");
// 		exit(EXIT_FAILURE);
// 	}

// 	//
// 	fseek(fp, 0, SEEK_END); // seek to end of file
// 	int file_size = ftell(fp); // get current file pointer
// 	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

// 	//
// 	unsigned char* buff = (unsigned char*) (malloc(
// 			sizeof(unsigned char) * file_size));
// 	if (buff == NULL) {
// 		perror("not enough space");
// 		fclose(fp);
// 		exit(EXIT_FAILURE);
// 	}

// 	//
// 	int bytes_read = fread(&buff[0], sizeof(unsigned char), file_size, fp);
// 	printf("bytes_read %d\n", bytes_read);

//         unsigned char* input = buff;
//         unsigned char* outputBuf = (unsigned char*) (malloc(sizeof(unsigned char) * file_size));
//         int output_ptr = 0;
//         int start_index = 0;
//         int end_index = file_size;
//         int index = -1;

//         if (index == -1) {
//             // Chunk not found, run LZW
//             unsigned char* tempBuf = (unsigned char*) malloc(sizeof(unsigned char) * (MAX_CHUNK_SIZE / 8) * 13);
//             int count = 0;
//             count = run_LZW((unsigned char*) input, start_index, end_index, tempBuf, count);

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
//         FILE *outfd = fopen("output_cpu.bin", "wb");
// 	    int bytes_written = fwrite(outputBuf, 1, output_ptr, outfd);
// 	    printf("write file with %d\n", bytes_written);
// 	    fclose(outfd);
// }
