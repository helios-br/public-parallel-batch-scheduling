#pragma once

#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <bits/stdc++.h>
#include "../util/NumberUtil.h"

inline double calculateAcceptanceCriterion(double temperature, int currentScheduleCost, int newScheduleCost) {
	double delta = -1 * (newScheduleCost - currentScheduleCost);
	return exp(delta / temperature);
}

inline bool acceptWorstSolutionLE(double temperature, int currentScheduleCost, int newScheduleCost) {
	double randomNumber = generateRandomNumberBetween0And1();
	double acceptanceCriterion = calculateAcceptanceCriterion(temperature, currentScheduleCost, newScheduleCost);
	return randomNumber <= acceptanceCriterion;
}

inline bool acceptWorstSolutionLT(double temperature, int currentScheduleCost, int newScheduleCost) {
	double randomNumber = generateRandomNumberBetween0And1();
	double acceptanceCriterion = calculateAcceptanceCriterion(temperature, currentScheduleCost, newScheduleCost);
	return randomNumber < acceptanceCriterion;
}

