#pragma once

#include <ctime>
#include <cstdlib>
#include <cmath>
#include <limits.h>
#include <random>

inline bool isSeedGenerated = false;
const double epsilon = 0.00001f;

inline void generateSeed() {
	if (!isSeedGenerated) {
		srand(time(NULL));
		isSeedGenerated = true;
	}
}

inline int generateRandomNumber() {
	generateSeed();
	return rand() % INT_MAX + 1;
}

inline int generateRandomIndex(int vectorSize) {
	generateSeed();
	return rand() % vectorSize;
}

inline int generateRandomNumberInRange(int firstPossibleValue, int lastPossibleValue) {
	generateSeed();
	return (rand() % (lastPossibleValue - firstPossibleValue + 1)) + firstPossibleValue;
}

inline double generateRandomNumberBetween0And1() {
	generateSeed();
	return (double) rand() / RAND_MAX;
}

inline bool isEqual(const double &value1, const double &value2) {
	return abs(value1 - value2) < epsilon;
}

inline bool lessThan(const double &value1, const double &value2) {
	return value1 < value2 - epsilon;
}

inline bool higherThan(const double &value1, const double &value2) {
    return (value1 - value2) > epsilon;
}
