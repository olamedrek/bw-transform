#include "common.cpp"
#include <algorithm>
#include <cstdio>

using namespace std;

void computeSuffixArrayNonparallel(char* str, int n, int* res) {
    int *tmp = new int[n];
    int *pos = new int[n];

    for (int i = 0; i < n; i++) {
        res[i] = i;
        pos[i] = str[i];
    }

    int gap;
    auto comp = [n, &gap, &pos](int i, int j) {
        if (pos[i] != pos[j]) {
            return pos[i] < pos[j];
        }

        i += gap; j += gap;
        return (i < n && j < n ? pos[i] < pos[j] : i > j);
    };

    for (gap = 1; ; gap *= 2) {
        sort(res, res + n, comp);

        for (int i = 0; i < n-1; i++) tmp[i + 1] = tmp[i] + comp(res[i], res[i + 1]);
        for (int i = 0; i < n; i++) pos[res[i]] = tmp[i];

        if (tmp[n-1] == n-1) {
            delete[] tmp;
            delete[] pos;
            
            return ;
        }
    }
}

char* bwEncodeNonparallel(char* str, int n) {
	int* suffArray = new int[n];
	computeSuffixArrayNonparallel(str, n, suffArray);

	char* encoded = new char[n];
	takeLastColumn(str, suffArray, n, encoded);

	delete[] suffArray;
    
	return encoded;
}

int main() {
	int n;
	scanf("%d\n", &n);

	char* in = new char[n + 2];
	in[0] = START_SYMBOL; 
	fgets(in + 1, n + 1, stdin);
	in[n+1] = END_SYMBOL;
	
	printf("Input:\n%.*s\n\n", n, in + 1);

	char* out = bwEncodeNonparallel(in, n + 2);
	
	printf("Encoded:\n%.*s\n\n", n + 2, out);
	
	char* dec = bwDecode(out, n + 2);
	
	printf("Decoded:\n%.*s\n\n", n, dec + 1);

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

