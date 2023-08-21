#pragma once

#include <iostream>
#include <set>
#include <assert.h>
#include "../../main/util/VectorUtil.h"
#include "../../main/util/NumberUtil.h"
#include "../instance/InstanceTestHelper.h"
#include "../../main/schedule/ScheduleValidator.h"
#include "../../main/solver/Solver.h"
#include "../../main/solver/TimeWindowDecomposition.h"
#include "../../main/solver/Individual.h"
#include "../../main/solver/Population.h"
#include "../../main/solver/PopulationUtil.h"

inline void testBatchMatrixGeneration() {
	Instance *instance = loadTestInstance();
	Solver *solver = new TimeWindowDecomposition(true);
	Schedule *schedule = solver->solve(instance);
	BatchMatrix batchMatrix = BatchMatrix(schedule);

	int totalPositions1 = 0;

	set<Job*> jobs;
	for (int i = 0; i < schedule->jobs.size(); i++) {
		Job *job = schedule->jobs[i];
		Batch *batch = job->batch;

		if (jobs.count(job) > 0) {
			continue;
		}

		for (int j = 0; j < schedule->jobs.size(); j++) {
			Job *otherJob = schedule->jobs[j];
			Batch *otherBatch = otherJob->batch;

			if (batch == otherBatch) {
				jobs.insert(otherJob);
				assert((batchMatrix.matrix[i][j] == 1) && "Value in matrix position should be 1");
				totalPositions1 += 1;
			} else {
				assert((batchMatrix.matrix[i][j] == 0) && "Value in matrix position should be 0");
			}

		}
	}

	cout << "Total jobs: " << schedule->jobs.size() << " | Total positions marked in matrix (1): " << totalPositions1 << endl;
	assert((totalPositions1 == schedule->jobs.size()) && "Total positive positions marked in matrix is wrong :: test1");

	totalPositions1 = 0;
	for (int i = 0; i < schedule->jobs.size(); i++) {
		for (int j = 0; j < schedule->jobs.size(); j++) {
			totalPositions1 += batchMatrix.matrix[i][j];
		}
	}

	cout << "Total jobs: " << schedule->jobs.size() << " | Total positions marked in matrix (2): " << totalPositions1 << endl;
	assert((totalPositions1 == schedule->jobs.size()) && "Total positive positions marked in matrix is wrong :: test2");
}

inline void testMatrixComparison() {
	Instance *instance = loadTestInstance();
	Solver *solver1 = new TimeWindowDecomposition(true);
	Schedule *schedule1 = solver1->solve(instance);
	BatchMatrix batchMatrix1 = BatchMatrix(schedule1);
	Solver *solver2 = new TimeWindowDecomposition(true);
	Schedule *schedule2 = solver2->solve(instance);
	BatchMatrix batchMatrix2 = BatchMatrix(schedule2);

	int distance1 = batchMatrix1.compare(batchMatrix2);
	if (schedule1->cost != schedule2->cost) {
		assert((distance1 > 0) && "Distances should be higher than zero :: distance1: " + distance1);
	}
	int distance2 = batchMatrix2.compare(batchMatrix1);
	if (schedule1->cost != schedule2->cost) {
		assert((distance2 > 0) && "Distances should be higher than zero :: distance2: " + distance2);
	}
	assert((distance1 == distance2) && "Distances should be equal");
	assert((batchMatrix1.compare(batchMatrix1) == 0) && "Distances should be 0 :: batchMatrix1");
	assert((batchMatrix2.compare(batchMatrix2) == 0) && "Distances should be 0 :: batchMatrix2");

	delete solver1;
	delete solver2;
	delete schedule1;
	delete schedule2;
}

inline void testMatrixDetailedComparison() {
	Instance *instance = loadTestInstance();
	Solver *solver1 = new TimeWindowDecomposition(true);
	Schedule *schedule1 = solver1->solve(instance);
	BatchMatrix batchMatrix1 = BatchMatrix(schedule1);
	Solver *solver2 = new TimeWindowDecomposition(true);
	Schedule *schedule2 = solver2->solve(instance);
	BatchMatrix batchMatrix2 = BatchMatrix(schedule2);
	int distance = batchMatrix1.compare(batchMatrix2);

	set<Batch*> batches;
	set<int> jobs;

	int totalPositions1 = 0;

	for (int i = 0; i < schedule1->jobs.size(); i++) {
		Job *job1 = schedule1->jobs[i];
		Batch *batch1 = job1->batch;

		if (jobs.count(job1->instanceJob->id) > 0) {
			continue;
		}

		set<int> jobsInBatch;

		for (Job *jobBatch1 : batch1->jobs) {
			if (jobBatch1->isDummy()) {
				continue;
			}
			jobsInBatch.insert(jobBatch1->instanceJob->id);
			jobs.insert(jobBatch1->instanceJob->id);
		}

		Batch *batch2 = NULL;
		for (int jobId : jobsInBatch) {
			Job *job2 = schedule2->jobs[jobId];
			if (batch2 == NULL) {
				batch2 = job2->batch;
			} else {
				if (batch2 != job2->batch) {
					totalPositions1 += jobsInBatch.size();
					break;
				}
			}
		}
	}

	cout << "Total jobs: " << schedule1->jobs.size() << "| Distance: " << distance << " | TotalPositions1: " << totalPositions1 << endl;
	assert((distance == totalPositions1) && "Values should be equal");

	delete solver1;
	delete solver2;
	delete schedule1;
	delete schedule2;
}

