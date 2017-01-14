#include "bwTransform.h"
#include <cstdio>
#include <sys/mman.h>
#include <cstdlib>
#include <ctime>
#include <cassert>

using namespace std;

int main() {

	int n;
	scanf("%d", &n);

	char* in = new char[n + 2];
	in[0] = '^'; 
	scanf("%s", in + 1);
	in[n+1] = '|';

	char* out = bwEncode(in, n + 2);
	
	printf("%s\n", out);

	delete[] in;
	delete[] out;
	
	return 0;
}
