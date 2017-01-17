#include "bwTransform.h"
#include <cstdio>
#include <sys/mman.h>
#include <cstdlib>

using namespace std;

int main() {
	int n;
	scanf("%d", &n);

	char* in = new char[n + 2];
	in[0] = START_SYMBOL; 
	scanf("%s", in + 1);
	in[n+1] = END_SYMBOL;
	
	printf("Input:    %.*s\n", n, in + 1);

	char* out = bwEncode(in, n + 2);
	
	printf("Encoded:  %.*s\n", n + 2, out);
	
	char* dec = bwDecode(out, n + 2);
	
	printf("Decoded:  %.*s\n", n, dec + 1);

	delete[] in;
	delete[] out;
	delete[] dec;
	
	return 0;
}
