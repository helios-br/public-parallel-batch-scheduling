#include "VNSBilyk.h"

#include "TWDBilyk.h"
#include "TimeWindowDecomposition.h"

using namespace std;

/**
 * Iterated Local Search
 */
Schedule* VNSBilyk::solveInstance(Instance *instance) {
	Solver *twdSolver = new TWDBilyk();
	Schedule *bestSchedule = twdSolver->solve(instance);
	this->initialCost = bestSchedule->cost;
	delete twdSolver;

	Schedule *newSchedule = NULL;
	Schedule *localSchedule = NULL;

	vector<Neighbourhood> localSearchProcedures { FULL_BATCH_SWAP_BILYK, FULL_SEQUENTIAL_JOB_INSERTION_BILYK, FULL_JOB_SWAP };
	vector<Neighbourhood> shakeMoves { RANDOM_BATCH_SPLIT_BILYK_PARAMETER, RANDOM_BATCH_INSERTION_BILYK, RANDOM_BATCH_SEQUENCE_INSERTION_BILYK_PARAMETER, RANDOM_BATCH_SWAP_BILYK, RANDOM_BATCH_SEQUENCE_SWAP_INSERTION_BILYK };

	int k = 1, l = 0;

	while (!executionController->stopped()) {

		// 1. Shaking
		newSchedule = cloneSchedule(bestSchedule);

		if (k <= 5) {
			l = 3 * (k - 1) + 1;
		} else if (k <= 10) {
			l = 3 * (k - 5);
		} else if (k <= 15) {
			l = 3 * (k - 11) + 1;
		} else if (k <= 20) {
			l = 3 * (k - 15);
		} else {
			l = 3 * (k - 21) + 1;
		}

		auto division = div(k, 5);
		auto shakeIndex = division.rem == 0 ? division.quot - 1 : division.quot;
		executePerturbationMove(newSchedule, shakeMoves[shakeIndex], l);

		// 2. Balancing (the workload of the machines is balanced)
		balanceSchedule(newSchedule);

		// 3. Local search procedure
		while(!executionController->stopped()) {
			localSchedule = cloneSchedule(newSchedule);

			for (int n = 0; n < localSearchProcedures.size() && !executionController->stopped(); n++) {
				executeLocalSearch(localSchedule, localSearchProcedures[n]);
			}

			if (localSchedule->cost < newSchedule->cost) {
				delete newSchedule;
				newSchedule = localSchedule;
			} else {
				delete localSchedule;
				break;
			}
		}

		// 4. Acceptance decision
		if (newSchedule->cost < bestSchedule->cost) {
			delete bestSchedule;
			bestSchedule = newSchedule;
			k = 1;
		} else {
			delete newSchedule;
			k = (k % 25) + 1;
		}
	}

	return bestSchedule;
}
