#pragma once

#include <iostream>
#include "../../main/instance/InstanceFamily.h"

inline InstanceFamily createInstanceFamily() {
	InstanceFamily instanceFamily = InstanceFamily(0, 65000, 0);
	cout << "New instance family created: " << instanceFamily.toString() << endl;
	return instanceFamily;
}
