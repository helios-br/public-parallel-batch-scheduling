#pragma once

#include <iostream>
#include "../../main/instance/Instance.h"
#include "../../main/instance/ChiangInstanceLoader.h"

using namespace std;

inline Instance* loadTestInstance() {
	char fileName[] = "INSTANCE-FOR-TEST.txt";
	return loadChiangInstance(fileName);
}
