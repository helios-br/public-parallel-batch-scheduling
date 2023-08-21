#pragma once

#include <iostream>
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceFamily.h"
#include "../../main/util/NumberUtil.h"

using namespace std;

inline InstanceJob createRandomInstanceJob(InstanceFamily *instanceFamily) {
	InstanceJob instance = InstanceJob(generateRandomNumber(), instanceFamily, 1, 40000, 40000, 21, 0);
	cout << "New random instance created: " << instance.toString() << endl;
	return instance;
}

struct InstanceJobTestBuilder {

private:

	int id;
	InstanceFamily *instanceFamily;
	int size;
	int dueDate;
	int processingTime;
	int priorityWeight;
	int releaseTime;

	InstanceJobTestBuilder(InstanceFamily *instanceFamily) {
		this->id = generateRandomNumberInRange(0, 1000);
		this->size = 1;
		this->priorityWeight = 1;
		this->releaseTime = 0;
		this->dueDate = 1;
		this->instanceFamily = instanceFamily;
		this->processingTime = instanceFamily->processingTime;
	}

public:

	inline static InstanceJobTestBuilder createInstanceJob(InstanceFamily *instanceFamily) {
		return InstanceJobTestBuilder(instanceFamily);
	}

	InstanceJobTestBuilder* withId(int id) {
		this->id = id;
		return this;
	}

	InstanceJobTestBuilder* withInstanceFamily(InstanceFamily *instanceFamily) {
		this->instanceFamily = instanceFamily;
		return this;
	}

	InstanceJobTestBuilder* withSize(int size) {
		this->size = size;
		return this;
	}

	InstanceJobTestBuilder* withDueDate(int dueDate) {
		this->dueDate = dueDate;
		return this;
	}

	InstanceJobTestBuilder* withProcessingTime(int processingTime) {
		this->processingTime = processingTime;
		return this;
	}

	InstanceJobTestBuilder* withPriorityWeight(int priorityWeight) {
		this->priorityWeight = priorityWeight;
		return this;
	}

	InstanceJobTestBuilder* withReleaseTime(int releaseTime) {
		this->releaseTime = releaseTime;
		return this;
	}

	InstanceJob* build() {
		return new InstanceJob(this->id, this->instanceFamily, this->size, this->dueDate, this->processingTime, this->priorityWeight, this->releaseTime);
	}

};
