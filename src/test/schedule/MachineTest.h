#pragma once

#include <iostream>
#include <vector>
#include <assert.h>
#include "MachineTestBuilder.h"
#include "../instance/InstanceJobTestBuilder.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceMachine.h"
#include "../../main/schedule/Job.h"
#include "../../main/schedule/Batch.h"
#include "../../main/schedule/Machine.h"
#include "../../main/util/NumberUtil.h"

using namespace std;

inline bool isBatchDummy(Batch *batch) {
	return batch->isDummy() && batch->machineIndexPosition == -1 && batch->machineStartTime == 0 && batch->machineCompletionTime == 0;
}

inline void assertBatchTime(Batch *batch, int machineStartTime, int machineCompletionTime) {
	cout << "[expected] startTime: " << machineStartTime << ", completionTime: " << machineCompletionTime << endl;
	cout << "[real] batch->machineStartTime: " << batch->machineStartTime << ", batch->completionTime: " << batch->completionTime << endl;
	assert(batch->machineStartTime == machineStartTime);
	assert(batch->machineCompletionTime == machineCompletionTime);
}

inline void testInsertAtFirstAvailablePosition() {
	// setup

	//InstanceFamily(int newId, unsigned processingTime, int numJobs) {
	InstanceFamily *instanceFamily = new InstanceFamily(1, 5, 6);

	InstanceJobTestBuilder *builder = InstanceJobTestBuilder::createInstanceJob(instanceFamily).withSize(1)->withDueDate(10);
	InstanceJob *instanceJob01 = builder->withId(1)->withReleaseTime(5)->withProcessingTime(3)->build();
	InstanceJob *instanceJob02 = builder->withId(2)->withReleaseTime(0)->withProcessingTime(2)->build();
	InstanceJob *instanceJob03 = builder->withId(3)->withReleaseTime(2)->withProcessingTime(2)->build();
	InstanceJob *instanceJob04 = builder->withId(4)->withReleaseTime(5)->withProcessingTime(1)->build();
	InstanceJob *instanceJob05 = builder->withId(5)->withReleaseTime(4)->withProcessingTime(2)->build();
	InstanceJob *instanceJob06 = builder->withId(6)->withReleaseTime(4)->withProcessingTime(1)->build();
	Job *job01 = new Job(instanceJob01);
	Job *job02 = new Job(instanceJob02);
	Job *job03 = new Job(instanceJob03);
	Job *job04 = new Job(instanceJob04);
	Job *job05 = new Job(instanceJob05);
	Job *job06 = new Job(instanceJob06);

	Batch *batch01 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch01->addJob(job01);
	Batch *batch02 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch02->addJob(job02);
	Batch *batch03 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch03->addJob(job03);
	Batch *batch04 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch04->addJob(job04);
	Batch *batch05 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch05->addJob(job05);
	Batch *batch06 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch06->addJob(job06);

	Machine *machine =
			MachineTestBuilder::createMachine().withInstanceMachineId(generateRandomNumber())->withInstanceMachineSize(5)->withMaxBatches(10)->build();

	// execute

	machine->insertAtFirstAvailablePosition(batch01);
	machine->insertAtFirstAvailablePosition(batch02);
	machine->insertAtFirstAvailablePosition(batch03);
	machine->insertAtFirstAvailablePosition(batch04);
	machine->insertAtFirstAvailablePosition(batch05);
	machine->insertAtFirstAvailablePosition(batch06);

	// verify

	assert(machine->numBatches == 6);
	assert(machine->batches[0] == batch02);
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob02->id);
	assert(machine->batches[0]->machineIndexPosition == 0);
	assert(machine->batches[0]->machineStartTime == 0);
	assert(machine->batches[0]->machineCompletionTime == 2);

	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[1] == batch03);
	assert(machine->batches[1]->machineIndexPosition == 1);
	assert(machine->batches[1]->machineStartTime == 2);
	assert(machine->batches[1]->machineCompletionTime == 4);

	assert(machine->batches[2]->jobs[0]->instanceJob->id == instanceJob06->id);
	assert(machine->batches[2] == batch06);
	assert(machine->batches[2]->machineIndexPosition == 2);
	assert(machine->batches[2]->machineStartTime == 4);
	assert(machine->batches[2]->machineCompletionTime == 5);

	assert(machine->batches[3]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[3] == batch01);
	assert(machine->batches[3]->machineIndexPosition == 3);
	assert(machine->batches[3]->machineStartTime == 5);
	assert(machine->batches[3]->machineCompletionTime == 8);

	assert(machine->batches[4]->jobs[0]->instanceJob->id == instanceJob04->id);
	assert(machine->batches[4] == batch04);
	assert(machine->batches[4]->machineIndexPosition == 4);
	assert(machine->batches[4]->machineStartTime == 8);
	assert(machine->batches[4]->machineCompletionTime == 9);

	assert(machine->batches[5]->jobs[0]->instanceJob->id == instanceJob05->id);
	assert(machine->batches[5] == batch05);
	assert(machine->batches[5]->machineIndexPosition == 5);
	assert(machine->batches[5]->machineStartTime == 9);
	assert(machine->batches[5]->machineCompletionTime == 11);
}

