#pragma once

#include <vector>
#include <iostream>
#include <string>

using namespace std;

struct InstanceFamily {

	inline static int DUMMY_FAMILY_ID = -1;

	int id;
	int processingTime = 0;
	int numJobs = 0;

public:

	InstanceFamily(int newId, int processingTime, int numJobs) {
		this->id = newId;
		this->processingTime = processingTime;
		this->numJobs = numJobs;
	}

	inline bool isDummy() {
		return this->id == DUMMY_FAMILY_ID;
	}

	string toString() {
		return "{id: " + to_string(this->id) + ", pt: " + to_string(this->processingTime) + "}";
	}
};

inline static InstanceFamily DUMMY_INSTANCE_FAMILY = InstanceFamily(InstanceFamily::DUMMY_FAMILY_ID, 0, 0);

