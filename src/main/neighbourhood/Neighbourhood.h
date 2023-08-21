#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

#include "../neighbourhood/BatchInsertion.h"
#include "../neighbourhood/BatchMerge.h"
#include "../neighbourhood/BatchSplit.h"
#include "../neighbourhood/BatchSwap.h"
#include "../neighbourhood/JobInsertion.h"
#include "../neighbourhood/JobSwap.h"
#include "../neighbourhood/SequentialBatchInsertion.h"
#include "../neighbourhood/SequentialJobInsertion.h"
#include "../neighbourhood/JobFastInsertion.h"
#include "../util/NumberUtil.h"
#include "../util/VectorUtil.h"

#define stringify( name ) #name

enum Neighbourhood {

	FULL_BATCH_INSERTION = 0,
	FULL_BATCH_SWAP = 1,
	FULL_JOB_INSERTION = 2,
	FULL_JOB_SWAP = 3,
	FULL_SEQUENTIAL_JOB_INSERTION = 4,
	FULL_SEQUENTIAL_BATCH_INSERTION = 5,

	RANDOM_BATCH_INSERTION = 6,
	RANDOM_BATCH_SWAP = 7,
	RANDOM_JOB_INSERTION = 8,
	RANDOM_JOB_SWAP = 9,
	RANDOM_BATCH_SPLIT_FIDELIS = 10,
	RANDOM_BATCH_MERGE_FIDELIS = 11,
	RANDOM_BATCH_SPLIT_QUEIROGA = 12,

	FULL_BATCH_MERGE_QUEIROGA = 13,

	RANDOM_BATCH_SWAP_TARDINESS = 14,

	FULL_SEQUENTIAL_JOB_INSERTION_BILYK = 15,
	FULL_BATCH_SWAP_BILYK = 16,

	RANDOM_BATCH_SPLIT_BILYK_PARAMETER = 17,
	RANDOM_BATCH_INSERTION_BILYK = 18,
	RANDOM_BATCH_SEQUENCE_INSERTION_BILYK_PARAMETER = 19,
	RANDOM_BATCH_SWAP_BILYK = 20,
	RANDOM_BATCH_SEQUENCE_SWAP_INSERTION_BILYK = 21,
	RANDOM_BATCH_SEQUENCE_INSERTION_BILYK = 22,
	RANDOM_BATCH_SPLIT_BILYK = 23,

	FULL_JOB_FAST_INSERTION = 24,

	RANDOM_BATCH_SPLIT_MEDEIROS = 25,

	FULL_JOB_INSERTION_QUEIROGA = 26
};

inline const char *neighbourhoodStr[] = {
    stringify(FULL_BATCH_INSERTION),
    stringify(FULL_BATCH_SWAP),
    stringify(FULL_JOB_INSERTION),
    stringify(FULL_JOB_SWAP),
    stringify(FULL_SEQUENTIAL_JOB_INSERTION),
    stringify(FULL_SEQUENTIAL_BATCH_INSERTION),
    stringify(RANDOM_BATCH_INSERTION),
    stringify(RANDOM_BATCH_SWAP),
    stringify(RANDOM_JOB_INSERTION),
    stringify(RANDOM_JOB_SWAP),
    stringify(RANDOM_BATCH_SPLIT_FIDELIS),
    stringify(RANDOM_BATCH_MERGE_FIDELIS),
    stringify(RANDOM_BATCH_SPLIT_QUEIROGA),
    stringify(FULL_BATCH_MERGE_QUEIROGA),
    stringify(RANDOM_BATCH_SWAP_TARDINESS),
    stringify(FULL_SEQUENTIAL_JOB_INSERTION_BILYK),
    stringify(FULL_BATCH_SWAP_BILYK),
    stringify(RANDOM_BATCH_SPLIT_BILYK_PARAMETER),
    stringify(RANDOM_BATCH_INSERTION_BILYK),
    stringify(RANDOM_BATCH_SEQUENCE_INSERTION_BILYK_PARAMETER),
    stringify(RANDOM_BATCH_SWAP_BILYK),
    stringify(RANDOM_BATCH_SEQUENCE_SWAP_INSERTION_BILYK),
    stringify(RANDOM_BATCH_SEQUENCE_INSERTION_BILYK),
    stringify(RANDOM_BATCH_SPLIT_BILYK),
    stringify(FULL_JOB_FAST_INSERTION),
    stringify(RANDOM_BATCH_SPLIT_MEDEIROS),
	stringify(FULL_JOB_INSERTION_QUEIROGA)
};

inline string neighbourhoodToString(Neighbourhood n) {
	return string(neighbourhoodStr[n]);
}

inline void shuffleNeighbourhoods(vector<Neighbourhood> &originalvector) {
	auto rng = default_random_engine { };
	shuffle(begin(originalvector), end(originalvector), rng);
}

