#pragma once

#include <iostream>
#include <map>
#include "Schedule.h"
#include "Machine.h"
#include "Batch.h"
#include "Job.h"
#include "../instance/InstanceFamily.h"
#include "../util/NumberUtil.h"
#include "ScheduleValidator.h"

using namespace std;

inline Batch* getLastBatchOfMachine(Machine *machine) {
	return machine->numBatches > 0 ? machine->batches[machine->numBatches - 1] : NULL;
}

inline Batch* getSecondToLastBatchOfMachine(Machine *machine) {
	return machine->batches[machine->numBatches - 2];
}

inline Job* getRandomJobFromFamily(Schedule *schedule, InstanceFamily *instanceFamily) {
	int index = (instanceFamily->id * instanceFamily->numJobs) + generateRandomIndex(instanceFamily->numJobs);
	return schedule->jobs[index];
}

inline Job* getRandomJob(Schedule *schedule) {
	return schedule->jobs[generateRandomIndex(schedule->jobs.size())];
}

inline Job* getRandomAssignedJob(vector<Job*> &jobs) {
	if (jobs.size() == 0) {
		return NULL;
	} else if (jobs.size() == 1) {
		return jobs[0];
	} else {
		return jobs[generateRandomIndex(jobs.size())];
	}
}

inline void fillWithAssignedJobs(Schedule *schedule, vector<Job*> &jobs) {
	for (Job *job : schedule->jobs) {
		if (job->assignedToMachine) {
			jobs.push_back(job);
		}
	}
}

inline Batch* getRandomBatchFromFamily(Schedule *schedule, InstanceFamily *instanceFamily) {
	return getRandomJobFromFamily(schedule, instanceFamily)->batch;
}

inline Batch* getRandomBatchFromMachine(Machine *machine) {
	return machine->batches[generateRandomIndex(machine->numBatches)];
}

inline Batch* getRandomBatch(Schedule *schedule) {
	return getRandomJob(schedule)->batch;
}

inline Machine* getRandomMachine(Schedule *schedule) {
	return schedule->machines[generateRandomIndex(schedule->machines.size())];
}

inline Machine* getRandomMachineException(Schedule *schedule, Machine *exception) {
	int index = generateRandomIndex(schedule->machines.size());
	Machine *machine = schedule->machines[index];
	if (machine == exception) {
		return schedule->machines[index == (schedule->machines.size() - 1) ? 0 : index + 1];
	}
	return machine;
}

inline Batch* getBatchWithSmallestNumberOfJobs(vector<Machine*> &machines) {
	Batch *smallestBatch = NULL;
	for (int m = 0; m < machines.size(); m++) {
		Machine *machine = machines[m];
		for (int b = 0; b < machine->numBatches; b++) {
			Batch *batch = machine->batches[b];
			if (batch->currentSize == 1) {
				return batch;
			} else if (smallestBatch == NULL || batch->currentSize < smallestBatch->currentSize) {
				smallestBatch = batch;
			}
		}
	}
	return smallestBatch;
}

inline Batch* getBatchWithSmallestNumberOfJobs(Schedule *schedule) {
	return getBatchWithSmallestNumberOfJobs(schedule->machines);
}

inline Schedule* cloneSchedule(Schedule *originalSchedule) {
	Schedule *newSchedule = new Schedule(originalSchedule->instance);
	newSchedule->cost = originalSchedule->cost;

	newSchedule->thres = originalSchedule->thres;
	newSchedule->lookAheadParam = originalSchedule->lookAheadParam;

	for (int m = 0; m < originalSchedule->machines.size(); m++) {
		Machine *oldMachine = originalSchedule->machines[m];
		Machine *newMachine = newSchedule->machines[m];
		newMachine->cost = oldMachine->cost;

		for (int b = 0; b < oldMachine->numBatches; b++) {
			Batch *originalBatch = oldMachine->batches[b];
			Batch *newBatch = new Batch(originalBatch->instanceFamily, originalBatch->maxSize);

			for (int j = 0; j < originalBatch->jobs.size(); j++) {
				Job *originalJob = originalBatch->jobs[j];
				if (!originalJob->isDummy()) {
					newBatch->addJob(newSchedule->jobs[originalJob->instanceJob->id]);
				}
			}
			newMachine->insertAtPositionAndDelay(newBatch, b);
		}
	}

	return newSchedule;
}

