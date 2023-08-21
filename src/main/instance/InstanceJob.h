#pragma once

#include <iostream>
#include <string>
#include "InstanceFamily.h"

using namespace std;

struct InstanceFamily;

struct InstanceJob {

	inline static int DUMMY_ID = -1;

	int id;
	InstanceFamily *instanceFamily = NULL;
	int size = 1;
	int priorityWeight = 0;
	int dueDate = 0;
	int processingTime = 0;
	int releaseTime = 0;

public:

	InstanceJob(int id, InstanceFamily *instanceFamily, int size, int dueDate, int processingTime, int priorityWeight, int releaseTime);

	string toString();
	void print();
};


