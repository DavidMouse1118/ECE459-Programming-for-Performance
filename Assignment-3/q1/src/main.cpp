#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>

#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h> 

#include <base64/base64.h>

using std::cout;
using std::cerr;
using std::endl;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

int gMaxSecretLen = 4;

std::string gAlphabet = "abcdefghijklmnopqrstuvwxyz"
                        "0123456789";

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

void usage(const char *cmd) {
    cout << cmd << " <token> [maxLen] [alphabet]" << endl;
    cout << endl;

    cout << "Defaults:" << endl;
    cout << "maxLen = " << gMaxSecretLen << endl;
    cout << "alphabet = " << gAlphabet << endl;
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    std::stringstream jwt;
    jwt << argv[1];

    if (argc > 2) {
        gMaxSecretLen = atoi(argv[2]);
    }
    if (argc > 3) {
        gAlphabet = argv[3];
    }

    std::string header64;
    getline(jwt, header64, '.');

    std::string payload64;
    getline(jwt, payload64, '.');

    std::string origSig64;
    getline(jwt, origSig64, '.');

    // Our goal is to find the secret to HMAC this string into our origSig
    std::string message = header64 + '.' + payload64;
    std::string origSig = base64_decode(origSig64);

    char *gAlphabet_char = new char[gAlphabet.size()+1];
    gAlphabet_char[gAlphabet.size()] = 0;
    memcpy(gAlphabet_char, gAlphabet.c_str(), gAlphabet.size());

    char *message_char = new char[message.size()+1];
    message_char[message.size()] = 0;
    memcpy(message_char, message.c_str(), message.size());

    char *origSig_char = new char[origSig.size()+1];
    origSig_char[origSig.size()] = 0;
    memcpy(origSig_char, origSig.c_str(), origSig.size());

    // Use OpenCL to brute force JWT
    try { 
        // Get available platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Select the default platform and create a context using this platform and the GPU
        cl_context_properties cps[3] = { 
            CL_CONTEXT_PLATFORM, 
            (cl_context_properties)(platforms[0])(), 
            0 
        };
        cl::Context context(CL_DEVICE_TYPE_GPU, cps);
 
        // Get a list of devices on this platform
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
 
        // Create a command queue and use the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        // cl_int err = CL_SUCCESS;

        // cl_uint uiMaxUnits = devices[0].getInfo< CL_DEVICE_MAX_COMPUTE_UNITS >( &err );

        // size_t stMaxWorkGroup = devices[0].getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >( &err );

        // cl_uint uiMaxDim = devices[0].getInfo< CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS >( &err );

        // std::vector< size_t > uiMaxDimSizes = devices[0].getInfo< CL_DEVICE_MAX_WORK_ITEM_SIZES >( &err );

        // std::cout  << "CL_DEVICE_MAX_COMPUTE_UNITS : " << uiMaxUnits << std::endl;
        // std::cout  << "CL_DEVICE_MAX_WORK_GROUP_SIZE : " << stMaxWorkGroup << std::endl;
        // std::cout  << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS : " << uiMaxDim << std::endl;
        // for( size_t wis = 0; wis < uiMaxDimSizes.size( ); ++wis )
        // {
        //     std::stringstream dimString;
        //     dimString << "Dimension[ " << wis << " ] : ";
        //     std::cout << dimString.str( ) << uiMaxDimSizes[ wis ] << std::endl;
        // }
        // Read source file
        std::ifstream sourceFile("src/jwtcracker.cl");
            if(!sourceFile.is_open()){
                std::cerr << "Cannot find kernel file" << std::endl;
                throw;
            }
        std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
 
        // Make program of the source code in the context
        cl::Program program = cl::Program(context, source);
 
        // Build program for these specific devices
        try {
            // Set compiler option for messageLength and gMaxSecretLen
            std::string option = "-D messageLength=" + std::to_string(message.size()) + " -D gMaxSecretLen=" + std::to_string(gMaxSecretLen);
            char *option_char = new char[option.size()+1];
            option_char[option.size()] = 0;
            memcpy(option_char, option.c_str(), option.size());
            // std::cout << std::string(option_char) << endl;

            program.build(devices, option_char);
        } catch(cl::Error error) {
            std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
            throw;
        }
 
        // Make kernels
        cl::Kernel kernel(program, "bruteForceJWT");
 
        // Create buffers
        cl::Buffer buffer_message = cl::Buffer(
            context,
            CL_MEM_READ_ONLY,
            strlen(message_char)
        );

        cl::Buffer buffer_origSig = cl::Buffer(
            context,
            CL_MEM_READ_ONLY,
            strlen(origSig_char)
        );

        cl::Buffer buffer_gAlphabet = cl::Buffer(
            context,
            CL_MEM_READ_ONLY,
            strlen(gAlphabet_char)
        );

        cl::Buffer buffer_secret = cl::Buffer(
            context,
            CL_MEM_WRITE_ONLY,
            gMaxSecretLen + 1
        );
 
        // Write buffers
        queue.enqueueWriteBuffer(
            buffer_message,
            CL_TRUE,
            0,
            strlen(message_char),
            message_char
        );

        queue.enqueueWriteBuffer(
            buffer_origSig,
            CL_TRUE,
            0,
            strlen(origSig_char),
            origSig_char
        );

        queue.enqueueWriteBuffer(
            buffer_gAlphabet,
            CL_TRUE,
            0,
            strlen(gAlphabet_char),
            gAlphabet_char
        );

        // Set arguments to kernel
        kernel.setArg(0, buffer_message); 
        kernel.setArg(1, buffer_origSig); 
        kernel.setArg(2, buffer_gAlphabet);
        kernel.setArg(3, buffer_secret);

        // Run the kernel on specific ND range
        cl::NDRange globalSize(pow(gAlphabet.length(), gMaxSecretLen));
        cl::NDRange local(256);
        queue.enqueueNDRangeKernel(
            kernel, 
            cl::NullRange, 
            globalSize,
            local); 
 
        // Read buffer(s)
        char* secret = new char[gMaxSecretLen + 1]; 
        queue.enqueueReadBuffer(
            buffer_secret,
            CL_TRUE,
            0,
            gMaxSecretLen + 1, 
            secret
        );

        std::cout << std::string(secret) << endl;
    } catch(cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
    }

    return 0;
}
