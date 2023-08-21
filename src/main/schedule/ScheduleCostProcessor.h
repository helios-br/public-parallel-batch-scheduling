#pragma once

#include <iostream>
#include <string>
#include <cfloat>
#include <limits>
#include "Batch.h"
#include "JobUtil.h"
#include "Machine.h"
#include "Schedule.h"

using namespace std;

inline int calculateJobCostStateless(Job *job) {
	if (job->assignedToMachine) {
		int tardiness = job->batch->machineCompletionTime > job->instanceJob->dueDate ? job->batch->machineCompletionTime - job->instanceJob->dueDate : 0;
		return job->instanceJob->priorityWeight * tardiness;
	}
	return 0;
}

inline int calculateBatchCostStateless(Batch *batch) {
	int cost = 0;
	for (int j = 0; j < batch->jobs.size(); j++) {
		cost += calculateJobCostStateless(batch->jobs[j]);
	}
	return cost;
}

inline int calculateMachineCostStateless(Machine *machine) {
	int cost = 0;
	for (int b = 0; b < machine->numBatches; b++) {
		cost += calculateBatchCostStateless(machine->batches[b]);
	}
	return cost;
}

inline int calculateMachineCostStateful(Machine *machine) {
	machine->cost = calculateMachineCostStateless(machine);
	return machine->cost;
}

inline void calculateMachineCostStatefulAndUpdateScheduleCost(Machine *machineToUpdate, Schedule *schedule) {
	schedule->cost = 0;
	for (Machine *machine : schedule->machines) {
		if (machine == machineToUpdate) {
			machine->cost = calculateMachineCostStateless(machine);
		}
		schedule->cost += machine->cost;
	}
}

inline int calculateScheduleCostStateful(Schedule *schedule) {
	schedule->cost = 0;
	for (Machine *machine : schedule->machines) {
		schedule->cost += calculateMachineCostStateful(machine);
	}
	return schedule->cost;
}

