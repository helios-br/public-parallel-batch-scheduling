#pragma once

#include <climits>
#include "../../main/instance/InstanceMachine.h"
#include "../../main/schedule/Machine.h"
#include "../../main/util/NumberUtil.h"
#include "../instance/InstanceTestHelper.h"

using namespace std;

struct MachineTestBuilder {

private:

	int instanceMachineId;
	int instanceMachineSize;
	int machineMaxBatches;
	Schedule *schedule = NULL;
	InstanceMachine *instance = NULL;

	MachineTestBuilder() {
		this->instanceMachineId = generateRandomNumber();
		this->instanceMachineSize = 5;
		this->machineMaxBatches = 100;
	}

public:

	inline static MachineTestBuilder createMachine() {
		return MachineTestBuilder();
	}

	MachineTestBuilder* withInstanceMachineId(int id) {
		this->instanceMachineId = id;
		return this;
	}

	MachineTestBuilder* withInstanceMachineSize(int size) {
		this->instanceMachineSize = size;
		return this;
	}

	MachineTestBuilder* withInstanceMachine(InstanceMachine *instance) {
		this->instance = instance;
		return this;
	}

	MachineTestBuilder* withMaxBatches(int max) {
		this->machineMaxBatches = max;
		return this;
	}

	MachineTestBuilder* withSchedule(Schedule *schedule) {
			this->schedule = schedule;
			return this;
		}

	Machine* build() {
		if (this->instance == NULL) {
			this->instance = new InstanceMachine(instanceMachineId, instanceMachineSize);
		}
		if (this->schedule == NULL) {
			this->schedule = new Schedule(loadTestInstance());
			this->schedule->machines.clear();
		}
		Machine *machine = new Machine(this->instance, this->machineMaxBatches, this->schedule);
		this->schedule->machines.push_back(machine);
		return machine;
	}

};

