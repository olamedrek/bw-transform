#include <iostream>

using namespace std;

string bwEncode(const string &s) {
	return s;
}

int main() {
	ios_base::sync_with_stdio(false);

	string input;
	cin >> input;

	cout << bwTransform(input);

	return 0;
}
