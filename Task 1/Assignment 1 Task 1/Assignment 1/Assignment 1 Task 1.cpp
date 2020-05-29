#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

// OpenCL header, depending on OS
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.hpp>
#endif

// functions to handle errors
#include "error.h"

//Assignment 1 Task 1.1 fn prototype
cl_device_type selectDeviceType(std::string);

//Assignment 1 Task 1.3 fn prototype
void createContext(std::vector<cl::Platform>& , std::vector<int>& , std::vector<cl::Device>& ,
    std::vector<int>& , cl::Context& , std::vector<cl::Device>& , cl::CommandQueue& );

int main(void){
    //Data structures to store the platforms, devices, contexts and their ids.
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
  
    std::vector<std::vector<cl::Device>> platformDevices;
    std::vector<std::pair<int, int>> options;
//    std::vector<cl::Device> devicesCPU;
//    std::vector<cl::Device> devicesGPU;
    //Store platform ID
    std::vector<int> platformsID;
    //Store device ID
    std::vector<int> devicesID;
    //Context of device
    cl::Context contexts;
    //Device with context
    std::vector<cl::Device> contextDevice;		
    //Command queue for context and device
    cl::CommandQueue queue;
    //Program
    cl::Program program;
    //Kernels
    std::vector<cl::Kernel> allKernels;

    //Store the output from when we getInfo of the relative getInfo query.
    std::string outputString;
    //Store User's input.
    std::string inputString;
    //Some devices.getInfo outputs are in integer.
    int outputInt;

    //Loop counters
    unsigned int i, j;
    //Boolean for if user input correctly, if not, while(1) till it gets correct input.
    bool correctInput = false;
    //to store the output of CL_DEVICE_GLOBAL_MEM_SIZE & CL_DEVICE_LOCAL_MEM_SIZE
    long globalMemorySize;
    long localMemorySize;
    //CL_DEVICE_MAX_WORK_ITEM_SIZES is in 3 dimension.
    int workItem[3];

    //Device chosen by user
    cl_device_type deviceChosen;
    //Store deviceType
    cl_device_type deviceType;
    //Counter
    unsigned int optionCounter = 0;
    //Task 4 file
    std::ifstream programFile("task1.cl");
    
    //try, if not catch openCL error
    try {
        //Assignment 1 Task 1.1: get User's device Choice.
        std::cout << std::endl;
        std::cout << "Assignment 1 Task 1.1: " << std::endl;
        bool correctInput = false;
        
        while (!correctInput) {
            std::cout << "Please enter the device you want to use (CPU/GPU): " << std::endl;
            std::getline(std::cin, inputString);
            std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::toupper);
            if (inputString == "GPU" || inputString == "CPU") {
                correctInput = true;
                //Assignment 1 Task 1.1: set Device type
                deviceChosen = selectDeviceType(inputString);
            }
            else {
                std::cout << " Please enter a valid input (CPU/GPU)" << std::endl;
            }
        }
        //Assignment 1 Task 1.2 Display infomation of each CPU/GPU device available on the system.
        std::cout << std::endl;
        std::cout << "Assignment 1 Task 1.2: " << std::endl;
        //Available OpenCL platforms on the system.
        cl::Platform::get(&platforms);
        //Print all the platforms on the System.
        std::cout << "Number of OpenCL platforms detected: " << platforms.size() << std::endl;
        //Looping thru the vectors storing the platforms.
        for (i = 0; i < platforms.size(); i++) {
            // get all devices available to the platform
            // platforms[i].getDevices(deviceChoice, &devices);
            platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);
            // store the avail devices for the platform
            platformDevices.push_back(devices);
            //Looping thru the devices
            for (j = 0; j < devices.size(); j++) {
                devices[j].getInfo(CL_DEVICE_TYPE, &deviceType);
                if (deviceType == deviceChosen) {
                    //Get Platform name
                    platforms[i].getInfo(CL_PLATFORM_NAME, &outputString);
                    std::cout << "CL_PLATFORM_NAME: " << outputString << std::endl;
                    //Get Device Name
                    devices[j].getInfo(CL_DEVICE_NAME, &outputString);
                    std::cout << "CL_DEVICE_ID: " << j+1 << std::endl;
                    std::cout << "CL_DEVICE_NAME: " << outputString << std::endl;
                    //Get Max compute units
                    devices[j].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &outputInt);
                    std::cout << "CL_DEVICE_MAX_COMPUTE_UNITS: " << outputInt << std::endl;
                    //Get Max work group size
                    devices[j].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &outputInt);
                    std::cout << "CL_DEVICE_MAX_WORK_GROUP_SIZE: " << outputInt << std::endl;
                    //Get Max work item dimensions
                    devices[j].getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &outputInt);
                    std::cout << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: " << outputInt << std::endl;
                    //Get Max work item sizes
                    devices[j].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &workItem);
                    std::cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES: " << workItem[0] << "*" << workItem[1] << "*" << workItem[2] << std::endl;
                    //Get dev global memory size
                    globalMemorySize = devices[j].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
                    std::cout << "CL_DEVICE_GLOBAL_MEM_SIZE: " << globalMemorySize << std::endl;
                    //Get dev Local memory size
                    localMemorySize = devices[j].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
                    std::cout << "CL_DEVICE_LOCAL_MEM_SIZE: " << localMemorySize << std::endl;
                    platformsID.push_back(i);
                    devicesID.push_back(j);
                    options.push_back(std::make_pair(i, j));
                }
            }
        }
        //Assignment 1 Task 1.3: based on device avail, user select one device, create a context using that device and a command queue 
        std::cout << std::endl;
        std::cout << "Assignment 1 Task 1.3: " << std::endl;
        //fn to create context on device
        createContext(platforms, platformsID, devices, devicesID, contexts, contextDevice, queue);

        //Assignment 1 Task 1.4 read program source code (task1.cl) and build the program, display the build log.
        if (!programFile.is_open()) {
            quit_program("File not found.");
        }
        //loads the contents from the file into the string programString and displays its contents
        std::string programString(std::istreambuf_iterator<char>(programFile),
            (std::istreambuf_iterator<char>()));
        std::cout << std::endl;
        std::cout << "Assignment 1 Task 1.4: " << std::endl;
        std::cout << programString << std::endl;
        //create program source from one input string
        cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));
        //Create a context for the program obj
        cl::Program program(contexts, source);
        //Try to build the program. If not, display error.
        try {
            program.build(contextDevice);
            //Output build log of context deviceS.
            for (int i = 0; i < contextDevice.size(); i++) {
                //get the cpntext device name
                outputString = contextDevice[i].getInfo<CL_DEVICE_NAME>();
                std::string buildLog =program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevice[i]);
                //output device name
                std::cout << "Device " << outputString << " build log:" << std::endl;
                //output the build log of the program.
                std::cout << buildLog << std::endl;
            }
        }
        //catch opencl program build error
        catch (cl::Error err){
            if (err.err() == CL_BUILD_PROGRAM_FAILURE){
                std::cout << err.what() << ": Failed to build program." << std::endl;
                for (int i = 0; i < contextDevice.size(); i++) {
                    //get the cpntext device name
                    outputString = contextDevice[i].getInfo<CL_DEVICE_NAME>();
                    std::string build_log =program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevice[i]);
                    //output device name
                    std::cout << "Device " << outputString << " build log:" << std::endl;
                    //output the build log of the program.
                    std::cout << build_log <<  std::endl;
                }
            } 
        }
        //Assignment 1 Task 1.5 displays the kernels in the program.
        std::cout << std::endl;
        std::cout << "Assignment 1 Task 1.5: " << std::endl;
        //create kernels in prog
        program.createKernels(&allKernels);
        //output number of kernels
        std::cout << "Number of Kernels: " << allKernels.size() << std::endl;
        std::cout << "Kernels found: " << std::endl;
        for (i = 0; i < allKernels.size(); i++) {
            //get the kernel names
            outputString = allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
            //output kernel names
            std::cout << "Kernel " << i << " : " << outputString << std::endl;
        }
    }
    catch(cl::Error err) {
        handle_error(err);
    }
    


}

