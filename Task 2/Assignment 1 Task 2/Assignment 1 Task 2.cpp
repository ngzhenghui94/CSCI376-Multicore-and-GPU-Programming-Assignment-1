#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>

// OpenCL header, depending on OS
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.hpp>
#endif
#include "common.h"
#define CHARLENGTH 52
#define INTLENGTH 512


// template to concatenate 2 vectors.
template <typename T>
std::vector<T> operator+(const std::vector<T>& A, const std::vector<T>& B){
	std::vector<T> AB;
	AB.reserve(A.size() + B.size());                // preallocate memory
	AB.insert(AB.end(), A.begin(), A.end());        // add A;
	AB.insert(AB.end(), B.begin(), B.end());        // add B;
	return AB;
}

int main(void) {
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device
	unsigned int i, j;
	unsigned int fillInt = 1; //starting integer to fill intVector

	//Create 2 vector, 1 with unsigned char of 52, another with unsigned int of 512.
	std::vector<cl_uchar> alphabets;
	std::vector<cl_uchar> alphabetsSmall (CHARLENGTH /2);
	std::vector<cl_uchar> alphabetsLarge (CHARLENGTH /2);
	std::vector<cl_uint> intVector;

	std::vector<cl_uchar> charOutput(CHARLENGTH);	// Char output data
	std::vector<cl_uint> intOutput(INTLENGTH);   // Int output data

	//Fill vector with small alphabets atoz
	std::iota(alphabetsSmall.begin(), alphabetsSmall.end(), 'a');
	//Fill vector with large alphabets AtoZ
	std::iota(alphabetsLarge.begin(), alphabetsLarge.end(), 'A');
	//Merge the 2 vector (alhpabetsSmall and alphabertsLarge) into vector alphabets
	/*
	alphabets.reserve(alphabetsSmall.size() + alphabetsLarge.size());
	alphabets.insert(alphabets.begin(), alphabetsSmall.begin(), alphabetsSmall.end());
	alphabets.insert(alphabets.end(), alphabetsLarge.begin(), alphabetsLarge.end());
	*/
	alphabets = alphabetsSmall + alphabetsLarge;

	//Fill the vector with ints
	for (i = 0; i < INTLENGTH; i++) {
		intVector.push_back(fillInt);
		fillInt++;
	}
	

	//Assignment 1 Task 2.2 (Create three openCL mem obj)
	cl::Buffer firstBuffer;
	cl::Buffer secondBuffer;
	cl::Buffer thirdBuffer;

	std::cout << "Assignment 1 Task 2: " << std::endl;
	try {

		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// create buffers
		//first buffer read-only, init with contents of the alphabet vector
		firstBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uchar) * alphabets.size(), &alphabets[0]);
		//second buffer write-only, created to store 52 unsigned char
		secondBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar) * CHARLENGTH);
		//third buffer read n write, created to store 512 unsigned char
		thirdBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * INTLENGTH);

		// create command queue
		queue = cl::CommandQueue(context, device);

		//enqueue copy content of first buffer into the second buffer.
		queue.enqueueCopyBuffer(firstBuffer, secondBuffer, 0, 0, sizeof(cl_uchar) * CHARLENGTH);
		//write content from vector of 512 int to third buffer
		queue.enqueueWriteBuffer(thirdBuffer, CL_TRUE, 0, sizeof(cl_uint) * intVector.size(), &intVector[0]);

		// build the program
		if (!build_program(&program, &context, "task2.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create a kernel
		kernel = cl::Kernel(program, "task2");

		// set kernel args as per task2.cl
		float a = 12.34;
		kernel.setArg(0, a); //index 0 (float value) pass in a  -> float value
		kernel.setArg(1, secondBuffer); //index 1 (__global unsigned char *copied) pass in the secondBuffer
		kernel.setArg(2, thirdBuffer); //index 2 (__global unsigned int *integers) pass in the third buffer

		// enqueue kernel for execution
		queue.enqueueTask(kernel);
		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(firstBuffer, CL_TRUE, 0, sizeof(cl_uchar) * CHARLENGTH, &charOutput[0]);

		// output contents
		std::cout << "\nContents of firstBuffer: " << std::endl;
		for (int i = 0; i < CHARLENGTH; i++){
			std::cout << charOutput[i] << " ";
		}

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(secondBuffer, CL_TRUE, 0, sizeof(cl_uchar) * CHARLENGTH, &charOutput[0]);

		// output contents
		std::cout << "\nContents of secondBuffer: " << std::endl;
		for (int i = 0; i < CHARLENGTH; i++){
			std::cout << charOutput[i] << " ";
		}

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(thirdBuffer, CL_TRUE, 0, sizeof(cl_uint) * INTLENGTH, &intOutput[0]);

		// output contents
		std::cout << "\nContents of thirdBuffer: " << std::endl;
		for (int i = 0; i < INTLENGTH; i++){
			std::cout << intOutput[i] << " ";
		}
	}
		
	// catch any OpenCL function errors
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}
	return 0;
}



/*
for (const auto& i : integers)
	{
		std::cout << i << std::endl;
	}
*/

/*

//get Mem size
	std::cout << "firstBuffer size: " << firstBuffer.getInfo<CL_MEM_SIZE>() << std::endl;
	std::cout << "secondBuffer size: " << secondBuffer.getInfo<CL_MEM_SIZE>() << std::endl;
	std::cout << "thirdBuffer size: " << thirdBuffer.getInfo<CL_MEM_SIZE>() << std::endl;
	//get Mem loc
	std::cout << "alphabets memory location: " << &alphabets[0] << std::endl;
	std::cout << "firstBuffer memory location: " << firstBuffer.getInfo<CL_MEM_HOST_PTR>() << std::endl;
	std::cout << "alphabets memory location: " << &alphabets[0] << std::endl;
	std::cout << "secondBuffer memory location: " << secondBuffer.getInfo<CL_MEM_HOST_PTR>() << std::endl;
	std::cout << "alphabets memory location: " << &alphabets[0] << std::endl;
	std::cout << "firstBuffer memory location: " << firstBuffer.getInfo<CL_MEM_HOST_PTR>() << std::endl;
*/

/*
// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}
		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "task2.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}
		// create a kernel
		kernel = cl::Kernel(program, "task2");

		// create command queue
		queue = cl::CommandQueue(context, device);*/

		/*
			std::cout << "IntVector: " << std::endl;
			for (const auto& i : intVector) {
				std::cout << i;
			}
			std::cout << std::endl;

			std::cout << "alphabetsSmall: " << std::endl;
			for (const auto& i : alphabetsSmall) {
				std::cout << i;
			}
			std::cout << std::endl;
			std::cout << "alphabetsLarge: " << std::endl;
			for (const auto& i : alphabetsLarge) {
				std::cout << i;
			}
			std::cout << std::endl;
			std::cout << "alphabets: " << std::endl;
			for (const auto& i : alphabets) {
				std::cout << i;
			}
			*/