#include "cuda.h"
#include <cstdio>
#include <climits>
#include <algorithm>

using namespace std;

int* radixsort(int* T, int n) {
    cuInit(0);
    CUdevice cuDevice;
    cuDeviceGet(&cuDevice, 0);
    
    CUcontext cuContext;
    cuCtxCreate(&cuContext, 0, cuDevice);
   
    CUmodule cuModule = (CUmodule) 0;
    cuModuleLoad(&cuModule, "radixsort.ptx");
    
    CUfunction computeLocalPositions, computeGlobalPositions, permute;
    cuModuleGetFunction(&computeLocalPositions, cuModule, "computeLocalPositions");
    cuModuleGetFunction(&computeGlobalPositions, cuModule, "computeGlobalPositions");
    cuModuleGetFunction(&permute, cuModule, "permute");
	
	int blocks_per_grid = (n + 2047)/2048;
	int threads_per_block = 1024;	

	cuMemHostRegister(T, sizeof(int) * n, 0);

	CUdeviceptr in, pos, out, zerosInBlocks;
	cuMemAlloc(&in, sizeof(int) * n);
	cuMemAlloc(&pos, sizeof(int) * n);
	cuMemAlloc(&out, sizeof(int) * n);
	cuMemAlloc(&zerosInBlocks, sizeof(int) * blocks_per_grid);

	cuMemcpyHtoD(in, T, sizeof(int) * n);
	
	int* zerosInBlocksHost = new int[blocks_per_grid];
	cuMemHostRegister(zerosInBlocksHost, sizeof(int) * blocks_per_grid, 0);
	
	int* sorted = new int[n];
	cuMemHostRegister(sorted, sizeof(int) * n, 0);
	
	for(int k = 0; k < 31; k++) {
		void* args1[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuLaunchKernel(computeLocalPositions, blocks_per_grid, 1, 1, threads_per_block, 1, 1, 0, 0, args1, 0);

		cuMemcpyDtoH(zerosInBlocksHost, zerosInBlocks, sizeof(int) * blocks_per_grid);
		for(int i = 1; i < blocks_per_grid; i++)
			zerosInBlocksHost[i] += zerosInBlocksHost[i-1];
		cuMemcpyHtoD(zerosInBlocks, zerosInBlocksHost, sizeof(int) * blocks_per_grid);

		void* args2[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuLaunchKernel(computeGlobalPositions, blocks_per_grid, 1, 1, threads_per_block, 1, 1, 0, 0, args2, 0);
		
		void* args3[] = {&in, &n, &out, &pos};
		cuLaunchKernel(permute, blocks_per_grid, 1, 1, threads_per_block, 1, 1, 0, 0, args3, 0);
		
		swap(in, out);
	}

	cuMemcpyDtoH(sorted, in, sizeof(int) * n);
	
    cuCtxDestroy(cuContext);

	delete[] zerosInBlocksHost;

	return sorted; 
}