inline void testAddDistance() {
	// setup
	Instance *instance = loadTestInstance();

	vector<Schedule*> schedules;
	for (int i = 0; i < 5; i++) {
		Solver *solver = new TimeWindowDecomposition(true);
		schedules.push_back(solver->solve(instance));
		delete solver;
	}

	vector<Individual*> individuals;
	for (Schedule *schedule : schedules) {
		individuals.push_back(new Individual(schedule, 2));
	}

	// execute
	for (Individual *individual : individuals) {
		for (Individual *other : individuals) {
			if (individual == other) {
				continue;
			}
			individual->addDistance(other);
		}
	}

	// verify
	for (Individual *individual : individuals) {
		assert((individual->distances.size() == 4) && "Distance vector size is wrong");
		for (int d = 0; d < individual->distances.size() - 1; d++) {
			DistanceData data = individual->distances[d];
			DistanceData nextData = individual->distances[d + 1];
			assert((data.otherIndividual != individual) && "Other individual should be different from this");
			assert((data.otherIndividual != nextData.otherIndividual) && "Other individual should be different from next");
			assert((data.distance <= nextData.distance) && "Next data should have higher distance");
		}
		double averageDistance = (individual->distances[0].distance + individual->distances[1].distance) / 2;
		assert((isEqual(individual->averageDistanceToClosestIndividuals, averageDistance)) && "Average distance is different");
	}

	Individual *individual = individuals[0];
	double averageDistance = individual->averageDistanceToClosestIndividuals;
	DistanceData lastData = individual->distances.back();
	delete lastData.otherIndividual;
	assert((isEqual(individual->averageDistanceToClosestIndividuals, averageDistance)) && "Average distance is different after removing last element");
	assert((individual->distances.size() == 3) && "Distance vector size is wrong (should be 3)");

	DistanceData firstData = individual->distances[0];
	double newAverrageDistance = (individual->distances[1].distance + individual->distances[2].distance) / 2;
	delete firstData.otherIndividual;
	assert((isEqual(individual->averageDistanceToClosestIndividuals, newAverrageDistance)) && "New average distance is wrong");
	assert((individual->distances.size() == 2) && "Distance vector size is wrong (should be 2)");
}

inline void testIndividualSortingByBiasedFitnessValue() {
	// setup
	Instance *instance = loadTestInstance();

	vector<Schedule*> schedules;
	for (int i = 0; i < 5; i++) {
		Solver *solver = new TimeWindowDecomposition(true);
		schedules.push_back(solver->solve(instance));
		delete solver;
	}

	vector<Individual*> individuals;
	for (Schedule *schedule : schedules) {
		individuals.push_back(new Individual(schedule, 2));
	}

	individuals[0]->biasedFitnessValue = 10;
	individuals[1]->biasedFitnessValue = 5;
	individuals[2]->biasedFitnessValue = 5;
	individuals[3]->biasedFitnessValue = 15;
	individuals[4]->biasedFitnessValue = 5;

	sortIndividualsByBiasedFitnessValue(individuals);

	for (Individual *individual : individuals) {
		cout << "individual = ";
		cout << "biased value: " << individual->biasedFitnessValue << " | ";
		cout << "cost: " << individual->schedule->cost << "(" << individual->rankCost << ") | ";
		cout << "diversity: " << individual->averageDistanceToClosestIndividuals << "(" << individual->rankDiversity << ")" << endl;
	}

	cout << "individual[0]->cost: " << individuals[0]->schedule->cost << endl;
	cout << "individual[1]->cost: " << individuals[1]->schedule->cost << endl;
	cout << "individual[2]->cost: " << individuals[2]->schedule->cost << endl;

	assert((individuals[0]->biasedFitnessValue == 5) && "Biased fitness value should be 5");
	assert((individuals[0]->schedule->cost <= individuals[1]->schedule->cost) && "Cost of 0 shoud be lower than 1");
	assert((individuals[1]->biasedFitnessValue == 5) && "Biased fitness value should be 5");
	assert((individuals[1]->schedule->cost <= individuals[2]->schedule->cost) && "Cost of 1 shoud be lower than 2");
	assert((individuals[2]->biasedFitnessValue == 5) && "Biased fitness value should be 5");
	assert((individuals[3]->biasedFitnessValue == 10) && "Biased fitness value should be 10");
	assert((individuals[4]->biasedFitnessValue == 15) && "Biased fitness value should be 15");
}

inline void runIndividualTest() {
	cout << "# Running IndividualTest.h" << endl;

	cout << endl << ".... testMatrixComparison [STARTED]" << endl;
	testMatrixComparison();
	cout << endl << ".... testBatchMatrixGeneration [STARTED]" << endl;
	testBatchMatrixGeneration();
	cout << endl << ".... testMatrixDetailedComparison [STARTED]" << endl;
	testMatrixDetailedComparison();
	cout << endl << ".... testAddDistance [STARTED]" << endl;
	testAddDistance();
	cout << endl << ".... testIndividualSortingByBiasedFitnessValue [STARTED]" << endl;
	testIndividualSortingByBiasedFitnessValue();

	cout << "[OK] IndividualTest.h" << endl;
}
