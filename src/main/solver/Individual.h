#pragma once

#include <iostream>
#include <vector>
#include <set>
#include "../neighbourhood/Neighbourhood.h"
#include "../schedule/Schedule.h"

using namespace std;

struct Individual;

struct BatchMatrix {

	vector<vector<int>> matrix;

public:

	BatchMatrix() {
		// default
	}

	BatchMatrix(Schedule *schedule) {
		matrix.resize(schedule->jobs.size());
		for (int i = 0; i < matrix.size(); i++) {
			matrix[i].resize(schedule->jobs.size(), 0);
		}

		set<int> jobsInMatrix;
		for (Job *job : schedule->jobs) {
			if (jobsInMatrix.count(job->instanceJob->id) > 0) {
				continue;
			}

			for (Job *batchJob : job->batch->jobs) {
				if (batchJob->isDummy()) {
					continue;
				}
				jobsInMatrix.insert(batchJob->instanceJob->id);
				matrix[job->instanceJob->id][batchJob->instanceJob->id] = 1;
			}
		}
	}

	int compare(BatchMatrix &other) {
		int totalEqualJobs = 0;

		for (int i = 0; i < matrix.size(); i++) {
			int totalEqualJobsInLine = 0;

			for (int j = 0; j < matrix[i].size(); j++) {
				if (matrix[i][j] != other.matrix[i][j]) {
					totalEqualJobsInLine = 0;
					break;
				}
				totalEqualJobsInLine += matrix[i][j];
			}

			totalEqualJobs += totalEqualJobsInLine;
		}

		return matrix.size() - totalEqualJobs;
	}

};

struct DistanceData {

	Individual *otherIndividual = NULL;
	int distance;

public:

	DistanceData(Individual *otherIndividual, int distance) {
		this->otherIndividual = otherIndividual;
		this->distance = distance;
	}
};

struct Individual {

	Schedule *schedule;
	BatchMatrix batchMatrix;
	vector<DistanceData> distances;
	double averageDistanceToClosestIndividuals;
	double biasedFitnessValue;
	int numClosestIndividuals;
	int rankCost;
	int rankDiversity;
	int iterWithoutImprovements = 0;

public:

	Individual(Schedule *schedule) {
		this->schedule = schedule;
	}

	Individual(Schedule *schedule, int numClosestIndividuals) {
		this->schedule = schedule;
		this->batchMatrix = BatchMatrix(schedule);
		this->numClosestIndividuals = numClosestIndividuals;
	}

	~Individual() {
		delete schedule;
		for (DistanceData distanceData : distances) {
			distanceData.otherIndividual->removeDistance(this);
		}
	}

	int calculateDistance(Individual *otherIndivual) {
		return batchMatrix.compare(otherIndivual->batchMatrix);
	}

	int addDistance(Individual *otherIndividual) {
		int distance = calculateDistance(otherIndividual);
		addDistance(DistanceData(otherIndividual, distance));
		return distance;
	}

	/**
	 * Insert new distance in the right position (vector must keep the order)
	 */
	void addDistance(DistanceData distanceData) {
		int index = 0;
		for (; index < distances.size(); index++) {
			if (distanceData.distance < distances[index].distance) {
				break;
			}
		}
		distances.insert(distances.begin() + index, distanceData);

		// Update average distance
		if (index < numClosestIndividuals) {
			calculateAverageDistanceToClosestIndividuals();
		}
	}

private:

	void removeDistance(Individual *otherIndividual) {
		int index = 0;
		for (; index < distances.size(); index++) {
			if (distances[index].otherIndividual == otherIndividual) {
				break;
			}
		}
		distances.erase(distances.begin() + index);

		// Update average distance only if necessary
		if (index < numClosestIndividuals) {
			calculateAverageDistanceToClosestIndividuals();
		}
	}

	/**
	 * Calculate the average distance to the closest individuals
	 */
	void calculateAverageDistanceToClosestIndividuals() {
		int divisor = distances.size() < numClosestIndividuals ? distances.size() : numClosestIndividuals;
		int totalDistance = 0;
		for (int i = 0; i < divisor; i++) {
			totalDistance += distances[i].distance;
		}
		averageDistanceToClosestIndividuals = totalDistance / divisor;
	}
};

struct Distance {

	Individual *individual;
	int distance;
};
