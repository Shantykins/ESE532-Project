#include "../App.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <iostream>


int main()
{
	
    char infile[] = "../LittlePrince.txt";

    char outfile[] = "out.txt";
    
    FILE* fp = fopen(infile,"r" );
	if(fp == NULL ){
		perror("fopen error");
		return 0;
	}

	fseek(fp, 0, SEEK_END); // seek to end of file
	int file_size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

	unsigned char* buff = (unsigned char *)malloc((sizeof(unsigned char) * file_size ));	
	if(buff == NULL)
	{
		perror("not enough space");
		fclose(fp);
		return 0;
	}

	int bytes_read = fread(&buff[0], sizeof(unsigned char), file_size, fp);


    fclose(fp);

    FILE* fp2 = fopen(outfile, "w");

    //
    // Output chunk array. Max chunk size = 8k or 8192
    //
    unsigned char outputChunk[8192];

    //
    // Variable to keep track of chunk progress. return from function 
    // if we cross the length
    //

    int packetTracker = 0;

    while(packetTracker < bytes_read)
    {

        // RUN CDC
        int chunksize = runCDC(buff, outputChunk, bytes_read);

        packetTracker += chunksize;

        fwrite(outputChunk, 1, chunksize, fp2);
    
    }

    fclose(fp2);

    return 0;

}