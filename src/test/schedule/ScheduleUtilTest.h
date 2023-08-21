#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include "ScheduleTestHelper.h"
#include "MachineTestBuilder.h"
#include "../../main/instance/Instance.h"
#include "../../main/instance/ChiangInstanceLoader.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceMachine.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"
#include "../../main/solver/Solver.h"
#include "../../main/solver/TimeWindowDecomposition.h"
#include "../../main/util/StringUtil.h"

using namespace std;

inline void testGetBatchWithSmallestNumberOfJobs() {
	// setup
	Batch *batch01 = BatchTestBuilder::createBatchWithDummyFamily(5);
	Batch *batch02 = BatchTestBuilder::createBatchWithDummyFamily(5);
	Batch *batch03 = BatchTestBuilder::createBatchWithDummyFamily(5);
	InstanceJob instanceJob01 = createRandomInstanceJob(batch01->instanceFamily);
	InstanceJob instanceJob02 = createRandomInstanceJob(batch01->instanceFamily);
	InstanceJob instanceJob03 = createRandomInstanceJob(batch01->instanceFamily);
	InstanceJob instanceJob04 = createRandomInstanceJob(batch01->instanceFamily);
	InstanceJob instanceJob05 = createRandomInstanceJob(batch01->instanceFamily);
	InstanceJob instanceJob06 = createRandomInstanceJob(batch01->instanceFamily);
	Job job01 = Job(&instanceJob01);
	Job job02 = Job(&instanceJob02);
	Job job03 = Job(&instanceJob03);
	Job job04 = Job(&instanceJob04);
	Job job05 = Job(&instanceJob05);
	Job job06 = Job(&instanceJob06);

	batch01->addJob(&job01);
	batch01->addJob(&job02);
	batch01->addJob(&job03);
	batch02->addJob(&job04);
	batch02->addJob(&job05);
	batch03->addJob(&job06);

	Machine *machine01 = MachineTestBuilder::createMachine().build();
	Machine *machine02 = MachineTestBuilder::createMachine().build();
	machine01->insertAtFirstAvailablePosition(batch01);
	machine02->insertAtFirstAvailablePosition(batch02);
	machine02->insertAtFirstAvailablePosition(batch03);
	vector<Machine*> machines { machine01, machine02 };

	// execute verify
	Batch *smallestBatch = getBatchWithSmallestNumberOfJobs(machines);
	assert(smallestBatch == batch03);

	// setup

	machine02->removeBatch(batch02);
	machine02->removeBatch(batch03);
	machine01->insertAtPositionAndDelay(batch02, 0);
	machine01->insertAtPositionAndDelay(batch03, 0);

	// execute verify
	smallestBatch = getBatchWithSmallestNumberOfJobs(machines);
	assert(smallestBatch == batch03);

}

inline void testCloneSchedule() {
	Schedule *originalSchedule = createScheduleWithTwdForTest();
	validateSchedule(originalSchedule);
	Schedule *newSchedule = cloneSchedule(originalSchedule);
	validateSchedule(newSchedule);
	int originalCost = originalSchedule->cost;
	int newCost = newSchedule->cost;
	assert(to_string(originalCost) == to_string(newCost));

	// All instances must be different
	for (int m = 0; m < originalSchedule->machines.size(); m++) {
		Machine *newMachine = newSchedule->machines[m];
		Machine *originalMachine = originalSchedule->machines[m];

		assert(originalMachine != newMachine);
		assert(originalMachine->instanceMachine->id == newMachine->instanceMachine->id);
		assert(originalMachine->batches.size() == newMachine->batches.size());
		assert(originalMachine->numBatches == newMachine->numBatches);

		for (int b = 0; b < originalMachine->batches.size(); b++) {
			Batch *originalBatch = originalMachine->batches[b];
			Batch *newBatch = newMachine->batches[b];

			if (b >= (int) originalMachine->numBatches) {
				assert(originalBatch->isDummy());
				assert(newBatch->isDummy());
				continue;
			}

			assert(originalBatch->instanceFamily == newBatch->instanceFamily);
			assert(originalBatch->instanceFamily->id == newBatch->instanceFamily->id);
			assert(originalBatch->currentSize == newBatch->currentSize);
			assert(originalBatch->currentSize > 0);
			assert(newBatch->currentSize > 0);
			assert(originalBatch->machine != newBatch->machine);
			assert(originalBatch->machine->instanceMachine == newBatch->machine->instanceMachine);
			assert(originalBatch->machine->instanceMachine->id == newBatch->machine->instanceMachine->id);
			assert(originalBatch->machineIndexPosition == newBatch->machineIndexPosition);
			assert(originalBatch->machineIndexPosition != -1);
			assert(newBatch->machineIndexPosition != -1);
			assert(originalBatch->machineStartTime == newBatch->machineStartTime);
			assert(originalBatch->machineCompletionTime == newBatch->machineCompletionTime);
			assert(originalBatch->maxSize == newBatch->maxSize);
			assert(originalBatch->releaseTime == newBatch->releaseTime);
			assert(originalBatch->processingTime == newBatch->processingTime);
			assert(originalBatch->completionTime == newBatch->completionTime);
			assert(originalBatch->jobs.size() == newBatch->jobs.size());
			assert(originalBatch->assigned == newBatch->assigned);
			assert(originalBatch->assigned);
			assert(newBatch->assigned);

			int currentSizeCounter = 0;
			for (int j = 0; j < originalBatch->jobs.size(); j++) {
				Job *originalJob = originalBatch->jobs[j];
				Job *newJob = newBatch->jobs[j];
				if (originalJob->isDummy() || newJob->isDummy()) {
					assert(originalJob->isDummy());
					assert(newJob->isDummy());
					continue;
				}
				assert(originalJob != newJob);
				assert(originalJob->instanceJob == newJob->instanceJob);
				assert(originalJob->instanceJob->id == newJob->instanceJob->id);
				assert(originalJob->instanceJob->id == newJob->instanceJob->id);
				assert(originalJob->assignedToBatch == newJob->assignedToBatch);
				assert(originalJob->assignedToBatch);
				assert(newJob->assignedToBatch);
				assert(originalJob->assignedToMachine);
				assert(newJob->assignedToMachine);
				assert(originalJob->batchIndexPosition == newJob->batchIndexPosition);
				currentSizeCounter++;
			}
			assert(currentSizeCounter == originalBatch->currentSize);
		}
	}
}

inline void runScheduleUtilTest() {
	cout << "# Running ScheduleUtilTest.h" << endl;

	cout << endl << ".... testGetBatchWithSmallestNumberOfJobs [STARTED]" << endl;
	testGetBatchWithSmallestNumberOfJobs();
	cout << endl << ".... testCloneSchedule [STARTED]" << endl;
	testCloneSchedule();

	cout << endl << "[OK] ScheduleUtilTest.h" << endl;
}
