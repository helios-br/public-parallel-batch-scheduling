#pragma once

#include <iostream>
#include <vector>
#include <assert.h>
#include <climits>
#include "BatchTestBuilder.h"
#include "../instance/InstanceJobTestBuilder.h"
#include "../instance/InstanceFamilyTestBuilder.h"
#include "../../main/schedule/Batch.h"
#include "../../main/schedule/JobAssignment.h"
#include "../../main/schedule/Job.h"
#include "../../main/util/NumberUtil.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceFamily.h"

using namespace std;

inline void testCreateBatch() {
	// setup
	int maxBatchSize = 4;

	// execute
	Batch *batch = BatchTestBuilder::createBatchWithDummyFamily(maxBatchSize);

	// verify
	assert(batch->assigned == false);
	assert(batch->currentSize == 0);
	assert(batch->releaseTime == 0);
	assert(batch->processingTime == 0);
	assert(batch->completionTime == 0);
	assert(batch->machineCompletionTime == 0);
	assert(batch->machineStartTime == 0);
	assert(batch->maxSize == maxBatchSize);
	assert(batch->jobs.size() == 4);
	assert(batch->jobs[0]->isDummy());
	assert(batch->jobs[1]->isDummy());
	assert(batch->jobs[2]->isDummy());
	assert(batch->jobs[3]->isDummy());
}

inline void testBatchWhenAddingAndRemovingJobs() {
	// setup
	Batch *batch = BatchTestBuilder::createBatchWithDummyFamily(2);
	InstanceJob instanceJob01 = createRandomInstanceJob(batch->instanceFamily);
	InstanceJob instanceJob02 = createRandomInstanceJob(batch->instanceFamily);
	Job job01 = Job(&instanceJob01);
	Job job02 = Job(&instanceJob02);

	// execute verify
	batch->addJob(&job01);
	assert(batch->currentSize == 1);
	assert(batch->jobs.size() == 2);
	assert(job01.assignedToBatch == true);
	assert(job01.batchIndexPosition == 0);
	assert(job01.batch == batch);

	batch->addJob(&job02);
	assert(batch->currentSize == 2);
	assert(batch->jobs.size() == 2);
	assert(job02.assignedToBatch == true);
	assert(job02.batchIndexPosition == 1);
	assert(job02.batch == batch);

	batch->removeJob(&job01);
	assert(batch->currentSize == 1);
	assert(batch->jobs.size() == 2);
	assert(job01.assignedToBatch == false);
	assert(job01.batchIndexPosition == -1);
	assert(job01.batch == NULL);

	batch->removeJob(&job02);
	assert(batch->currentSize == 0);
	assert(batch->jobs.size() == 2);
	assert(job02.assignedToBatch == false);
	assert(job02.batchIndexPosition == -1);
	assert(job02.batch == NULL);

	batch->addJob(&job01);
	assert(batch->currentSize == 1);
	assert(batch->jobs.size() == 2);
	assert(job01.assignedToBatch == true);
	assert(job01.batchIndexPosition == 0);
	assert(job01.batch == batch);
	assert(batch->jobs[0]->instanceJob->id == job01.instanceJob->id);
	assert(batch->jobs[1]->isDummy());
}

