#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"
#include "../schedule/Job.h"
#include "../schedule/Batch.h"
#include "../util/NumberUtil.h"

class TimeWindowDecomposition: public Solver {

	// Vepsalainen, Ari P. J.; Morton, Thomas E. "For the ATC rule, look-ahead is extended to k = 3.0" (page 1042)
	double lookAhead = 3; // 0.1 * 30
	int lookAheadParam = 30;

	// Bylik, Monch, Almeder, 2014: thres = 15
	int thres = 15;

	// Evolutionary
	const int thresLowerBound = 10;
	const int thresUpperBound = 20;

	const int lookAheadParamLowerBound = 5;
	const int lookAheadParamUpperBound = 25;

public:

	TimeWindowDecomposition(double lookAhead) {
		this->lookAhead = lookAhead;
	}

	TimeWindowDecomposition(bool generateRandomParameters) {
		if (generateRandomParameters) {
			this->thres = generateRandomNumberInRange(thresLowerBound, thresUpperBound);
			this->lookAheadParam = generateRandomNumberInRange(lookAheadParamLowerBound, lookAheadParamUpperBound);
			this->lookAhead = 0.1 * (double) this->lookAheadParam;
		}
	}

	TimeWindowDecomposition(int thres, int lookAheadParam) {
		int tLowerBound = thres - 1 < thresLowerBound ? thresLowerBound : thres - 1;
		int tUpperBound = thres + 1 > thresUpperBound ? thresUpperBound : thres + 1;
		this->thres = generateRandomNumberInRange(tLowerBound, tUpperBound);

		int laLowerBound = lookAheadParam - 1 < lookAheadParamLowerBound ? lookAheadParamLowerBound : lookAheadParam - 1;
		int laUpperBound = lookAheadParam + 1 > lookAheadParamUpperBound ? lookAheadParamUpperBound : lookAheadParam + 1;
		this->lookAheadParam = generateRandomNumberInRange(laLowerBound, laUpperBound);
		this->lookAhead = 0.1 * (double) this->lookAheadParam;
	}

	int getSolverInstanceId() {
		return 2;
	}

private:

	Schedule* solveInstance(Instance *instance);
};
