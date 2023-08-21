#pragma once

#include <iostream>
#include <vector>
#include <assert.h>
#include "../instance/InstanceJobTestBuilder.h"
#include "../../main/schedule/Batch.h"
#include "../../main/schedule/Job.h"
#include "../../main/schedule/JobUtil.h"
#include "../../main/solver/TWDUtil.h"
#include "../../main/util/NumberUtil.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceFamily.h"
#include "../../main/util/Matrix.h"

using namespace std;

inline void testSortJobsByAssignmentAndPriority() {
	// setup
	InstanceFamily instanceFamily = DUMMY_INSTANCE_FAMILY;
	InstanceFamily otherFamily = InstanceFamily(1, 1, 2);

	InstanceJob instanceJob01 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob02 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob03 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob04 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob05 = createRandomInstanceJob(&instanceFamily);
	InstanceJob instanceJob06 = createRandomInstanceJob(&instanceFamily);

	InstanceJob instanceJob07 = createRandomInstanceJob(&otherFamily);
	InstanceJob instanceJob08 = createRandomInstanceJob(&otherFamily);
	InstanceJob instanceJob09 = createRandomInstanceJob(&otherFamily);

	// jobs not assigned
	Job *job01 = new Job(&instanceJob01);
	job01->priority = 2;
	job01->assignedToMachine = false;
	Job *job02 = new Job(&instanceJob02);
	job02->priority = 1;
	job02->assignedToMachine = false;
	Job *job03 = new Job(&instanceJob03);
	job03->priority = 3;
	job03->assignedToMachine = false;

	// jobs assigned
	Job *job04 = new Job(&instanceJob04);
	job04->priority = 1;
	job04->assignedToMachine = true;
	Job *job05 = new Job(&instanceJob05);
	job05->priority = 2;
	job05->assignedToMachine = true;
	Job *job06 = new Job(&instanceJob06);
	job06->priority = 3;
	job06->assignedToMachine = true;

	// other family
	Job *job07 = new Job(&instanceJob07);
	job07->priority = 1;
	job07->assignedToMachine = true;
	Job *job08 = new Job(&instanceJob08);
	job08->priority = 2;
	job08->assignedToMachine = false;
	Job *job09 = new Job(&instanceJob09);
	job09->priority = 3;
	job09->assignedToMachine = false;

	vector<Job*> jobs;
	jobs.push_back(job01);
	jobs.push_back(job02);
	jobs.push_back(job03);
	jobs.push_back(job04);
	jobs.push_back(job05);
	jobs.push_back(job06);
	jobs.push_back(job07);
	jobs.push_back(job08);
	jobs.push_back(job09);

	assert(jobs[0]->instanceJob->id == job01->instanceJob->id);
	assert(jobs[1]->instanceJob->id == job02->instanceJob->id);
	assert(jobs[2]->instanceJob->id == job03->instanceJob->id);
	assert(jobs[3]->instanceJob->id == job04->instanceJob->id);
	assert(jobs[4]->instanceJob->id == job05->instanceJob->id);
	assert(jobs[5]->instanceJob->id == job06->instanceJob->id);
	assert(jobs[6]->instanceJob->id == job07->instanceJob->id);
	assert(jobs[7]->instanceJob->id == job08->instanceJob->id);
	assert(jobs[8]->instanceJob->id == job09->instanceJob->id);

	// execute
	cout << "sorting..." << endl;
	sortJobsByAssignmentAndPriority(jobs);
	cout << "sorted!" << endl;

	// verify
	assert(jobs[0]->instanceJob->id == job03->instanceJob->id);
	assert(jobs[1]->instanceJob->id == job01->instanceJob->id);
	assert(jobs[2]->instanceJob->id == job02->instanceJob->id);
	assert(jobs[3]->instanceJob->id == job06->instanceJob->id);
	assert(jobs[4]->instanceJob->id == job05->instanceJob->id);
	assert(jobs[5]->instanceJob->id == job04->instanceJob->id);
	// other family
	assert(jobs[6]->instanceJob->id == job09->instanceJob->id);
	assert(jobs[7]->instanceJob->id == job08->instanceJob->id);
	assert(jobs[8]->instanceJob->id == job07->instanceJob->id);
}

inline void runJobTest() {
	cout << "# Running JobTest.h" << endl << endl;

	cout << ".... testSortJobsByAssignmentAndPriority [STARTED]" << endl;
	testSortJobsByAssignmentAndPriority();

	cout << endl << "[OK] JobTest.h" << endl;
}
