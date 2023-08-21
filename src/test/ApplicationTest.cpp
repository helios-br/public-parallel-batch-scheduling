#include "util/StringUtilTest.h"
#include "util/JobUtilTest.h"
#include "schedule/BatchTest.h"
#include "schedule/JobTest.h"
#include "schedule/MachineTest.h"
#include "schedule/ScheduleUtilTest.h"
#include "solver/TimeWindowDecompositionSolverTest.h"
#include "solver/IteratedGreedySolverTest.h"
#include "solver/IndividualTest.h"
#include "solver/PopulationTest.h"
#include "neighbourhood/BatchInsertionTest.h"
#include "neighbourhood/BatchSwapTest.h"
#include "neighbourhood/BatchMergeTest.h"
#include "neighbourhood/BatchSplitTest.h"
#include "neighbourhood/JobInsertionTest.h"
#include "neighbourhood/JobSwapTest.h"
#include "neighbourhood/SequentialBatchInsertionTest.h"
#include "neighbourhood/SequentialJobInsertionTest.h"
#include "neighbourhood/NeighbourhoodTest.h"
#include "neighbourhood/JobFastInsertionTest.h"
#include <iostream>

using namespace std;

inline void runTest() {

	cout << "Testing application!" << endl << endl;

	cout << endl << "------------------" << endl << endl;
	runNeighbourhoodTest();
	cout << endl << "------------------" << endl << endl;
	runJobUtilTest();
	cout << endl << "------------------" << endl << endl;
	runStringUtilTest();
	cout << endl << "------------------" << endl << endl;
	runBatchTest();
	cout << endl << "------------------" << endl << endl;
	runJobTest();
	cout << endl << "------------------" << endl << endl;
	runMachineTest();
	cout << endl << "------------------" << endl << endl;
	runScheduleUtilTest();
	cout << endl << "------------------" << endl << endl;
	runIndividualTest();
	cout << endl << "------------------" << endl << endl;
	runPopulationTest();
	cout << endl << "------------------" << endl << endl;
	runTimeWindowDecompositionSolverTest();
	cout << endl << "------------------" << endl << endl;
	runIteratedGreedySolverTest();
	cout << endl << "------------------" << endl << endl;
	runBatchInsertionTest();
	cout << endl << "------------------" << endl << endl;
	runBatchMergeTest();
	cout << endl << "------------------" << endl << endl;
	runBatchSplitTest();
	cout << endl << "------------------" << endl << endl;
	runBatchSwapTest();
	cout << endl << "------------------" << endl << endl;
	runJobInsertionTest();
	cout << endl << "------------------" << endl << endl;
	runJobSwapTest();
	cout << endl << "------------------" << endl << endl;
	runSequentialBatchInsertionTest();
	cout << endl << "------------------" << endl << endl;
	runSequentialJobInsertionTest();
	cout << endl << "------------------" << endl << endl;
	runJobFastInsertionTest();

	cout << "Done!" << endl;
}

