#include "InstanceJob.h"

InstanceJob::InstanceJob(
		int id,
		InstanceFamily *instanceFamily,
		int size,
		int dueDate,
		int processingTime,
		int priorityWeight,
		int releaseTime) {
	this->id = id;
	this->instanceFamily = instanceFamily;
	this->size = size;
	this->dueDate = dueDate;
	this->processingTime = processingTime;
	this->priorityWeight = priorityWeight;
	this->releaseTime = releaseTime;
}

string InstanceJob::toString() {
	return "{id: " + to_string(this->id) + ", f: " + this->instanceFamily->toString() + ", sz: " + to_string(this->size) + ", rt: " + to_string(this->releaseTime)
			+ ", dt: " + to_string(this->dueDate) + ", pt: " + to_string(this->processingTime) + ", wt: " + to_string(this->priorityWeight) + "}";
}

void InstanceJob::print() {
	cout << toString() << endl;
}
