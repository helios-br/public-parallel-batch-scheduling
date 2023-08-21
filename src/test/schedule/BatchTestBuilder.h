#pragma once

#include <vector>
#include "../instance/InstanceJobTestBuilder.h"
#include "../instance/InstanceFamilyTestBuilder.h"
#include "../../main/instance/InstanceFamily.h"
#include "../../main/schedule/Batch.h"

struct BatchTestBuilder {

private:
	InstanceFamily *instanceFamily;
	int maxSize;

	BatchTestBuilder(InstanceFamily *instanceFamily, int maxSize) {
		this->instanceFamily = instanceFamily;
		this->maxSize = maxSize;
	}

public:

	inline static Batch* createBatchWithDummyFamily(int maxSize) {
		return BatchTestBuilder(&DUMMY_INSTANCE_FAMILY, maxSize).build();
	}

	BatchTestBuilder* withFamily(InstanceFamily *instanceFamily) {
		this->instanceFamily = instanceFamily;
		return this;
	}

	BatchTestBuilder* withMaxSize(int maxSize) {
		this->maxSize = maxSize;
		return this;
	}

	Batch* build() {
		Batch *batch = new Batch(this->instanceFamily, this->maxSize);
		cout << "New batch created: " << batch->toString() << endl;
		return batch;
	}

};
