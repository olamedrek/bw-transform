#include <cstdio>

extern "C" {
__global__
/* Result:
	[pos] - target positions in blocks only
	[zerosInBlocks] - for each block number of zeros in that block */
void computeLocalPositions(int* in, int n, int* pos, int k, int* zerosInBlocks) {
	int thid = blockDim.x * blockIdx.x + threadIdx.x;
	int id = threadIdx.x;

	__shared__ int sh[2048];

	int realShSize = 
		blockIdx.x == gridDim.x - 1 ? 
		n - blockIdx.x * 2048 : 2048;

	if(id >= realShSize) return;

	int bit = k < 32 ? 
		(in[3 * thid] >> k) & 1 : 
		(in[3 * thid + 1] >> (k - 32)) & 1;

	sh[id] = bit;

	for(int offset = 1; offset < 2048; offset *= 2) {
		__syncthreads();

		int tmp = 0;
		if(id >= offset)
			tmp = sh[id - offset];

		__syncthreads();

		sh[id] += tmp;
	}

	__syncthreads();

	int zeros = realShSize - sh[realShSize - 1];

	pos[thid] = bit ? 
		zeros + sh[id] - 1 : id - sh[id];

	if(id == 0)
		zerosInBlocks[blockIdx.x] = zeros;
}
}

extern "C" {
__global__
/* 
Parameters:
	[zerosPref] - prefix sum of array [zerosInBlock] (filled by function radixsort)
Result:
	[pos] - global target positions */
void computeGlobalPositions(int* in, int n, int* pos, int k, int* zerosPref) {
	int thid = blockDim.x * blockIdx.x + threadIdx.x;

	if(thid >= n) return;

	int bit = k < 32 ? 
		(in[3 * thid] >> k) & 1 : 
		(in[3 * thid + 1] >> (k - 32)) & 1;

	int elementsBefore = blockIdx.x * 2048;

	int zerosBefore = 
		blockIdx.x == 0 ? 
		0 : zerosPref[blockIdx.x - 1];

	int zerosAfter = zerosPref[gridDim.x - 1] - zerosPref[blockIdx.x];
	
	if(bit == 0) {
		pos[thid] += zerosBefore;
	} else {
		pos[thid] += zerosAfter + elementsBefore;
	}
}
}

extern "C" {
__global__
/* This function rewrites elements from array [in] to array [out] at their correct positions in order given in array [pos] */ 
void permute(int* in, int n, int* out, int* pos) {
	int thid = blockDim.x * blockIdx.x + threadIdx.x;

	if(thid >= n) return;

	out[3 * pos[thid]] = in[3 * thid];
	out[3 * pos[thid] + 1] = in[3 * thid + 1];
	out[3 * pos[thid] + 2] = in[3 * thid + 2];
}
}
