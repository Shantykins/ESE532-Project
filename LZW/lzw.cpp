#include <stdio.h>
#include "hashtable.h"

#define NUM_BUCKETS 1000
#define MAX_CHUNK_SIZE 8192

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
    sprintf(curr_str, "%c", input_buf[start_index]);
    char* temp_str = (char*) malloc(sizeof(char) * MAX_CHUNK_SIZE);

    unsigned char state = 0;

    // Compress the input chunk
    for (index = start_index; index < end_index; index++) {
        printf("In for, index = %d\n", index);
        printf("Curr_str: %s\n", curr_str);
        char next_char[2];
        next_char[0] = input_buf[index + 1];
        next_char[1] = 0;
        printf("Next char: %s\n", next_char);
        strcpy(temp_str, curr_str);
        strcat(temp_str, next_char);
        printf("Temp_str: %s\n", temp_str);
        if (get(dict, temp_str) != (uint16_t) -1) {
            printf("In if\n");
            strcpy(curr_str, temp_str);
        } else {
            printf("In else\n");
            // Output the code for curr_str
            int code = get(dict, curr_str);
            code = code & 0x1FFF;
            switch (state) {
                case 0:
                    // Output: 8 | 5 _ _ _
                    code = code << 3;
                    output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 1;
                    break;
                case 1:
                    // Output: _ _ _ _ _ 3 | 8 | 2 _ _ _ _ _ _
                    code = code << 6;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x07));
                    output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 2;
                    break;
                case 2:
                    // Output: _ _ 6 | 7 _
                    code = code << 1;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x3F));
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 3;
                    break;
                case 3:
                    // Output: _ _ _ _ _ _ _ 1 | 8 | 4 _ _ _ _
                    code = code << 4;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x01));
                    output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 4;
                    break;
                case 4:
                    // Output: _ _ _ _ 4 | 8 | 1 _ _ _ _ _ _ _
                    code = code << 7;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x0F));
                    output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 5;
                    break;
                case 5:
                    // Output: _ 7 | 6 _ _
                    code = code << 2;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x7F));
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 6;
                    break;
                case 6:
                    // Output: _ _ _ _ _ _ 2 | 8 | 3 _ _ _ _ _
                    code = code << 5;
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x03));
                    output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 7;
                    break;
                case 7:
                    // Output: _ 5 | 8
                    output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x1F));
                    output[output_ptr++] = (unsigned char) (code & 0xFF);
                    state = 0;
                    break;
                default:
                    printf("ERROR IN STATE\n");
                    return -1;
            }

            // Insert the new string into the dictionary
            put(dict, temp_str, num++);
            strcpy(curr_str, next_char);
        }
    }

    // Output the last code, if necessary
    if (strlen(curr_str) > 0) {
        int code = get(dict, curr_str);
        code = code & 0x1FFF;
        switch (state) {
            case 0:
                // Output: 8 | 5 _ _ _
                code = code << 3;
                output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 1;
                break;
            case 1:
                // Output: _ _ _ _ _ 3 | 8 | 2 _ _ _ _ _ _
                code = code << 6;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x07));
                output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 2;
                break;
            case 2:
                // Output: _ _ 6 | 7 _
                code = code << 1;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x3F));
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 3;
                break;
            case 3:
                // Output: _ _ _ _ _ _ _ 1 | 8 | 4 _ _ _ _
                code = code << 4;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x01));
                output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 4;
                break;
            case 4:
                // Output: _ _ _ _ 4 | 8 | 1 _ _ _ _ _ _ _
                code = code << 7;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x0F));
                output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 5;
                break;
            case 5:
                // Output: _ 7 | 6 _ _
                code = code << 2;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x7F));
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 6;
                break;
            case 6:
                // Output: _ _ _ _ _ _ 2 | 8 | 3 _ _ _ _ _
                code = code << 5;
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 16) & 0x03));
                output[output_ptr++] = (unsigned char) ((code >> 8) & 0xFF);
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 7;
                break;
            case 7:
                // Output: _ 5 | 8
                output[output_ptr - 1] = (unsigned char) (output[output_ptr - 1] | ((code >> 8) & 0x1F));
                output[output_ptr++] = (unsigned char) (code & 0xFF);
                state = 0;
                break;
            default:
                printf("ERROR IN STATE\n");
                return -1;
        }
    }

    // Clean up allocated memory
    free(curr_str);
    free(temp_str);
    delete_table(dict);

    return output_ptr;
}

/*int main(int argc, char** argv) {
    char* input = "I AM SAM SAM I AM";
    unsigned char output [100];
    int output_ptr = 0;
    output_ptr = run_LZW((unsigned char*) input, 0, 17, output, output_ptr);
    printf("%d\n", output_ptr);
    int i;
    for (i = 0; i < output_ptr; i++) {
        printf("0x%.2X\n", output[i]);
    }
}*/
