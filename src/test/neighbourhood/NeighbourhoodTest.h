#pragma once

#include "../../main/neighbourhood/Neighbourhood.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testNeighbourhoodToString() {
	string name = neighbourhoodToString(FULL_BATCH_INSERTION);
	cout << "Name: " << name << endl;
	assert((name == string("FULL_BATCH_INSERTION")) && "Invalid name");
	name = neighbourhoodToString(FULL_SEQUENTIAL_JOB_INSERTION_BILYK);
	cout << "Name: " << name << endl;
	assert((name == string("FULL_SEQUENTIAL_JOB_INSERTION_BILYK")) && "Invalid name");
}

inline void runNeighbourhoodTest() {
	cout << "# Running NeighbourhoodTest.h" << endl;

	cout << endl << ".... testNeighbourhoodToString [STARTED]" << endl;
	testNeighbourhoodToString();

	cout << endl << "[OK] NeighbourhoodTest.h" << endl;
}
