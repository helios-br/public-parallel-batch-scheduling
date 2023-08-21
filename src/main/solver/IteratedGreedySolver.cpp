#include "IteratedGreedySolver.h"

#include "TWDBilyk.h"
#include "MetropolisRule.h"
#include "../neighbourhood/SequentialBatchInsertion.h"
#include "../neighbourhood/SequentialJobInsertion.h"

using namespace std;

// For all the algorithms we use the same stopping criterion which is a given elapsed CPU time equal to t ∗ n seconds,
// where n is the number of jobs and t is a fixed factor. We use t = 0.05 and t = 0.1

// For IG algorithm, the best values found for its parameters were: ddmax = n/6, numB = 0.3∗tB, where tB is the total number of batches in the solution.

// In the calibration experiments, IG and SA were run with 0.1 ∗ n seconds as stopping criterion.

// For all the algorithms we use the same stopping criterion which is a given elapsed CPU time equal  jobs and t is a fixed factor. We use t = 0.05 and t = 0.1

/**
 * Iterated Greedy metaheuristic
 * Fidelis, Michele B. Arroyo, Jose Elias C.
 * Meta-heuristic algorithms for scheduling on parallel batch machines with unequal job ready times
 * 2017 IEEE International Conference on Systems, Man, and Cybernetics (SMC)
 */
Schedule* IteratedGreedySolver::solveInstance(Instance *instance) {
	// Algorithm parameters
	int maxDestructionSize = instance->instanceJobs.size() / 6;
	int destructionSize = 1;
	double constantTemperature = calculateConstantTemperature(instance);

	// Create initial solution
	Solver *twdSolver = new TWDBilyk();
	Schedule *currentSchedule = twdSolver->solve(instance);
	this->initialCost = currentSchedule->cost;
	delete twdSolver;

	// Calculate initial solution cost
	executeLocalSearchProcedure(currentSchedule);

	Schedule *bestSchedule = currentSchedule;
	Schedule *newSchedule;

	// Greedy iteration
	while (!executionController->stopped()) {
		newSchedule = cloneSchedule(currentSchedule);
		destroyAndConstruct(newSchedule, destructionSize);
		executeLocalSearchProcedure(newSchedule);

		if (newSchedule->cost < currentSchedule->cost) {

			destructionSize = 1;
			if (currentSchedule != bestSchedule) {
				delete currentSchedule;
			}

			currentSchedule = newSchedule;

			if (newSchedule->cost < bestSchedule->cost) {
				delete bestSchedule;
				bestSchedule = newSchedule;
			}
		} else {
			destructionSize += 1;
			if (destructionSize > maxDestructionSize) {
				destructionSize = 1;
			}
			if (acceptWorstSolutionLE(constantTemperature, currentSchedule->cost, newSchedule->cost)) {
				if (currentSchedule != bestSchedule) {
					delete currentSchedule;
				}
				currentSchedule = newSchedule;
			} else {
				delete newSchedule;
			}
		}
	}

	return bestSchedule;
}

double IteratedGreedySolver::calculateConstantTemperature(Instance *instance) {
	double divisor = instance->instanceJobs.size() * instance->instanceMachines.size() * 10;
	double dividend = 0;
	for (InstanceJob *instanceJob : instance->instanceJobs) {
		dividend += instanceJob->processingTime;
	}
	return dividend / divisor;
}

void IteratedGreedySolver::executeLocalSearchProcedure(Schedule *schedule) {
	// The LS is based on batch insertions, that is, a batch is removed from its original position and inserted into all the possible positions of the schedule
	// The LS procedure receives a solution S and the maximum number of batches numB to be removed randomly.
	// In this procedure we use the best improvement strategy, that is, for each removed batch, all the insertion possibilities are tested and the best neighbor solution SB is considered.
	// For IG algorithm, the best values found for its parameters were: ddmax = n/6, numB = 0.3∗tB, where tB is the total number of batches in the solution.

	int numB = 0.3 * schedule->totalBatches();
	int numBatchesToBeRemoved = generateRandomNumberInRange(1, numB);
	executeFullSequentialBatchInsertion(schedule, numBatchesToBeRemoved);
}

void IteratedGreedySolver::destroyAndConstruct(Schedule *schedule, int destructionSize) {
	// "... In the first phase, named Destruction, d_s jobs are randomly chosen and removed from batches that are also randomly selected. "
	// This phase generates a partial solution S_p and the set of removed jobs, which is denotaed as J_r."

	// "In the second phase, named Construction, the jobs of J_r are sequentially re-inserted in the partial solution by using a greedy insertion heuristic. "
	// That is, each job is inserted in a feasible batch that provides the smallest total weighted tardiness value."

	executeFullSequentialJobInsertion(schedule, destructionSize);
}
