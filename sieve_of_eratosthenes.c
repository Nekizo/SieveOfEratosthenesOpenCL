#include <CL/cl_platform.h>
#define PROGRAM_FILE "sieve_of_eratosthenes.cl"
#define KERNEL_FUNC "sieve_of_eratosthenes"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "sieve_of_eratosthenes.h"


cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}


/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   err = fread(program_buffer, sizeof(char), program_size, program_handle);
   if(err < 0) {
      printf("Couldn't read file with the program");
      exit(1);
   }

   fclose(program_handle);

   /* Create program from file 

   Creates a program from the source code in the sieve_of_eratosthenes.cl file. 
   Specifically, the code reads the file's content into a char array 
   called program_buffer, and then calls clCreateProgramWithSource.
   */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      printf("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}



bool* SieveOfEratosthenes(int n) {
	/* OpenCL structures */
	cl_device_id device;
	cl_context context;
	cl_program program;
	cl_kernel kernel;
	cl_command_queue queue;
	cl_int err;
	size_t local_size, global_size;
	local_size=1;
	global_size=n;
	cl_mem input_buffer;


	/* Create bool array and set value on true*/
	bool prime[n];
    bool* output_prime = (bool*)malloc((n + 1) * sizeof(bool));
    memset(prime,true, sizeof(prime));

	prime[0] = prime[1] = false;
	
	device = create_device();
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if(err < 0) {
	  perror("Couldn't create a context");
	  exit(1);   
	}

	program = build_program(context, device, PROGRAM_FILE);

	input_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
		 CL_MEM_COPY_HOST_PTR, n * sizeof(bool), prime, &err); 
	if(err < 0) {
		perror("Couldn't create a buffer");
		exit(1);   
	};
	/* Create a command queue 

	Does not support profiling or out-of-order-execution
	*/

	queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
	if(err < 0) {
	  perror("Couldn't create a command queue");
	  exit(1);   
	};

	/* Create a kernel */
	kernel = clCreateKernel(program, KERNEL_FUNC, &err);
	if(err < 0) {
	  perror("Couldn't create a kernel");
	  exit(1);
	};
    for (cl_uint p = 2; p * p <= (cl_uint)n; p++) {
        if (prime[p]) {
			/* Bool Buffer */
		   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer); // <=====INPUT
		   if(err < 0) {
			  perror("Couldn't create a kernel argument 1");
			  exit(1);
		   }

		   /*Current Checking prime*/
		   err = clSetKernelArg(kernel, 1, sizeof(cl_uint), &p);
		   if(err < 0) {
			  printf("Couldn't create a kernel argument 2 %i",(int)err);
			  exit(1);
		   }

		   /*Remove check prime from check*/
		   size_t offset_size=p; 
		   size_t current_offset_size=(global_size-p)/p; 
		  
			err = clEnqueueNDRangeKernel(
					queue, kernel,1, &offset_size,
					&current_offset_size, 
					&local_size, 0, NULL, NULL); 
		   if(err < 0) {
			  printf("Couldn't enqueue the kernel %i",err);
			  exit(1);
		   }
        }
    }

   /* Read the kernel's output   */
   err = clEnqueueReadBuffer(queue, input_buffer, CL_TRUE, 0, 
         sizeof(bool)*n, output_prime, 0, NULL, NULL); // <=====GET OUTPUT
   if(err < 0) {
      printf("Couldn't read the buffer %i",err);
      exit(1);
   }
    
	
	return output_prime;
}


