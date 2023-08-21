#pragma once

#include <iostream>
#include <vector>
#include "../instance/InstanceMachine.h"
#include "Batch.h"

using namespace std;

struct Batch;
struct Job;
struct Schedule;

struct Machine {

	Schedule *schedule;
	InstanceMachine *instanceMachine;
	vector<Batch*> batches;
	int numBatches = 0;
	int cost = 0;

public:

	Machine(InstanceMachine *instanceMachine, int maxBatches, Schedule *schedule);
	~Machine();

	void createBatchWithJobAndInsert(Job *job, int machineIndexPosition);
	void insertAfterLastBatch(Batch *batch);
	void insertAtFirstAvailablePosition(Batch *batch);
	void insertAtPositionAndDelay(Batch *batch, int index);
	void removeBatch(Batch *batch);

private:

	void insertAtPosition(Batch *batch, int index);
	void removeBatch(Batch *batch, bool improveOthers);
};
