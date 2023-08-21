#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

inline void shuffle(vector<int> &originalvector) {
	auto rng = default_random_engine { };
	shuffle(begin(originalvector), end(originalvector), rng);
}

inline void printMatrix(vector<vector<int>> &matrix) {
	for (const auto &row : matrix) {
		for (int num : row) {
			cout << num << " ";
		}
		cout << endl;
	}
}
