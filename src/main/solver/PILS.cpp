#include "PILS.h"

#include "TimeWindowDecomposition.h"
#include "Population.h"

using namespace std;

Schedule* PILS::solveInstance(Instance *instance) {
	Population population(this->maxPopulationSize, this->numberEliteIndividuals);

	// Population initialization
	Solver *twdSolver;
	for (int i = 0; i < this->minimumPopulationSize; i++) {
		twdSolver = new TimeWindowDecomposition(true);
		population.addIndividual(new Individual(twdSolver->solve(instance), numberCloseIndividuals));
		delete twdSolver;
	}

	Schedule *newSchedule = NULL;
	Schedule *newLocalSchedule = NULL;
	Schedule *bestSchedule = NULL;

	vector<Neighbourhood> localSearchProcedures { FULL_BATCH_INSERTION, FULL_BATCH_SWAP, FULL_JOB_INSERTION, FULL_JOB_SWAP, FULL_BATCH_MERGE_QUEIROGA };
	vector<Neighbourhood> perturbationMoves { RANDOM_BATCH_SPLIT_FIDELIS };

	int iterationsWithoutImprovements = 0;

	while (!executionController->stopped()) {

		newSchedule = cloneSchedule(population.getIndividualByBinaryTournament()->schedule);

		//executionController->totalIterations++;

		// Perturb
		executeRandomPerturbationMoves(1, 3, newSchedule, perturbationMoves);

		// Education (local search)
		for (int k = 0; k < localSearchProcedures.size() && !executionController->stopped(); k++) {
			newLocalSchedule = cloneSchedule(newSchedule);
			executeLocalSearch(newLocalSchedule, localSearchProcedures[k]);

			if (newLocalSchedule->cost < newSchedule->cost) {
				delete newSchedule;
				newSchedule = newLocalSchedule;
				shuffleNeighbourhoods(localSearchProcedures);
				k = 0;
			} else {
				delete newLocalSchedule;
			}
		}

		population.addIndividual(new Individual(newSchedule, numberCloseIndividuals));

		if (bestSchedule == NULL || newSchedule->cost < bestSchedule->cost) {
			iterationsWithoutImprovements = 0;
			delete bestSchedule;
			bestSchedule = cloneSchedule(newSchedule);
		} else if (iterationsWithoutImprovements > maxIterationsWithoutImprovements) {
			int numIndividualsToRetain = numberEliteIndividuals;
			int numIndividualsToRemove = population.individuals.size() - numIndividualsToRetain;
			population.removeIndividuals(numIndividualsToRemove);
			while (population.individuals.size() < this->minimumPopulationSize) {
				Individual *individual = population.individuals[generateRandomIndex(numberEliteIndividuals)];
				twdSolver = new TimeWindowDecomposition(individual->schedule->thres, individual->schedule->lookAheadParam);
				population.addIndividual(new Individual(twdSolver->solve(instance), numberCloseIndividuals));
				delete twdSolver;
			}
			iterationsWithoutImprovements = 0;
			//executionController->totalDiversifications++;
		} else {
			iterationsWithoutImprovements++;
		}

		// Select survivors
		if (population.individuals.size() == this->maxPopulationSize) {
			population.removeIndividuals(this->generationSize);
		}
	}

	return bestSchedule;
}