inline void testInsertAtFirstAvailablePositionAndRemove() {
	// setup

	//InstanceFamily(int newId, unsigned processingTime, int numJobs) {
	InstanceFamily *instanceFamily = new InstanceFamily(1, 5, 6);

	InstanceJobTestBuilder *builder = InstanceJobTestBuilder::createInstanceJob(instanceFamily).withSize(1)->withDueDate(10);
	InstanceJob *instanceJob01 = builder->withId(1)->withReleaseTime(10)->withProcessingTime(2)->build();
	InstanceJob *instanceJob02 = builder->withId(2)->withReleaseTime(10)->withProcessingTime(5)->build();
	InstanceJob *instanceJob03 = builder->withId(3)->withReleaseTime(5)->withProcessingTime(4)->build();
	InstanceJob *instanceJob04 = builder->withId(4)->withReleaseTime(9)->withProcessingTime(1)->build();
	InstanceJob *instanceJob05 = builder->withId(5)->withReleaseTime(9)->withProcessingTime(1)->build();
	InstanceJob *instanceJob06 = builder->withId(6)->withReleaseTime(2)->withProcessingTime(2)->build();
	Job *job01 = new Job(instanceJob01);
	Job *job02 = new Job(instanceJob02);
	Job *job03 = new Job(instanceJob03);
	Job *job04 = new Job(instanceJob04);
	Job *job05 = new Job(instanceJob05);
	Job *job06 = new Job(instanceJob06);

	Batch *batch01 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch01->addJob(job01);
	Batch *batch02 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch02->addJob(job02);
	Batch *batch03 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch03->addJob(job03);
	Batch *batch04 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch04->addJob(job04);
	Batch *batch05 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch05->addJob(job05);
	Batch *batch06 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch06->addJob(job06);

	Machine *machine =
			MachineTestBuilder::createMachine().withInstanceMachineId(generateRandomNumber())->withInstanceMachineSize(5)->withMaxBatches(10)->build();

	// execute verify

	cout << "t1" << endl;
	machine->insertAtFirstAvailablePosition(batch01);

	cout << "t2" << endl;
	assert(machine->numBatches == 1);
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[0]->machineIndexPosition == 0);
	assert(machine->batches[0]->machineStartTime == 10);
	assert(machine->batches[0]->machineCompletionTime == 12);

	cout << "t3" << endl;
	machine->removeBatch(batch01);
	cout << "t4" << endl;
	assert(machine->numBatches == 0);
	cout << "t5" << endl;
	assert(isBatchDummy(machine->batches[0]));
	cout << "t6" << endl;
	machine->insertAtFirstAvailablePosition(batch01);
	cout << "t7" << endl;
	machine->insertAtFirstAvailablePosition(batch02);
	machine->insertAtFirstAvailablePosition(batch03);
	machine->insertAtFirstAvailablePosition(batch04);
	machine->insertAtFirstAvailablePosition(batch05);
	machine->insertAtFirstAvailablePosition(batch06);
	cout << "t8" << endl;
	assert(machine->numBatches == 6);
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob06->id);
	assert(machine->batches[0]->machineIndexPosition == 0);
	assert(machine->batches[0]->machineStartTime == 2);
	assert(machine->batches[0]->machineCompletionTime == 4);

	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[1]->machineIndexPosition == 1);
	assert(machine->batches[1]->machineStartTime == 5);
	assert(machine->batches[1]->machineCompletionTime == 9);

	assert(machine->batches[2]->jobs[0]->instanceJob->id == instanceJob04->id);
	assert(machine->batches[2]->machineIndexPosition == 2);
	assert(machine->batches[2]->machineStartTime == 9);
	assert(machine->batches[2]->machineCompletionTime == 10);

	assert(machine->batches[3]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[3]->machineIndexPosition == 3);
	assert(machine->batches[3]->machineStartTime == 10);
	assert(machine->batches[3]->machineCompletionTime == 12);

	assert(machine->batches[4]->jobs[0]->instanceJob->id == instanceJob02->id);
	assert(machine->batches[4]->machineIndexPosition == 4);
	assert(machine->batches[4]->machineStartTime == 12);
	assert(machine->batches[4]->machineCompletionTime == 17);

	assert(machine->batches[5]->jobs[0]->instanceJob->id == instanceJob05->id);
	assert(machine->batches[5]->machineIndexPosition == 5);
	assert(machine->batches[5]->machineStartTime == 17);
	assert(machine->batches[5]->machineCompletionTime == 18);

	machine->removeBatch(machine->batches[5]);
	assert(machine->numBatches == 5);
	assert(isBatchDummy(machine->batches[5]));
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob06->id);
	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[2]->jobs[0]->instanceJob->id == instanceJob04->id);
	assert(machine->batches[3]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[4]->jobs[0]->instanceJob->id == instanceJob02->id);

	machine->removeBatch(machine->batches[0]);
	assert(machine->numBatches == 4);
	assert(isBatchDummy(machine->batches[4]));
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob04->id);
	assert(machine->batches[2]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[3]->jobs[0]->instanceJob->id == instanceJob02->id);

	machine->removeBatch(machine->batches[1]);
	assert(machine->numBatches == 3);
	assert(isBatchDummy(machine->batches[3]));
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob01->id);
	assert(machine->batches[2]->jobs[0]->instanceJob->id == instanceJob02->id);

	machine->removeBatch(machine->batches[1]);
	assert(machine->numBatches == 2);
	assert(isBatchDummy(machine->batches[2]));
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob03->id);
	assert(machine->batches[1]->jobs[0]->instanceJob->id == instanceJob02->id);

	machine->removeBatch(machine->batches[1]);
	assert(machine->numBatches == 1);
	assert(isBatchDummy(machine->batches[1]));
	assert(machine->batches[0]->jobs[0]->instanceJob->id == instanceJob03->id);

	machine->removeBatch(machine->batches[0]);
	assert(machine->numBatches == 0);
	assert(isBatchDummy(machine->batches[0]));
}

