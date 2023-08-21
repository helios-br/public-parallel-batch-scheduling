#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "Population.h"
#include "Individual.h"

using namespace std;

inline bool compareIndividualsByBiasedFitnessValue(Individual *firstIndividual, Individual *secondIndividual) {
	if (firstIndividual->biasedFitnessValue == secondIndividual->biasedFitnessValue) {
		return firstIndividual->schedule->cost < secondIndividual->schedule->cost;
	}

	return firstIndividual->biasedFitnessValue < secondIndividual->biasedFitnessValue;
}

inline void sortIndividualsByBiasedFitnessValue(vector<Individual*> &individuals) {
	sort(individuals.begin(), individuals.end(), compareIndividualsByBiasedFitnessValue);
}

inline bool compareIndividualsByDiversity(Individual *firstIndividual, Individual *secondIndividual) {
	return firstIndividual->averageDistanceToClosestIndividuals > secondIndividual->averageDistanceToClosestIndividuals;
}

inline void sortIndividualsByDiversity(vector<Individual*> &individuals) {
	sort(individuals.begin(), individuals.end(), compareIndividualsByDiversity);
}
