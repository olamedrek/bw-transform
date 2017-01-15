#include "cuda.h"
#include "radixsort.h"
#include <cstdio>
#include <climits>
#include <algorithm>

using namespace std;

void bubblesort(int curr[][3], int n) {
	for(int i = 0; i < n; i++) {
		for(int j = 1; j < n; j++) {
			if(curr[j][0] < curr[j-1][0] || (curr[j][0] == curr[j-1][0] && curr[j][1] < curr[j-1][1])) {
				swap(curr[j], curr[j-1]);
			}
		}
	}
}

void computeSuffixArray(char* str, int n, int* res) {
	int curr[n][3];
	int positions[n]; 

	for(int i = 0; i < n; i++) {
		positions[i] = str[i];
	}

	for(int len = 1; len <= n; len++) {
		for(int i = 0; i < n; i++) {
			curr[i][0] = positions[i];
			curr[i][1] = i + len - 1 < n ? positions[i + len - 1] : -1;
			curr[i][2] = i;
		}

		bubblesort(curr, n);

		for(int i = 0, prevPos = -1; i < n; i++) {
			if(i > 0 && curr[i][0] == curr[i-1][0] && curr[i][1] == curr[i-1][1]) { 
				positions[curr[i][2]] = prevPos;
			} else {
				positions[curr[i][2]] = ++prevPos;
			}
		}	
	}

	for(int i = 0; i < n; i++) {
		res[positions[i]] = i;
	}
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
