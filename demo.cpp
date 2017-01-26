#include "bwTransform.h"
#include <cstdio>

using namespace std;

int main(int argc, char* argv[]) {
	bool verbose = (argc > 1 && string(argv[1]) == VERBOSE_FLAG);

	int n;
	scanf("%d\n", &n);

	char* in = new char[n + 2];
	in[0] = START_SYMBOL; 
	fgets(in + 1, n + 1, stdin);
	in[n+1] = END_SYMBOL;
	
	if (verbose) {
		printf("Input:\n%.*s\n\n", n, in + 1);
	}

	char* out = bwEncode(in, n + 2);
	
	if (verbose) {
		printf("Encoded:\n%.*s\n\n", n + 2, out);
	}
	
	char* dec = bwDecode(out, n + 2);
	
	if (verbose) {
		printf("Decoded:\n%.*s\n\n", n, dec + 1);
	}

	bool ok = true;
	for (int i = 0; i < n + 2; i++) if (in[i] != dec[i]) {
		ok = false;
		break;
	}
	
	if (ok) {
		printf("OK\n");
	} else {
		printf("DECODED STRING INVALID\n");
	}

	delete[] in;
	delete[] out;
	delete[] dec;
	
	return 0;
}
