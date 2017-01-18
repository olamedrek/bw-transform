#include "cuda.h"

const int THREADS_PER_BLOCK = 1024;

void cudaInit(CUcontext &cuContext);
void cudaDestory(CUcontext &cuContext);
int* radixsort(int* T, int n);
