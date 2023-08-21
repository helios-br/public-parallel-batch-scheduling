#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <iomanip>
#include "ImprovementData.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"
#include "../neighbourhood/Neighbourhood.h"

using namespace std;
using namespace chrono;

struct ExecutionController {

	time_point<steady_clock> startTime;
	int stopCriterion; // seconds
	bool halt = false;
	int executionTime; // seconds
	vector<ImprovementData> improvements;
	int totalIterations = 0;
	int bestParameter = -1;
	int totalDiversifications = 0;

public:

	ExecutionController(Instance *instance) {
		this->stopCriterion = instance->instanceJobs.size() * 0.1; // 0.05 or 0.1 or 0.5
	}

	ExecutionController(int stopCriterion) {
		this->stopCriterion = stopCriterion;
	}

	void start() {
		this->startTime = steady_clock::now();
		this->halt = false;
	}

	bool stopped() {
		if (!this->halt) {
			this->halt = getExecutionTimeInSeconds() > this->stopCriterion;
		}
		return this->halt;
	}

	void stop() {
		if (!this->halt) {
			this->halt = true;
			getExecutionTimeInSeconds();
		}
	}

	void addImprovement(Neighbourhood perturbation, int iteration, int iterationsWithoutImprovement, int initialCost, int newCost) {
		improvements.push_back(ImprovementData(perturbation, iteration, iterationsWithoutImprovement, initialCost, newCost));
	}

private:

	int getExecutionTimeInSeconds() {
		time_point<steady_clock> completionTime = steady_clock::now();
		executionTime = duration_cast<seconds>(completionTime - this->startTime).count();
		return executionTime;
	}
};

class Solver {

public:

	ExecutionController *executionController = NULL;
	double initialCost = -1;

	virtual ~Solver() {
		delete executionController;
	}

	Schedule* solve(Instance *instance) {
		delete executionController;
		executionController = new ExecutionController(instance);
		executionController->start();
		Schedule *schedule = solveInstance(instance);
		executionController->stop();
		return schedule;
	}

	virtual int getSolverInstanceId() = 0;

private:

	virtual Schedule* solveInstance(Instance *instance) = 0;

};