inline void balanceSchedule(Schedule *schedule) {

	// If the last batch of the machine with the maximum completion time starts later than the
	// completion time of another machine, this batch is moved to the machine with the smallest
	// completion time of the last batch. Ties are broken randomly. This step is repeated until
	// no batch can be moved.

	Batch *batchWithMaxCompletionTime;
	Batch *batchWithMinCompletionTime;

	while (true) {

		batchWithMaxCompletionTime = NULL;
		batchWithMinCompletionTime = NULL;

		for (int m = 0; m < schedule->machines.size(); m++) {
			Batch *localBatch = getLastBatchOfMachine(schedule->machines[m]);

			if (localBatch == NULL) {
				continue;
			}

			if (batchWithMaxCompletionTime == NULL || localBatch->machineCompletionTime > batchWithMaxCompletionTime->machineCompletionTime) {
				batchWithMaxCompletionTime = localBatch;
			}
			if (batchWithMinCompletionTime == NULL || localBatch->machineCompletionTime < batchWithMinCompletionTime->machineCompletionTime) {
				batchWithMinCompletionTime = localBatch;
			}
		}

		if (batchWithMaxCompletionTime->machineStartTime > batchWithMinCompletionTime->machineCompletionTime) {
			Machine *machineBatchWithMaxCompletionTime = batchWithMaxCompletionTime->machine;
			Machine *machineBatchWithMinCompletionTime = batchWithMinCompletionTime->machine;

			machineBatchWithMaxCompletionTime->removeBatch(batchWithMaxCompletionTime);
			machineBatchWithMinCompletionTime->insertAfterLastBatch(batchWithMaxCompletionTime);

			calculateMachineCostStatefulAndUpdateScheduleCost(machineBatchWithMaxCompletionTime, schedule);
			calculateMachineCostStatefulAndUpdateScheduleCost(machineBatchWithMinCompletionTime, schedule);
		} else {
			break;
		}
	}
}

inline void printBatchesSequence(Schedule *schedule) {
	cout << "# schedule id: " << schedule->id;
	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *machine = schedule->machines[m];
		cout << endl << "@ machine" << m << ": ";
		for (int b = 0; b < machine->numBatches; b++) {
			cout << static_cast<void*>(machine->batches[b]) << ", ";
		}
	}
	cout << endl;
}

inline void printScheduleCostOnly(Schedule *schedule) {
	string cost = to_string(schedule->cost/(double) schedule->instance->multiplier);
	replace(cost.begin(), cost.end(), '.', ',');
	cout << cost << endl;
}

inline void addScheduleInOrderByCostAsc(vector<Schedule*> &allSchedules, Schedule *newSchedule) {
	int index = 0;
	for (; index < allSchedules.size(); index++) {
		if (newSchedule->cost == allSchedules[index]->cost) {
			delete newSchedule;
			return;
		}
		if (newSchedule->cost < allSchedules[index]->cost) {
			break;
		}
	}
	auto position = allSchedules.begin() + index;
	allSchedules.insert(position, newSchedule);
}

inline bool isTardyJob(Job *job) {
	return !job->isDummy() && job->batch->machineCompletionTime > job->instanceJob->dueDate;
}

inline int totalTardyJobs(Schedule *schedule) {
	int total = 0;
	for (int j = 0; j < schedule->jobs.size(); j++) {
		Job *job = schedule->jobs[j];
		if (isTardyJob(job)) {
			total++;
		}
	}
	return total;
}
