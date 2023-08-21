#include "SimulatedAnnealing.h"

#include "TWDBilyk.h"
#include "MetropolisRule.h"

using namespace std;

/**
 * Simulated Annealing
 */
Schedule* SimulatedAnnealing::solveInstance(Instance *instance) {
	Solver *twdSolver = new TWDBilyk();
	Schedule *currentSchedule = twdSolver->solve(instance);
	this->initialCost = currentSchedule->cost;
	delete twdSolver;

	Schedule *bestLocal = currentSchedule;
	Schedule *bestSchedule = NULL;
	Schedule *newSchedule = NULL;

	// Number of solutions evaluated for each temperature
	int nMax = instance->instanceJobs.size() / 10;

	while (!executionController->stopped()) {
		double currentTemperature = this->initialTemperature;

		while (currentTemperature > 0.0001) {

			// nMax: number of solutions evaluated for each temperature
			for (int iter = 0; iter < nMax && !executionController->stopped(); iter++) {

				// Perturb
				newSchedule = perturb(currentSchedule);

				if (newSchedule->cost < currentSchedule->cost) {

					// Update current schedule
					if (currentSchedule != bestLocal && currentSchedule != bestSchedule) {
						delete currentSchedule;
					}
					currentSchedule = newSchedule;

					// Update best schedule
					if (newSchedule->cost < bestLocal->cost) {
						if (bestLocal != bestSchedule) {
							delete bestLocal;
						}
						bestLocal = newSchedule;
					}
				} else if (acceptWorstSolutionLT(currentTemperature, currentSchedule->cost, newSchedule->cost)) {
					if (currentSchedule != bestLocal && currentSchedule != bestSchedule) {
						delete currentSchedule;
					}
					currentSchedule = newSchedule;
				} else {
					delete newSchedule;
				}
			}

			currentTemperature = currentTemperature * this->coolingRate;
		}

		// Update current schedule

		if (currentSchedule != bestLocal && currentSchedule != bestSchedule) {
			delete currentSchedule;
		}
		currentSchedule = cloneSchedule(bestLocal);

		if (bestSchedule == NULL || bestLocal->cost < bestSchedule->cost) {
			delete bestSchedule;
			bestSchedule = bestLocal;
			executeBatchSingletonInsertion(currentSchedule);
		} else {
			executeJobInsertionFromSmallestBatch(currentSchedule);
		}
	}

	return bestSchedule;
}

Schedule* SimulatedAnnealing::perturb(Schedule *schedule) {
	Schedule *bestSchedule = NULL;
	Schedule *newSchedule = NULL;

	vector<Neighbourhood> perturbationMoves { RANDOM_BATCH_SWAP_TARDINESS, RANDOM_BATCH_INSERTION, RANDOM_BATCH_MERGE_FIDELIS, RANDOM_BATCH_SPLIT_FIDELIS,
			RANDOM_JOB_SWAP, RANDOM_JOB_INSERTION };

	for (Neighbourhood neighbourhood : perturbationMoves) {
		newSchedule = cloneSchedule(schedule);
		executePerturbationMove(newSchedule, neighbourhood);
		if (bestSchedule == NULL || newSchedule->cost < bestSchedule->cost) {
			delete bestSchedule;
			bestSchedule = newSchedule;
		} else {
			delete newSchedule;
		}
	}

	return bestSchedule;
}
