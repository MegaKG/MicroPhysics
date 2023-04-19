//MicroPhysics 2D Physics Engine
//V1.0
//Copyright Kaelan Grainger 2023

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <CL/cl.h>


#define MAX_SOURCE_SIZE (0x100000)
#define CL_TARGET_OPENCL_VERSION 200


typedef struct __attribute__ ((packed)) _Body
{
    cl_long ID;

    cl_double X;
    cl_double Y;

    cl_double Mass;

    cl_double XVelocity;
    cl_double YVelocity;

    cl_double PendingXVelocity;
    cl_double PendingYVelocity;

    cl_char WholeX; //Is an X Plane, locks the body in place
    cl_char WholeY; //Is a Y Plane, locks the body in place

    cl_double Radius;
} Body;

const int workers = 1024;

void PrettyPrint(Body* Input){
            double Energy = ((0.5 * Input->Mass) * pow(Input->XVelocity,2)) + ((0.5 * Input->Mass) * pow(Input->YVelocity,2));
            printf("Object %li:\n\tPosition: (%lf,%lf)\n\tSpeed: %lf m/s\n\tMass: %lf\nKinetic Energy: %lf",Input->ID,Input->X,Input->Y,pow(pow(Input->XVelocity,2) + pow(Input->YVelocity,2),0.5),Input->Mass,Energy);
        }

void jsonPrint(Body* Input){
    printf("{\"ID\": %li, \"Position\": [%lf, %lf], \"Mass\": %lf, \"Radius\": %lf, \"Velocity\": [%lf, %lf]}\n",Input->ID,Input->X,Input->Y,Input->Mass,Input->Radius,Input->XVelocity,Input->YVelocity);

}


Body* NewBody(long ID, double X, double Y, double Mass, double Radius, double XVelocity, double YVelocity, char WholeX, char WholeY){
    Body* Output = (Body*)malloc(sizeof(Body));
    Output->ID = ID;
    Output->X = X;
    Output->Y = Y;
    Output->Mass = Mass;
    Output->Radius = Radius;
    Output->XVelocity = XVelocity;
    Output->YVelocity = YVelocity;
    Output->WholeX = WholeX;
    Output->WholeY = WholeY;

    return Output;

}

class Engine {
    private:
        cl_int BodyCount;
        Body* MyBodies;

        cl_int ret;
        cl_platform_id platform_id;
        cl_device_id device_id;   
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;
        cl_context context;
        cl_command_queue command_queue;
        cl_program program;
        cl_kernel mathkernel;
        cl_kernel tickkernel;
        cl_mem gpuBodies;
        cl_mem gpuTimeFrame;
        cl_mem gpuBodiesLen;

        

