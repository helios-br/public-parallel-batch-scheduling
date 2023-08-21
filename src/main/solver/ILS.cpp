#include "ILS.h"

#include "Solver.h"
#include "../neighbourhood/Neighbourhood.h"
#include "TimeWindowDecomposition.h"

using namespace std;

/**
 * Iterated Local Search
 */
Schedule* ILS::solveInstance(Instance *instance) {

	// Create initial solution

	Solver *twdSolver = new TimeWindowDecomposition(true);
	Schedule *bestSchedule = twdSolver->solve(instance);
	delete twdSolver;

	Schedule *currentSchedule = NULL;
	Schedule *newSchedule = NULL;

	vector<Neighbourhood> localSearchProcedures { FULL_BATCH_INSERTION, FULL_BATCH_SWAP, FULL_JOB_INSERTION, FULL_JOB_SWAP, FULL_BATCH_MERGE_QUEIROGA };
	vector<Neighbourhood> perturbationMoves { RANDOM_BATCH_SPLIT_FIDELIS };

	int numPerturbations = 0;

	while (!executionController->stopped()) {

		currentSchedule = cloneSchedule(bestSchedule);

		// Perturbation

		numPerturbations = generateRandomNumberInRange(1, this->maxNumberPerturbationMoves);
		for (int i = 0; i < numPerturbations; i++) {
			int neighbourhoodIndex = generateRandomIndex(perturbationMoves.size());
			executePerturbationMove(currentSchedule, perturbationMoves[neighbourhoodIndex]);
		}

		// Local search

		shuffleNeighbourhoods(localSearchProcedures);

		for (int k = 0; k < localSearchProcedures.size() && !executionController->stopped(); k++) {
			newSchedule = cloneSchedule(currentSchedule);
			executeLocalSearch(newSchedule, localSearchProcedures[k]);

			if (newSchedule->cost < currentSchedule->cost) {
				delete currentSchedule;
				currentSchedule = newSchedule;
				k = 0;
				shuffleNeighbourhoods(localSearchProcedures);
			} else {
				delete newSchedule;
			}
		}

		// Update best schedule

		if (currentSchedule->cost < bestSchedule->cost) {
			delete bestSchedule;
			bestSchedule = currentSchedule;
		} else {
			delete currentSchedule;
		}
	}

	return bestSchedule;
}

