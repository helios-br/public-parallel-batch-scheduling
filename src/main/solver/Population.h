#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include "Individual.h"
#include "PopulationUtil.h"
#include "../schedule/Schedule.h"
#include "../schedule/ScheduleUtil.h"
#include "../util/NumberUtil.h"

using namespace std;

struct Population {

	int maxSize;
	vector<Individual*> individuals;
	int numEliteIndividuals;

public:

	Population(int maxSize) {
		this->maxSize = maxSize;
		this->numEliteIndividuals = -1;
	}

	Population(int maxSize, int numEliteIndividuals) {
		this->maxSize = maxSize;
		this->numEliteIndividuals = numEliteIndividuals;
	}

	~Population() {
		// first element is the best individual (dot not destroy it)
		for (int i = 1; i < individuals.size(); i++) {
			delete individuals[i];
		}
	}

	void addIndividualOrderByCostAsc(Individual *newIndividual) {
		int index = 0;
		for (; index < individuals.size(); index++) {
			if (newIndividual->schedule->cost < individuals[index]->schedule->cost) {
				break;
			}
		}
		individuals.insert(individuals.begin() + index, newIndividual);
	}

	void addIndividual(Individual *newIndividual) {
		// Calculate and add new distance
		for (int i = 0; i < individuals.size(); i++) {
			Individual *individual = individuals[i];
			int distance = newIndividual->addDistance(individual);
			individual->addDistance(DistanceData(newIndividual, distance));
		}

		newIndividual->rankCost = individuals.size() + 1;
		newIndividual->rankDiversity = individuals.size() + 1;

		individuals.push_back(newIndividual);

		// Update cost and diversity rankings
		for (int i = 0; i < individuals.size(); i++) {
			Individual *otherIndividual = individuals[i];
			if (newIndividual == otherIndividual) {
				continue;
			}

			if (newIndividual->schedule->cost < otherIndividual->schedule->cost) {
				newIndividual->rankCost--;
				otherIndividual->rankCost++;
			}
		}

		// Order individuals by diversity to find out their ranks
		sortIndividualsByDiversity(individuals);

		// Update biased fitness function value
		int numIndividuals = individuals.size();
		for (int i = 0; i < individuals.size(); i++) {
			Individual *individual = individuals[i];
			individual->rankDiversity = i + 1;
			individual->biasedFitnessValue = individual->rankCost + ((1 - (numEliteIndividuals / numIndividuals)) * individual->rankDiversity);
		}

		sortIndividualsByBiasedFitnessValue(individuals);
	}

	void removeIndividuals(int number) {
		// Remove last elements and update rankings
		for (int i = 0; i < number; i++) {
			Individual *individualToRemove = individuals[individuals.size() - 1];

			for (int j = 0; j < individuals.size(); j++) {
				Individual *otherIndividual = individuals[j];
				if (individualToRemove == otherIndividual) {
					continue;
				}

				if (individualToRemove->rankCost < otherIndividual->rankCost) {
					otherIndividual->rankCost--;
				}
			}

			delete individualToRemove;
			individuals.pop_back();
		}

		// Order individuals by diversity to find out their ranks
		sortIndividualsByDiversity(individuals);

		// Update biased fitness function value
		int numIndividuals = individuals.size();
		for (int i = 0; i < individuals.size(); i++) {
			Individual *individual = individuals[i];
			individual->rankDiversity = i + 1;
			individual->biasedFitnessValue = individual->rankCost + ((1 - (numEliteIndividuals / numIndividuals)) * individual->rankDiversity);
		}

		sortIndividualsByBiasedFitnessValue(individuals);
	}

	void removeIndividual(Individual *individualToRemove) {
		//cout << "Removing individual ";
		int individualToRemoveIndex = -1;

		for (int i = 0; i < individuals.size(); i++) {
			Individual *otherIndividual = individuals[i];

			if (individualToRemove == otherIndividual) {
				individualToRemoveIndex = i;
			} else {
				if (individualToRemoveIndex >= 0) {
					otherIndividual->rankCost--;
				}
			}
		}

		delete individualToRemove;
		individuals.erase(individuals.begin() + individualToRemoveIndex);
		//cout << individualToRemoveIndex;

		// Order individuals by diversity to find out their ranks
		sortIndividualsByDiversity(individuals);

		// Update biased fitness function value
		int numIndividuals = individuals.size();
		for (int i = 0; i < individuals.size(); i++) {
			Individual *individual = individuals[i];
			individual->rankDiversity = i + 1;
			individual->biasedFitnessValue = individual->rankCost + ((1 - (numEliteIndividuals / numIndividuals)) * individual->rankDiversity);
		}

		sortIndividualsByBiasedFitnessValue(individuals);
		//cout << " .. ok";
	}

	Individual* getIndividualRandomly() {
		return individuals[generateRandomIndex(individuals.size())];
	}

	Individual* getIndividualByBinaryTournament() {
		Individual *firstIndividual = getIndividualRandomly();
		Individual *secondIndividual = getIndividualRandomly();
		return secondIndividual->biasedFitnessValue < firstIndividual->biasedFitnessValue ? secondIndividual : firstIndividual;
	}
};
