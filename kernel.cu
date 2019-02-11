#include <cuda.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <helper_cuda.h>
#include <helper_math.h>
#include <helper_functions.h>

//Rounded up
//for CUDA network setting
int iDivUp(int a, int b)
{
    return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

// device fonction
//using HSV color map to find color in green
__device__ unsigned char binHSV(int R, int G, int B,float hL,float hH, float sL, float sH,float vL, float vH)
{
    //hL = 70,  hH = 160,  sL = 43, sH = 255,  vL = 46,  vH = 255
    // to find green color, G must be max (H in 120+-60)
    if (G > B&&G > R)
    {
        float V = G;      //V=max(R,G，B)  0-255
        if (V >vL && V <vH)  //for green V in 46-255
        {
            float cMin = min(R, B);
            float band = V - cMin;
            float S = band / V * 255; //s=(1-min(R,G,B)/max) 0-255
            if (S >sL && S <sH)  //for green S in 43-255
            {
                float H = (B - R) / band * 60 + 120;  //0-360
                if (H>hL && H<hH) //H in 120+-60
                    return 0; // black
            }
        }
    }
    return 255; //white
}

// kenerl fonction for CUDA network
__global__ void binKernel(unsigned char*indata, unsigned char*outdata, int w, int h,  float hL, float hH, float sL, float sH, float vL, float vH)
{
    int x = blockDim.x*blockIdx.x + threadIdx.x;
    int y = blockDim.y*blockIdx.y + threadIdx.y;

    if (x < w&&y < h)
    {

        int coordinator = y * w * 3 + x * 3;
        int coordinator_out = y * w + x;

        //in opencv RGB chanels order is BGR
        //attention if BGR2RGB is used
        int r = indata[coordinator];
        int g = indata[coordinator + 1];
        int b = indata[coordinator + 2];

        outdata[coordinator_out] = binHSV(r, g, b,hL,hH,  sL, sH, vL, vH);
    }
}







extern "C" void binGreen(unsigned char*indata_h, unsigned char*outdata_h, int w, int h, float hL, float hH, float sL, float sH, float vL, float vH)
{

    int framesize = sizeof(unsigned  char)*w*h * 3;
    int framesizeout = sizeof(unsigned  char)*w*h;

    //device memory
    unsigned char*indata_d;
    checkCudaErrors(cudaMalloc((void **)&indata_d, framesize));
    unsigned char*outdata_d;
    checkCudaErrors(cudaMalloc((void **)&outdata_d, framesizeout));


    dim3 block(16, 16, 1); // 16x16 threads each block
    dim3 grid(iDivUp(w, block.x), iDivUp(h, block.y), 1); //nb block depends on image data

    //copy data from host to device
    checkCudaErrors(cudaMemcpy(indata_d, indata_h, framesize, cudaMemcpyHostToDevice));
    //call kernel function
    binKernel << <grid, block >> > (indata_d, outdata_d, w,h , hL, hH, sL, sH, vL, vH);

    //waiting for kernel function complete
    checkCudaErrors(cudaDeviceSynchronize());

    // data from device to host
    checkCudaErrors(cudaMemcpy(outdata_h, outdata_d, framesizeout, cudaMemcpyDeviceToHost));
    //free memory
    cudaFree(indata_d);
    cudaFree(outdata_d);
    cudaThreadExit();
}


