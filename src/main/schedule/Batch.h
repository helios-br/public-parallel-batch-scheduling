#pragma once

#include <vector>
#include <string>

#include "Job.h"
#include "Machine.h"
#include "../instance/InstanceFamily.h"

using namespace std;

struct Machine;
struct Job;

struct Batch {

	InstanceFamily *instanceFamily = NULL;

	int maxSize;
	int currentSize = 0;
	int releaseTime = 0;
	int processingTime = 0;
	int completionTime = 0;

	// Jobs of this batch
	vector<Job*> jobs;

	// Machine to which this batch is assigned
	bool assigned = false;
	Machine *machine = NULL;
	int machineStartTime = 0;
	int machineCompletionTime = 0;

	// Index of this batch on the machine
	int machineIndexPosition = -1;

public:

	Batch(InstanceFamily *instanceFamily, int maxBatchSize);
	~Batch();
	bool isFull();
	void addJob(Job *job);
	void addJobs(vector<Job *> &jobs);
	void removeJob(Job *job);
	void removeAllJobs();
	void removeAllJobs(vector<Job *> &output);
	bool isDummy();
	Job* getJob(int index);
	int tardiness();
	string toString();
};

inline static Batch DUMMY_BATCH = Batch(NULL, 0);
