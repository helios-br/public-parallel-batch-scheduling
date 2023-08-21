#pragma once

#include <iostream>
#include <vector>
#include <assert.h>
#include "../instance/InstanceJobTestBuilder.h"
#include "../instance/InstanceFamilyTestBuilder.h"
#include "../../main/schedule/Batch.h"
#include "../../main/schedule/Job.h"
#include "../../main/schedule/JobUtil.h"
#include "../../main/util/NumberUtil.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceFamily.h"

using namespace std;

inline void testProcessingTimeOrderingNonAsc() {
	// setup
	InstanceFamily instanceFamily = createInstanceFamily();
	InstanceJob instanceJob01 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob02 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob03 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob04 = createRandomInstanceJob(&instanceFamily);

	Job *job01 = new Job(&instanceJob01);
	job01->instanceJob->processingTime = 10;
	Job *job02 = new Job(&instanceJob02);
	job02->instanceJob->processingTime = 12;
	Job *job03 = new Job(&instanceJob03);
	job03->instanceJob->processingTime = 8;
	Job *job04 = new Job(&instanceJob04);
	job04->instanceJob->processingTime = 14;

	vector<Job*> jobs { job01, job02, job03, job04, &DUMMY_JOB, &DUMMY_JOB };

	// execute
	sortJobsByProcessingTimeNonAsc(jobs);
	printJobs(jobs);

	// verify
	assert(jobs[0]->instanceJob->id == job04->instanceJob->id);
	assert(jobs[1]->instanceJob->id == job02->instanceJob->id);
	assert(jobs[2]->instanceJob->id == job01->instanceJob->id);
	assert(jobs[3]->instanceJob->id == job03->instanceJob->id);
	assert(jobs[4]->isDummy());
	assert(jobs[5]->isDummy());
}

inline void runJobUtilTest() {
	cout << "# Running JobUtilTest.h" << endl << endl;

	cout << ".... testProcessingTimeOrderingNonAsc [STARTED]" << endl;
	testProcessingTimeOrderingNonAsc();

	cout << endl << "[OK] JobUtilTest.h" << endl;
}