inline void executeLocalSearch(Schedule *schedule, Neighbourhood neighbourhood) {
	switch (neighbourhood) {
	case FULL_BATCH_INSERTION:
		executeFullBatchInsertion(schedule);
		break;
	case FULL_BATCH_SWAP:
		executeFullBatchSwap(schedule);
		break;
	case FULL_JOB_INSERTION:
		executeFullJobInsertionMedeiros(schedule);
		break;
	case FULL_JOB_SWAP:
		executeFullJobSwap(schedule);
		break;
	case FULL_SEQUENTIAL_JOB_INSERTION:
		executeFullSequentialJobInsertion(schedule, generateRandomNumberInRange(2, schedule->instance->instanceJobs.size() / 6));
		break;
	case FULL_BATCH_MERGE_QUEIROGA:
		executeFullBatchMergeQueiroga(schedule);
		break;
	case FULL_SEQUENTIAL_JOB_INSERTION_BILYK:
		executeFullJobInsertionBilyk(schedule);
		break;
	case FULL_BATCH_SWAP_BILYK:
		executeFullBatchSwapBilyk(schedule);
		break;
	case FULL_SEQUENTIAL_BATCH_INSERTION:
		executeFullSequentialBatchInsertion(schedule, generateRandomNumberInRange(1, 0.3 * schedule->totalBatches()));
		break;
	case FULL_JOB_FAST_INSERTION:
		executeFullJobFastInsertion(schedule);
		break;
	case FULL_JOB_INSERTION_QUEIROGA:
		executeFullJobInsertionQueiroga(schedule);
		break;
	default:
		cout << "Unknown neighbourhood. Code: " << neighbourhood << endl;
		exit(0);
	}
}

inline void executePerturbationMove(Schedule *schedule, Neighbourhood neighbourhood, int parameter) {
	switch (neighbourhood) {
	case RANDOM_BATCH_INSERTION:
		executeRandomBatchInsertion(schedule);
		break;
	case RANDOM_BATCH_SWAP:
		executeRandomBatchSwap(schedule);
		break;
	case RANDOM_JOB_INSERTION:
		executeRandomJobInsertion(schedule);
		break;
	case RANDOM_JOB_SWAP:
		executeRandomJobSwap(schedule);
		break;
	case RANDOM_BATCH_SPLIT_FIDELIS:
		executeRandomBatchSplitFidelis(schedule);
		break;
	case RANDOM_BATCH_MERGE_FIDELIS:
		executeRandomBatchMergeFidelis(schedule);
		break;
	case RANDOM_BATCH_SPLIT_QUEIROGA:
		executeRandomBatchSplitQueiroga(schedule);
		break;
	case RANDOM_BATCH_SWAP_TARDINESS:
		executeBatchSwapBasedOnHighestTardiness(schedule);
		break;
	case RANDOM_BATCH_SPLIT_BILYK_PARAMETER:
		executeRandomBatchSplitBilyk(schedule, parameter);
		break;
	case RANDOM_BATCH_INSERTION_BILYK:
		executeRandomBatchInsertion(schedule, parameter);
		break;
	case RANDOM_BATCH_SEQUENCE_INSERTION_BILYK_PARAMETER:
		executeRandomBatchSequenceInsertionBilyk(schedule, parameter);
		break;
	case RANDOM_BATCH_SWAP_BILYK:
		executeRandomBatchSwapBilyk(schedule, parameter);
		break;
	case RANDOM_BATCH_SEQUENCE_SWAP_INSERTION_BILYK:
		executeRandomBatchSequenceSwapInsertionBilyk(schedule, parameter);
		break;
	case RANDOM_BATCH_SEQUENCE_INSERTION_BILYK:
		executeRandomBatchSequenceInsertionBilyk(schedule);
		break;
	case RANDOM_BATCH_SPLIT_BILYK:
		executeRandomBatchSplitBilyk(schedule, parameter);
		break;
	case FULL_SEQUENTIAL_JOB_INSERTION:
		executeFullSequentialJobInsertion(schedule, generateRandomNumberInRange(2, schedule->instance->instanceJobs.size() / 6));
		break;
	case FULL_SEQUENTIAL_BATCH_INSERTION:
		executeFullSequentialBatchInsertion(schedule, generateRandomNumberInRange(1, 0.3 * schedule->totalBatches()));
		break;
	case RANDOM_BATCH_SPLIT_MEDEIROS:
		executeRandomBatchSplitMedeiros(schedule);
		break;
	default:
		exit(0);
	}
}

inline void executePerturbationMove(Schedule *schedule, Neighbourhood neighbourhood) {
	executePerturbationMove(schedule, neighbourhood, 0);
}

inline void executeRandomPerturbationMoves(int minPerturbations, int maxPerturbations, Schedule *schedule, vector<Neighbourhood> &perturbationMoves) {
	int numberOfMoves = generateRandomNumberInRange(minPerturbations, maxPerturbations);
	int neighbourhoodIndex = generateRandomIndex(perturbationMoves.size());
	for (int i = 0; i < numberOfMoves; i++) {
		executePerturbationMove(schedule, perturbationMoves[neighbourhoodIndex]);
	}
}
