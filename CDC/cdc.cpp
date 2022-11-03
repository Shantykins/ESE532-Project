#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include "Stopwatch.h"
#include "App.h"

#define WIN_SIZE 16
#define PRIME 3
#define MODULUS 256
#define TARGET 0
#define NUM_THREADS 4

uint64_t hash_func(unsigned char *input, unsigned int pos)
{
	// put your hash function implementation here
	uint64_t hashOut = 0; 
	int i = 0; 

	for(i = 0; i <  WIN_SIZE; i++)
	{
		uint64_t ord = input[pos + WIN_SIZE - 1 - i];
		uint64_t multiplier = pow(PRIME, i+1);

		hashOut += ord * multiplier;
	}

	return hashOut;
}

void cdc(unsigned char *buff,  unsigned int  start,  unsigned int  end)
{
	// put your cdc implementation here
	unsigned int i; 
    uint64_t currHash = hash_func(buff, start + WIN_SIZE);

	for(i = start + WIN_SIZE; i <= end-WIN_SIZE; i++)
	{
		if((currHash % MODULUS) == TARGET)
		{
			std::cout<<std::endl<<i<<std::endl;
		}

        currHash = currHash * PRIME - ((uint64_t)buff[i] * pow(PRIME, WIN_SIZE+1)) + ((uint64_t)buff[i + WIN_SIZE] * PRIME);
	}
}

void test_cdc( const char* file )
{
	FILE* fp = fopen(file,"r" );
	if(fp == NULL ){
		perror("fopen error");
		return;
	}

	fseek(fp, 0, SEEK_END); // seek to end of file
	int file_size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

	unsigned char* buff = (unsigned char *)malloc((sizeof(unsigned char) * file_size ));	
	if(buff == NULL)
	{
		perror("not enough space");
		fclose(fp);
		return;
	}

	int bytes_read = fread(&buff[0], sizeof(unsigned char), file_size, fp);

	stopwatch total_time;
	// parallelize cdc over 4 threads here
	total_time.start();

	std::vector<std::thread> ths;
    ths.push_back(std::thread(&cdc, buff, 0, file_size/4));
    ths.push_back(std::thread(&cdc, buff, file_size/4, file_size/2));
	ths.push_back(std::thread(&cdc, buff, file_size/2, 3*file_size/4));
	ths.push_back(std::thread(&cdc, buff, 3*file_size/4, file_size));

    pin_thread_to_cpu(ths[0], 0);
    pin_thread_to_cpu(ths[1], 1);
	pin_thread_to_cpu(ths[2], 2);
	pin_thread_to_cpu(ths[3], 3);

    for (auto &th : ths)
    {
      th.join();
    }

	//cdc(buff, 0, file_size);
	total_time.stop();

	std::cout << "Total time taken by the loop is: " << total_time.latency() << " ns." << std::endl;

    free(buff);
    return;
}