inline void testInsertAtFirstAvailablePositionRemoveAndImprove() {
	// setup

	//InstanceFamily(int newId, unsigned processingTime, int numJobs) {
	InstanceFamily *instanceFamily = new InstanceFamily(1, 5, 6);

	InstanceJobTestBuilder *builder = InstanceJobTestBuilder::createInstanceJob(instanceFamily).withSize(1)->withDueDate(10);
	InstanceJob *instanceJob01 = builder->withId(1)->withReleaseTime(0)->withProcessingTime(2)->build();
	InstanceJob *instanceJob02 = builder->withId(2)->withReleaseTime(0)->withProcessingTime(3)->build();
	InstanceJob *instanceJob03 = builder->withId(3)->withReleaseTime(2)->withProcessingTime(1)->build();
	InstanceJob *instanceJob04 = builder->withId(4)->withReleaseTime(2)->withProcessingTime(2)->build();
	InstanceJob *instanceJob05 = builder->withId(5)->withReleaseTime(3)->withProcessingTime(1)->build();
	InstanceJob *instanceJob06 = builder->withId(6)->withReleaseTime(2)->withProcessingTime(2)->build();
	Job *job01 = new Job(instanceJob01);
	Job *job02 = new Job(instanceJob02);
	Job *job03 = new Job(instanceJob03);
	Job *job04 = new Job(instanceJob04);
	Job *job05 = new Job(instanceJob05);
	Job *job06 = new Job(instanceJob06);

	Batch *batch01 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch01->addJob(job01);
	Batch *batch02 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch02->addJob(job02);
	Batch *batch03 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch03->addJob(job03);
	Batch *batch04 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch04->addJob(job04);
	Batch *batch05 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch05->addJob(job05);
	Batch *batch06 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch06->addJob(job06);

	Machine *machine =
			MachineTestBuilder::createMachine().withInstanceMachineId(generateRandomNumber())->withInstanceMachineSize(5)->withMaxBatches(10)->build();

	// execute verify

	machine->insertAtFirstAvailablePosition(batch01);
	machine->insertAtFirstAvailablePosition(batch02);
	machine->insertAtFirstAvailablePosition(batch03);
	machine->insertAtFirstAvailablePosition(batch04);
	machine->insertAtFirstAvailablePosition(batch05);
	machine->insertAtFirstAvailablePosition(batch06);

	assertBatchTime(machine->batches[0], 0, 2);  // job 1
	assertBatchTime(machine->batches[1], 2, 5);  // job 2
	assertBatchTime(machine->batches[2], 5, 6);  // job 3
	assertBatchTime(machine->batches[3], 6, 8);  // job 4
	assertBatchTime(machine->batches[4], 8, 9);  // job 5
	assertBatchTime(machine->batches[5], 9, 11); // job 6

	machine->removeBatch(machine->batches[0]);
	assertBatchTime(machine->batches[0], 0, 3); // job 2
	assertBatchTime(machine->batches[1], 3, 4); // job 3
	assertBatchTime(machine->batches[2], 4, 6); // job 4
	assertBatchTime(machine->batches[3], 6, 7); // job 5
	assertBatchTime(machine->batches[4], 7, 9); // job 6

	machine->removeBatch(machine->batches[1]);
	assertBatchTime(machine->batches[0], 0, 3); // job 2
	assertBatchTime(machine->batches[1], 3, 5); // job 4
	assertBatchTime(machine->batches[2], 5, 6); // job 5
	assertBatchTime(machine->batches[3], 6, 8); // job 6

	machine->removeBatch(machine->batches[2]);
	assertBatchTime(machine->batches[0], 0, 3); // job 2
	assertBatchTime(machine->batches[1], 3, 5); // job 4
	assertBatchTime(machine->batches[2], 5, 7); // job 6

	machine->removeBatch(machine->batches[0]);
	assertBatchTime(machine->batches[0], 2, 4); // job 4
	assertBatchTime(machine->batches[1], 4, 6); // job 6

	machine->removeBatch(machine->batches[0]);
	assertBatchTime(machine->batches[0], 2, 4); // job 6
}

