#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include "../App.h"

#define WIN_SIZE 12
#define PRIME 3
#define MODULUS 4096
#define TARGET 0

//
// Assume prime = 3
//
uint64_t powPrime[WIN_SIZE + 2] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441, 1594323};


uint64_t hash_func(unsigned char *input, unsigned int pos)
{
	// put your hash function implementation here
	uint64_t hashOut = 0; 
	int i = 0; 

	for(i = 0; i <  WIN_SIZE; i++)
	{
		uint64_t ord = input[pos + WIN_SIZE - 1 - i];
		uint64_t multiplier = powPrime[i+1];//pow(PRIME, i+1);

		hashOut += ord * multiplier;
	}

	return hashOut;
}

int runCDC(unsigned char* inputBuf, unsigned char* outputBuf ,int length, int* lastChunkIdx)
{
	// put your cdc implementation here
	int i, chunkSize = 0; 

	// //
	// // Retains the last chunk index
	// //
	// static int lastChunkIdx = 0;

	//
	// Hash for first WIN_SIZE block
	//
    uint64_t currHash = hash_func(inputBuf, *lastChunkIdx);
				
	for(i = *lastChunkIdx; i < *lastChunkIdx + WIN_SIZE; i++)
	{
		outputBuf[chunkSize++] = inputBuf[i];
	}

	*lastChunkIdx = *lastChunkIdx + WIN_SIZE;

	static unsigned long p = powPrime[WIN_SIZE+1];

	for(i = *lastChunkIdx; i < length-WIN_SIZE ; i++)
	{
		if((currHash % MODULUS) == TARGET || chunkSize >= MAX_CHUNK_SIZE)
		{
			break;			
		}

		outputBuf[chunkSize++] = inputBuf[i];

		(*lastChunkIdx)++;

        currHash = currHash * PRIME - ((uint64_t)inputBuf[i] * p) + ((uint64_t)inputBuf[i + WIN_SIZE] * PRIME);
	}

	outputBuf[chunkSize++] = '\0';
	return chunkSize;
}