    public:
        Engine(int BodyCount){
            this->BodyCount = BodyCount;
            this->MyBodies = (Body*)malloc(sizeof(Body) * BodyCount);
            for (int i = 0; i < BodyCount; i++){
                MyBodies[i].ID = -1;
            }
            //printf("Allocate!\n");

            //Initialise OpenCL
            // Load the kernel source code into the array source_str
            FILE *fp;
            char *source_str;
            size_t source_size;
        
            fp = fopen("MicroPhysics.cl", "r");
            if (!fp) {
                fprintf(stderr, "Failed to load kernel.\n");
                exit(1);
            }
            source_str = (char*)malloc(MAX_SOURCE_SIZE);
            source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
            fclose( fp );
            //printf("Kernel\n");
        
            // Get platform and device information
            platform_id = NULL;
            device_id = NULL;   

            ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
            ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
            //printf("Get Devices\n");
        
            // Create an OpenCL context
            context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
            //printf("Create Context\n");

        
            // Create a command queue
            command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
            //printf("Create Command Queue\n");


            // Create a program from the kernel source
            program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
            //printf("Create Program\n");


            // Build the program
            ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
            //printf("Build Program\n");

        
            // Create the OpenCL kernel
            mathkernel = clCreateKernel(program, "doMaths", &ret);
            tickkernel = clCreateKernel(program, "stepBodies", &ret);
            //printf("Create Kernels\n");

            //Create Arrays in GPU Memory
            gpuBodies = clCreateBuffer(context, CL_MEM_READ_WRITE, BodyCount * sizeof(Body), NULL, &ret);
            gpuTimeFrame = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(cl_int), NULL, &ret);
            gpuBodiesLen = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(cl_int), NULL, &ret);
            //printf("Allocate GPU Memory\n");

            //Set the Arguments
            ret = clSetKernelArg(mathkernel, 0, sizeof(cl_mem), (void*)&gpuBodies);
            ret = clSetKernelArg(mathkernel, 1, sizeof(cl_mem), (void*)&gpuTimeFrame);
            ret = clSetKernelArg(mathkernel, 2, sizeof(cl_mem), (void*)&gpuBodiesLen);

            ret = clSetKernelArg(tickkernel, 0, sizeof(cl_mem), (void*)&gpuBodies);
            ret = clSetKernelArg(tickkernel, 1, sizeof(cl_mem), (void*)&gpuTimeFrame);
            ret = clSetKernelArg(tickkernel, 2, sizeof(cl_mem), (void*)&gpuBodiesLen);
            //printf("Set Arguments for Kernels\n");

            ret = clEnqueueWriteBuffer(command_queue, gpuBodiesLen, CL_TRUE, 0, 1 * sizeof(cl_int), &BodyCount, 0, NULL, NULL);
            //printf("Wrote Array Length\n");
            
        }

        void insertBody(Body* MyBody){
            for (long i = 0; i < this->BodyCount; i++){
                if (this->MyBodies[i].ID == -1){
                    this->MyBodies[i] = *MyBody;
                    break;
                }
            }

            //Update the registry
            ret = clEnqueueWriteBuffer(command_queue, gpuBodies, CL_TRUE, 0, BodyCount * sizeof(Body), MyBodies, 0, NULL, NULL);
            //printf("Added Body & Sync\n");

        }

        Body* getBody(long ID){
            ret = clEnqueueReadBuffer(command_queue, gpuBodies, CL_TRUE, 0, BodyCount * sizeof(Body), MyBodies, 0, NULL, NULL);
            //printf("Read Buffer\n");

            for (long i = 0; i < BodyCount; i++){
                if (MyBodies[i].ID != -1){
                    if (MyBodies[i].ID == ID){
                        return &MyBodies[i];
                    }

                }
                
            }
            return NULL;
        }

        void ListBodies(){
            ret = clEnqueueReadBuffer(command_queue, gpuBodies, CL_TRUE, 0, BodyCount * sizeof(Body), MyBodies, 0, NULL, NULL);
            //printf("Read Buffer\n");
            for (long i = 0; i < BodyCount; i++){
                if (MyBodies[i].ID != -1){
                    printf("Body %li Pointer %p\n",MyBodies[i].ID,&MyBodies[i]);
                }
            }
        }

        void JsonBodies(){
            ret = clEnqueueReadBuffer(command_queue, gpuBodies, CL_TRUE, 0, BodyCount * sizeof(Body), MyBodies, 0, NULL, NULL);
            //printf("Read Buffer\n");
            for (long i = 0; i < BodyCount; i++){
                if (MyBodies[i].ID != -1){
                    jsonPrint(&MyBodies[i]);
                }
            }
        }

        void tick(cl_double TimeFrame){
            ret = clEnqueueWriteBuffer(command_queue, gpuTimeFrame, CL_TRUE, 0, 3 * sizeof(cl_double), &TimeFrame, 0, NULL, NULL);
            //printf("Update Timeframe\n");
            //printf("TimeFrame %f\n",TimeFrame);

            size_t global_item_size = BodyCount;
            size_t local_item_size = 64;

            ret = clEnqueueNDRangeKernel(command_queue, mathkernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
            //printf("Execute Maths\n");

            ret = clEnqueueNDRangeKernel(command_queue, tickkernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
            //printf("Execute Ticks\n");


        }
    
};