//Assn1 Task 1.1: selecting device
cl_device_type selectDeviceType(std::string inputString) {
    cl_device_type selectDeviceType;
    std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::toupper);
    if (inputString == "GPU" || inputString == "CPU" || inputString == "ALL" ) {
        if (inputString == "CPU") {
            selectDeviceType = CL_DEVICE_TYPE_CPU;
        }
        else if (inputString == "GPU") {
            selectDeviceType = CL_DEVICE_TYPE_GPU;
        }
    }
    return selectDeviceType;
}

//Assignment 1 Task 1.3: creating context
void createContext(std::vector<cl::Platform>& platforms, std::vector<int>& platformsID, std::vector<cl::Device>& devices,
    std::vector<int>& devicesID, cl::Context& context, std::vector<cl::Device>& contextDevice, cl::CommandQueue& queue) {
    bool correctInput = false;
    std::string getInput;
    while (!correctInput) {
        std::cout << "Enter one device ID to create context: " << std::endl;
        std::getline(std::cin, getInput);
        unsigned int num = std::atoi(getInput.c_str());
        if (num <= devicesID.size() && num != 0) {
            std::cout << "Creating Context for platform: " << platforms[platformsID[num - 1]].getInfo<CL_PLATFORM_NAME>() << std::endl;
            platforms[platformsID[num - 1]].getDevices(CL_DEVICE_TYPE_ALL, &devices);
            context = cl::Context(devices[devicesID[num - 1]]);
            contextDevice = context.getInfo<CL_CONTEXT_DEVICES>();
            for (unsigned int i = 0; i < contextDevice.size(); i++) {
                std::cout << "Context created for Device: " << contextDevice[i].getInfo<CL_DEVICE_NAME>() << std::endl;
            }
            queue = cl::CommandQueue(context, devices[devicesID[num - 1]]);
            std::cout << "Command Queue created for Device: " << std::endl;
            correctInput = true;
        }
        else {
            std::cout << "Incorrect Input." << std::endl;
        }
    }
}