inline void testInsertAtPositionAndDelay() {
	// setup

	//InstanceFamily(int newId, unsigned processingTime, int numJobs) {
	InstanceFamily *instanceFamily = new InstanceFamily(1, 5, 6);

	InstanceJobTestBuilder *builder = InstanceJobTestBuilder::createInstanceJob(instanceFamily).withSize(1)->withDueDate(10);
	InstanceJob *instanceJob01 = builder->withId(1)->withReleaseTime(2)->withProcessingTime(2)->build();
	InstanceJob *instanceJob02 = builder->withId(2)->withReleaseTime(0)->withProcessingTime(1)->build();
	InstanceJob *instanceJob03 = builder->withId(3)->withReleaseTime(1)->withProcessingTime(1)->build();
	InstanceJob *instanceJob04 = builder->withId(4)->withReleaseTime(0)->withProcessingTime(1)->build();
	InstanceJob *instanceJob05 = builder->withId(5)->withReleaseTime(2)->withProcessingTime(1)->build();
	InstanceJob *instanceJob06 = builder->withId(6)->withReleaseTime(10)->withProcessingTime(3)->build();
	Job *job01 = new Job(instanceJob01);
	Job *job02 = new Job(instanceJob02);
	Job *job03 = new Job(instanceJob03);
	Job *job04 = new Job(instanceJob04);
	Job *job05 = new Job(instanceJob05);
	Job *job06 = new Job(instanceJob06);

	Batch *batch01 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch01->addJob(job01);
	Batch *batch02 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch02->addJob(job02);
	Batch *batch03 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch03->addJob(job03);
	Batch *batch04 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch04->addJob(job04);
	Batch *batch05 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch05->addJob(job05);
	Batch *batch06 = BatchTestBuilder::createBatchWithDummyFamily(1);
	batch06->addJob(job06);

	Machine *machine =
			MachineTestBuilder::createMachine().withInstanceMachineId(generateRandomNumber())->withInstanceMachineSize(5)->withMaxBatches(10)->build();

	// execute verify

	machine->insertAtPositionAndDelay(batch01, 0);
	assertBatchTime(batch01, 2, 4);
	machine->insertAtPositionAndDelay(batch02, 0);
	assertBatchTime(batch02, 0, 1);
	assertBatchTime(batch01, 2, 4);
	machine->insertAtPositionAndDelay(batch03, 1);
	assertBatchTime(batch02, 0, 1);
	assertBatchTime(batch03, 1, 2);
	assertBatchTime(batch01, 2, 4);
	machine->insertAtPositionAndDelay(batch04, 0);
	assertBatchTime(batch04, 0, 1);
	assertBatchTime(batch02, 1, 2);
	assertBatchTime(batch03, 2, 3);
	assertBatchTime(batch01, 3, 5);
	machine->insertAtPositionAndDelay(batch05, 2);
	assertBatchTime(batch04, 0, 1);
	assertBatchTime(batch02, 1, 2);
	assertBatchTime(batch05, 2, 3);
	assertBatchTime(batch03, 3, 4);
	assertBatchTime(batch01, 4, 6);
	machine->insertAtPositionAndDelay(batch06, 5);
	assertBatchTime(batch04, 0, 1);
	assertBatchTime(batch02, 1, 2);
	assertBatchTime(batch05, 2, 3);
	assertBatchTime(batch03, 3, 4);
	assertBatchTime(batch01, 4, 6);
	assertBatchTime(batch06, 10, 13);
}

inline void runMachineTest() {
	cout << "# Running MachineTest.h" << endl;

	cout << endl << ".... testInsertAtFirstAvailablePosition [STARTED]" << endl;
	testInsertAtFirstAvailablePosition();
	cout << endl << ".... testInsertAtFirstAvailablePositionAndRemove [STARTED]" << endl;
	testInsertAtFirstAvailablePositionAndRemove();
	cout << endl << ".... testInsertAtFirstAvailablePositionRemoveAndImprove [STARTED]" << endl;
	testInsertAtFirstAvailablePositionRemoveAndImprove();
	cout << endl << ".... testInsertAtPositionAndDelay [STARTED]" << endl;
	testInsertAtPositionAndDelay();

	cout << endl << "[OK] MachineTest.h" << endl;
}
