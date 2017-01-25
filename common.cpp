#include "common.h"

void takeLastColumn(char* str, int* suffArray, int n, char* result) {
	for(int i = 0; i < n; i++) {
		result[i] = str[(suffArray[i] + n - 1) % n];
	}
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
