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
#include "../../main/solver/Population.h"

inline void testAddRemoveIndividual() {
	// setup
	Instance *instance = loadTestInstance();

	Population population(5, 2);

	vector<Schedule*> schedules;
	for (int i = 0; i < 5; i++) {
		Solver *solver = new TimeWindowDecomposition(true);
		schedules.push_back(solver->solve(instance));
		delete solver;
	}

	// execute
	for (Schedule *schedule : schedules) {
		population.addIndividual(new Individual(schedule, 2));
	}

	cout << "-- AFTER INSERTIONS:" << endl;

	for (Individual *individual : population.individuals) {
		cout << "individual = ";
		cout << "biased value: " << individual->biasedFitnessValue << " | ";
		cout << "cost: " << individual->schedule->cost << "(" << individual->rankCost << ") | ";
		cout << "diversity: " << individual->averageDistanceToClosestIndividuals << "(" << individual->rankDiversity << ")" << endl;
	}

	// verify
	for (int i = 0; i < population.individuals.size() - 1; i++) {
		Individual *individual = population.individuals[i];
		for (int j = i + 1; j < population.individuals.size(); j++) {
			Individual *other = population.individuals[j];
			assert((individual->biasedFitnessValue <= other->biasedFitnessValue) && "Order of individuals is not correct");
			if (individual->rankCost < other->rankCost) {
				assert((individual->schedule->cost <= other->schedule->cost) && "Ranking by cost is wrong");
			}
			if (individual->rankDiversity < other->rankDiversity) {
				assert((individual->averageDistanceToClosestIndividuals >= other->averageDistanceToClosestIndividuals) && "Ranking by distance is wrong");
			}
		}
	}

	int sumAllRankingValues = 0;
	int sumAllRankingCosts = 0;
	int sumAllRankingDiversity = 0;
	for (int i = 0; i < population.individuals.size(); i++) {
		sumAllRankingValues += i + 1;
		sumAllRankingCosts += population.individuals[i]->rankCost;
		sumAllRankingDiversity += population.individuals[i]->rankDiversity;
	}

	assert((sumAllRankingValues == sumAllRankingCosts) && "Ranking values incorrect :: cost");
	assert((sumAllRankingValues == sumAllRankingDiversity) && "Ranking values incorrect :: diversity");

	// Remove elements

	population.removeIndividuals(2);

	cout << "-- AFTER EXCLUSIONS:" << endl;

	for (Individual *individual : population.individuals) {
		cout << "individual = ";
		cout << "biased value: " << individual->biasedFitnessValue << " | ";
		cout << "cost: " << individual->schedule->cost << "(" << individual->rankCost << ") | ";
		cout << "diversity: " << individual->averageDistanceToClosestIndividuals << "(" << individual->rankDiversity << ")" << endl;
	}

	assert((3 == population.individuals.size()) && "Population size is wrong");

	for (int i = 0; i < population.individuals.size() - 1; i++) {
		Individual *individual = population.individuals[i];
		for (int j = i + 1; j < population.individuals.size(); j++) {
			Individual *other = population.individuals[j];
			assert((individual->biasedFitnessValue <= other->biasedFitnessValue) && "Order of individuals is not correct");
			if (individual->rankCost < other->rankCost) {
				assert((individual->schedule->cost <= other->schedule->cost) && "Ranking by cost is wrong");
			}
			if (individual->rankDiversity < other->rankDiversity) {
				assert((individual->averageDistanceToClosestIndividuals >= other->averageDistanceToClosestIndividuals) && "Ranking by distance is wrong");
			}
		}
	}

	sumAllRankingValues = 0;
	sumAllRankingCosts = 0;
	sumAllRankingDiversity = 0;
	for (int i = 0; i < population.individuals.size(); i++) {
		sumAllRankingValues += i + 1;
		sumAllRankingCosts += population.individuals[i]->rankCost;
		sumAllRankingDiversity += population.individuals[i]->rankDiversity;
	}
	assert((sumAllRankingValues == sumAllRankingCosts) && "Ranking values incorrect :: cost");
	assert((sumAllRankingValues == sumAllRankingDiversity) && "Ranking values incorrect :: diversity");
}

inline void runPopulationTest() {
	cout << "# Running PopulationTest.h" << endl;

	cout << endl << ".... testAddRemoveIndividual [STARTED]" << endl;
	testAddRemoveIndividual();

	cout << "[OK] PopulationTest.h" << endl;
}
