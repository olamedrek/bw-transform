#include "radixsort.h"
#include <cstdio>
#include <climits>
#include <algorithm>

using namespace std;

void cudaInit(CUcontext &cuContext) {
	cuInit(0);
	CUdevice cuDevice;
	cuDeviceGet(&cuDevice, 0);

	cuCtxCreate(&cuContext, 0, cuDevice);
}

void cudaDestory(CUcontext &cuContext) {
	cuCtxDestroy(cuContext);
}

int* radixsort(int* T, int n) {
	CUmodule cuModule = (CUmodule) 0;
	cuModuleLoad(&cuModule, "radixsort.ptx");

	CUfunction computeLocalPositions, computeGlobalPositions, permute;
	cuModuleGetFunction(&computeLocalPositions, cuModule, "computeLocalPositions");
	cuModuleGetFunction(&computeGlobalPositions, cuModule, "computeGlobalPositions");
	cuModuleGetFunction(&permute, cuModule, "permute");

	const int BLOCKS_PER_GRID = (n + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;

	cuMemHostRegister(T, sizeof(int) * n * 3, 0);

	CUdeviceptr in, pos, out, zerosInBlocks;
	cuMemAlloc(&in, sizeof(int) * n * 3);
	cuMemAlloc(&pos, sizeof(int) * n);
	cuMemAlloc(&out, sizeof(int) * n * 3);
	cuMemAlloc(&zerosInBlocks, sizeof(int) * BLOCKS_PER_GRID);

	cuMemcpyHtoD(in, T, sizeof(int) * n * 3);
	
	int* zerosInBlocksHost = new int[BLOCKS_PER_GRID];
	cuMemHostRegister(zerosInBlocksHost, sizeof(int) * BLOCKS_PER_GRID, 0);
	
	int* sorted = new int[n * 3];
	cuMemHostRegister(sorted, sizeof(int) * n * 3, 0);
	
	for(int k = 32; k < 63; k++) {
		void* args1[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuLaunchKernel(computeLocalPositions, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args1, 0);

		cuMemcpyDtoH(zerosInBlocksHost, zerosInBlocks, sizeof(int) * BLOCKS_PER_GRID);
		for(int i = 1; i < BLOCKS_PER_GRID; i++)
			zerosInBlocksHost[i] += zerosInBlocksHost[i-1];
		cuMemcpyHtoD(zerosInBlocks, zerosInBlocksHost, sizeof(int) * BLOCKS_PER_GRID);

		void* args2[] = {&in, &n, &pos, &k, &zerosInBlocks};
		cuLaunchKernel(computeGlobalPositions, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args2, 0);
		
		void* args3[] = {&in, &n, &out, &pos};
		cuLaunchKernel(permute, BLOCKS_PER_GRID, 1, 1, THREADS_PER_BLOCK, 1, 1, 0, 0, args3, 0);
		
		swap(in, out);

		if(k == 62) k = -1;
		if(k == 30) break;
	}

	cuMemcpyDtoH(sorted, in, sizeof(int) * n * 3);

	delete[] zerosInBlocksHost;

	return sorted; 
}
