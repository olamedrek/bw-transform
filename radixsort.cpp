#include "radixsort.h"
#include <cstdio>
#include <climits>
#include <algorithm>

using namespace std;

void cuCall(CUresult res) {
    if (res != CUDA_SUCCESS) {
        printf("cuda error: %d\n", res);
        exit(1);
    }
}

void cudaInit(CUcontext &cuContext) {
	cuCall(cuInit(0));
	CUdevice cuDevice;
	cuCall(cuDeviceGet(&cuDevice, 0));

	cuCall(cuCtxCreate(&cuContext, 0, cuDevice));
}

void cudaDestory(CUcontext &cuContext) {
	cuCall(cuCtxDestroy(cuContext));
}

int* radixsort(int* T, int n) {
	CUmodule cuModule = (CUmodule) 0;
	cuCall(cuModuleLoad(&cuModule, "radixsort.ptx"));

	CUfunction computeLocalPositions, computeGlobalPositions, permute;
	cuCall(cuModuleGetFunction(&computeLocalPositions, cuModule, "computeLocalPositions"));
	cuCall(cuModuleGetFunction(&computeGlobalPositions, cuModule, "computeGlobalPositions"));
	cuCall(cuModuleGetFunction(&permute, cuModule, "permute"));

	const int BLOCKS_PER_GRID = (n + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;

	cuCall(cuMemHostRegister(T, sizeof(int) * n * 3, 0));

	CUdeviceptr in, pos, out, zerosInBlocks;
	cuCall(cuMemAlloc(&in, sizeof(int) * n * 3));
	cuCall(cuMemAlloc(&pos, sizeof(int) * n));
	cuCall(cuMemAlloc(&out, sizeof(int) * n * 3));
	cuCall(cuMemAlloc(&zerosInBlocks, sizeof(int) * BLOCKS_PER_GRID));

	cuCall(cuMemcpyHtoD(in, T, sizeof(int) * n * 3));
	
	int* zerosInBlocksHost = new int[BLOCKS_PER_GRID];
	cuCall(cuMemHostRegister(zerosInBlocksHost, sizeof(int) * BLOCKS_PER_GRID, 0));
	
	int* sorted = new int[n * 3];
	cuCall(cuMemHostRegister(sorted, sizeof(int) * n * 3, 0));
	
	for(int k = 0; k < 63; k++) {
		void* args1[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuCall(cuLaunchKernel(computeLocalPositions, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args1, 0));

		cuCall(cuMemcpyDtoH(zerosInBlocksHost, zerosInBlocks, sizeof(int) * BLOCKS_PER_GRID));
		for(int i = 1; i < BLOCKS_PER_GRID; i++)
			zerosInBlocksHost[i] += zerosInBlocksHost[i-1];
		cuCall(cuMemcpyHtoD(zerosInBlocks, zerosInBlocksHost, sizeof(int) * BLOCKS_PER_GRID));

		void* args2[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuCall(cuLaunchKernel(computeGlobalPositions, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args2, 0));
		
		void* args3[] = {&in, &n, &out, &pos};
		cuCall(cuLaunchKernel(permute, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args3, 0));
		
		swap(in, out);
	}

	cuCall(cuMemcpyDtoH(sorted, in, sizeof(int) * n * 3));

	delete[] zerosInBlocksHost;

	return sorted; 
}
