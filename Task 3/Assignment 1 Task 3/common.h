#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// function to handle error
void handle_error(cl::Error e);

// outputs message then quits
void quit_program(const std::string str);

// looks up and displays OpenCL error code as a string
const std::string lookup_error_code(cl_int error_code);

// allows the user to select a device, displays the available platform and device options
// returns whether selection was successful, the selected device and its platform
bool select_one_device(cl::Platform* platfm, cl::Device* dev);

// builds program from given filename
bool build_program(cl::Program* prog, const cl::Context* ctx, const std::string filename);

#endif