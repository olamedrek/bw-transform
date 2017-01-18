#include "cuda.h"
#include "radixsort.h"
#include "bwTransform.h"
#include <cstdio>
#include <climits>
#include <algorithm>

using namespace std;

void computeSuffixArray(char* str, int n, int* res) {
	int* curr = new int[3 * n];
	int positions[n]; 

	for(int i = 0; i < n; i++) {
		positions[i] = str[i];
	}

	for(int len = 1; len < n; len *= 2) {
		for(int i = 0; i < n; i++) {
			curr[3 * i] = positions[i];
			curr[3 * i + 1] = i + len < n ? positions[i + len] : -1;
			curr[3 * i + 2] = i;
		}

		curr = radixsort(curr, n);

		for(int i = 0, prevPos = -1; i < n; i++) {
			if(i > 0 && curr[3 * i] == curr[3 * (i-1)] && curr[3 * i + 1] == curr[3 * (i-1) + 1]) { 
				positions[curr[3 * i + 2]] = prevPos;
			} else {
				positions[curr[3 * i + 2]] = ++prevPos;
			}
		}
	}

	for(int i = 0; i < n; i++) {
		res[positions[i]] = i;
	}

	delete[] curr;
}

void takeLastColumn(char* str, int* suffArray, int n, char* result) {
	for(int i = 0; i < n; i++) {
		result[i] = str[(suffArray[i] + n - 1) % n];
	}
}

char* bwEncode(char* str, int n) {
	int* suffArray = new int[n];
	computeSuffixArray(str, n, suffArray);

	char* encoded = new char[n];
	takeLastColumn(str, suffArray, n, encoded);

	delete[] suffArray;
    
	return encoded;
}

char* bwDecode(char* encoded, int n) {
	const int MAXCHAR = 256;
	int cnt[MAXCHAR];
	
	int *id = new int[n];

	for(int i = 0; i < MAXCHAR; i++) {
		cnt[i] = 0;
	}

	for(int i = 0; i < n; i++) {
		char c = encoded[i];
		id[i] = cnt[c]++;
	}
	
	for(int i = 1; i < MAXCHAR; i++) {
		cnt[i] += cnt[i-1];
	}

	char *decoded = new char[n];

	int j = n - 1;
	decoded[n - 1] = END_SYMBOL;

	for(int i = n - 2; i >= 0; i--) {
		char c = encoded[j];
		decoded[i] = c;
		
		int prevEqual = id[j];
		int totalSmaller = (c > 0 ? cnt[c-1] : 0);
		
		j = totalSmaller + prevEqual;
	}
	
	delete[] id;

	return decoded;
}

