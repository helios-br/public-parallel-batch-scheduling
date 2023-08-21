#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <iomanip>
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"
#include "../neighbourhood/Neighbourhood.h"

using namespace std;

struct ImprovementData {

	Neighbourhood neighbourhood;
	int iteration;
	int iterationsWithoutImprovement;
	int initialCost;
	int newCost;
	bool solutionImproved;

public:

	ImprovementData(Neighbourhood neighbourhood, int iteration, int iterationsWithoutImprovement, int initialCost, int newCost) {
		this->neighbourhood = neighbourhood;
		this->iteration = iteration;
		this->iterationsWithoutImprovement = iterationsWithoutImprovement;
		this->initialCost = initialCost;
		this->newCost = newCost;
		this->solutionImproved = newCost > initialCost;
	}

	void print() {
		cout << "Iteration: " << iteration << endl;
		cout << "Iterations without improvement: " << iterationsWithoutImprovement << endl;
		cout << "Initial cost: " << initialCost << " | After => " << newCost << endl;
		cout << "Perturbation: " << neighbourhoodToString(neighbourhood) << endl;
		cout << endl << endl;
	}
};
