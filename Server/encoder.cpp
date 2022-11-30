#include "encoder.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "stopwatch.h"

//
// Include Application Header file. 
//
#include "../App.h"

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

int offset = 0;
unsigned char* file;

void handle_input(int argc, char* argv[], int* blocksize) {
	int x;
	extern char *optarg;

	while ((x = getopt(argc, argv, ":b:")) != -1) {
		switch (x) {
		case 'b':
			*blocksize = atoi(optarg);
			printf("blocksize is set to %d optarg\n", *blocksize);
			break;
		case ':':
			printf("-%c without parameter\n", optopt);
			break;
		}
	}
}



int main(int argc, char* argv[]) {
	stopwatch ethernet_timer;
	stopwatch total_timer;
	unsigned char* input[NUM_PACKETS];
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	int total_length = 0;
	ESE532_Server server;
	float runtime = 0;
	int bytes_dropped = 0;
	float kernel_time = 0;
	float non_lzw = 0;

	if (argc != 2) {
		printf("Usage: ./encoder <output_filename>\n");
		return -1;
	}

	total_timer.start();
	// default is 2k
	int blocksize = BLOCKSIZE;

	// set blocksize if decalred through command line
	handle_input(argc, argv, &blocksize);

	file = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	if (file == NULL) {
		printf("help\n");
	}

	for (int i = 0; i < NUM_PACKETS; i++) {
		input[i] = (unsigned char*) malloc(
				sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
		if (input[i] == NULL) {
			std::cout << "aborting " << std::endl;
			return 1;
		}
	}

	server.setup_server(blocksize);

	writer = pipe_depth;
	server.get_packet(input[writer]);
	count++;

	// get packet
	unsigned char* buffer = input[writer];

	// decode
	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;
	total_length += length;
	// printing takes time so be weary of transfer rate
	//printf("length: %d offset %d\n",length,offset);

	// we are just memcpy'ing here, but you should call your
	// top function here.
	//memcpy(&file[offset], &buffer[HEADER], length);

	//
	// OUR APP IS CALLED HERE
	// INPUT -> buffer[2] : Since first two bytes are read above (no message body)
	// Output -> file[offset]  : Pointer to output; offset incremented after every packet read
	// length -> Length of each packet 
	//

    
    //======================================================================================================================
    //
    // OPENCL INITIALIZATION
    //======================================================================================================================    

    cl_int err;
    std::string binaryFile = "encoder.xclbin";
    unsigned fileBufSize;
    std::vector<cl::Device> devices = get_xilinx_devices();
    devices.resize(1);
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);
    char *fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);

    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err); 
    cl::Kernel kernel_lzw(program, "lzw_hw", &err);

    std::vector<cl::Event> write_event(1);
    std::vector<cl::Event> compute_event(1);
    std::vector<cl::Event> done_event(1);


    cl::Buffer in_buf = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char) * MAX_CHUNK_SIZE, NULL, &err);
    cl::Buffer out_buf = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * (MAX_CHUNK_SIZE / 8) * 13, NULL, &err);

    unsigned char* outputChunk = (unsigned char *)q.enqueueMapBuffer(in_buf, CL_TRUE, CL_MAP_WRITE, 
                                                        0, sizeof(unsigned char)*MAX_CHUNK_SIZE);
    unsigned char* tempbuf = (unsigned char *)q.enqueueMapBuffer(out_buf, CL_TRUE, CL_MAP_READ, 
                                                        0, sizeof(unsigned char)*(MAX_CHUNK_SIZE / 8) * 13);

    //======================================================================================================================
    //
    // OPENCL INITIALIZATION ENDS
    //======================================================================================================================   

	int output_ptr = runApp(&buffer[HEADER], &file[offset], length, &runtime, &bytes_dropped, 
							q, kernel_lzw, in_buf, out_buf, outputChunk, tempbuf);

	offset += output_ptr;
	writer++;

	//last message
	while (!done) {
		// reset ring buffer
		if (writer == NUM_PACKETS) {
			writer = 0;
		}

		ethernet_timer.start();
		server.get_packet(input[writer]);
		ethernet_timer.stop();

		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		total_length += length;
		//printf("length: %d offset %d\n",length,offset);
		//memcpy(&file[offset], &buffer[HEADER], length);


		//
		// OUR APP IS CALLED HERE
		// INPUT -> buffer[2] : Since first two bytes are read above (no message body)
		// Output -> file[2]  : Pointer to output
		// length -> Length of each packet 
		//
		int output_ptr = runApp(&buffer[HEADER], &file[offset], length, &runtime, &bytes_dropped, 
							q, kernel_lzw, in_buf, out_buf, outputChunk, tempbuf);

		offset += output_ptr;
		writer++;
	}

	// write file to root and you can use diff tool on board
	FILE *outfd = fopen(argv[1], "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

	for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	free(file);
	total_timer.stop();
	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;
	std::cout << "Total Time: " << total_timer.latency() << " ms." << std::endl;
	int total_bits = total_length * 8;
	float total_megabits = total_bits / 1048576.0f;
	float total_mbps = (total_megabits / total_timer.latency()) * 1000;
	std::cout << "Throughput of Application (including setup overhead): " << total_mbps << " Mb/s" << std::endl;

	float total_mbps2 = (total_megabits / runtime) * 1000;
	std::cout << "Throughput of Application (just App.cpp): " << total_mbps2 << " Mb/s" << std::endl;
	std::cout << "Bytes dropped by Dedup: " << bytes_dropped << " B" << std::endl;
	float kernel_time_ms = kernel_time / 1000000.0f;
	std::cout << "Total Non-LZW Time: " << non_lzw << " ms" << std::endl;
	std::cout << "Throughput of Application (no LZW): " << (total_megabits / non_lzw) * 1000 << " Mb/s" << std::endl;
	std::cout << "Total Kernel Time: " << kernel_time_ms << " ms" << std::endl;
	std::cout << "Kernel Bandwidth: " << (total_megabits / kernel_time_ms) * 1000 << "Mb/s" << std::endl;

	return 0;
}