inline void testBatchReleaseAndProcessingTimes() {
	// setup
	Batch *batch = BatchTestBuilder::createBatchWithDummyFamily(4);

	//InstanceJob(int id, InstanceFamily *instanceFamily, unsigned size, unsigned dueDate, unsigned processingTime, int priorityWeight, unsigned releaseTime)
	InstanceJob instanceJob01 = InstanceJob(1, batch->instanceFamily, 1, 40000, 50000, 1, 40000);
	InstanceJob instanceJob02 = InstanceJob(2, batch->instanceFamily, 1, 40000, 60000, 1, 30000);
	InstanceJob instanceJob03 = InstanceJob(3, batch->instanceFamily, 1, 40000, 55000, 1, 45000);
	InstanceJob instanceJob04 = InstanceJob(3, batch->instanceFamily, 1, 40000, 70000, 1, 20000);
	Job job01 = Job(&instanceJob01);
	Job job02 = Job(&instanceJob02);
	Job job03 = Job(&instanceJob03);
	Job job04 = Job(&instanceJob04);

	// execute verify
	assert(batch->releaseTime == 0);
	assert(batch->processingTime == 0);
	assert(batch->completionTime == 0);

	batch->addJob(&job01);
	assert(batch->releaseTime == 40000);
	assert(batch->processingTime == 50000);
	assert(batch->completionTime == 40000 + 50000);

	batch->addJob(&job02);
	assert(batch->releaseTime == 40000);
	assert(batch->processingTime == 60000);
	assert(batch->completionTime == 40000 + 60000);

	batch->addJob(&job03);
	assert(batch->releaseTime == 45000);
	assert(batch->processingTime == 60000);
	assert(batch->completionTime == 45000 + 60000);

	batch->addJob(&job04);
	assert(batch->releaseTime == 45000);
	assert(batch->processingTime == 70000);
	assert(batch->completionTime == 45000 + 70000);

	batch->removeJob(&job04);
	assert(batch->releaseTime == 45000);
	assert(batch->processingTime == 60000);
	assert(batch->completionTime == 45000 + 60000);

	batch->removeJob(&job03);
	assert(batch->releaseTime == 40000);
	assert(batch->processingTime == 60000);
	assert(batch->completionTime == 40000 + 60000);

	batch->removeJob(&job02);
	assert(batch->releaseTime == 40000);
	assert(batch->processingTime == 50000);
	assert(batch->completionTime == 40000 + 50000);

	batch->removeJob(&job01);
	assert(batch->releaseTime == 0);
	assert(batch->processingTime == 0);
	assert(batch->completionTime == 0);
}

inline void testCanAddJob() {
	// setup
	Batch *batch = BatchTestBuilder::createBatchWithDummyFamily(2);

	//int id, InstanceFamily *instanceFamily,unsigned size,unsigned dueDate,unsigned processingTime,int priorityWeight,unsigned releaseTime
	InstanceJob instanceJob01 = InstanceJob(1, batch->instanceFamily, 1, 40000, 50000, 1, 40000);
	InstanceJob instanceJob02 = InstanceJob(2, batch->instanceFamily, 1, 40000, 60000, 1, 30000);
	InstanceJob instanceJob03 = InstanceJob(3, batch->instanceFamily, 1, 40000, 55000, 1, 35000);
	Job *job01 = new Job(&instanceJob01);
	Job *job02 = new Job(&instanceJob02);
	Job *job03 = new Job(&instanceJob03);

	InstanceFamily *otherInstanceFamily = new InstanceFamily(0, 0, 0);
	InstanceJob instanceJob04 = InstanceJob(4, otherInstanceFamily, 1, 40000, 50000, 1, 40000);
	Job *jobOtherFamily = new Job(&instanceJob04);

	// execite verify

	// Can't insert job of another family
	assert(canAddJobToBatch(batch, jobOtherFamily) == false);

	assert(canAddJobToBatch(batch, job01));
	batch->addJob(job01);  // rt 40000 ct 90000
	assert(canAddJobToBatch(batch, job01) == false);

	assert(canAddJobToBatch(batch, job02));
	batch->addJob(job02);
	assert(canAddJobToBatch(batch, job02) == false);

	// Batch is already full
	assert(canAddJobToBatch(batch, job03) == false);
}

inline void runBatchTest() {
	cout << "# Running BatchTest.h" << endl << endl;

	cout << ".... testCreateBatch [STARTED]" << endl;
	testCreateBatch();
	cout << ".... testBatchWhenAddingAndRemovingJobs [STARTED]" << endl;
	testBatchWhenAddingAndRemovingJobs();
	cout << ".... testBatchReleaseAndProcessingTimes [STARTED]" << endl;
	testBatchReleaseAndProcessingTimes();
	cout << ".... testCanAddJob [STARTED]" << endl;
	testCanAddJob();

	cout << endl << "[OK] BatchTest.h" << endl;
}
