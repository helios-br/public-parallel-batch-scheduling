#pragma once

#include <iostream>
#include "Solver.h"
#include "IteratedGreedySolver.h" // ok
#include "SimulatedAnnealing.h" // ok
#include "VNSBilyk.h" // ok
#include "TWDBilyk.h" // ok
#include "ILS.h" // ok
#include "PILS.h" // ok
#include "TimeWindowDecomposition.h" // ok

inline static const string ITERATED_GREEDY = "ig"; // 10
inline static const string TIME_WINDOW_DECOMPOSITION = "twd"; // 2
inline static const string TWD_BILYK = "twdb"; // 18
inline static const string SIMULATED_ANNEALING = "sa"; // 11
inline static const string ILS_ENHANCED = "ilse"; // 14
inline static const string VNS_BILYK = "vnsb"; // 15
inline static const string POPULATIONAL_ILS = "pils"; // 26

inline Solver* getSolver(char *solverParam) {
	string solverStr = solverParam;

	if (solverStr.compare(TIME_WINDOW_DECOMPOSITION) == 0) { return new TimeWindowDecomposition(false); } // ok
		else if (solverStr.compare(TWD_BILYK) == 0) { return new TWDBilyk(); } // ok
		else if (solverStr.compare(ILS_ENHANCED) == 0) { return new ILS(); } // ok
		else if (solverStr.compare(POPULATIONAL_ILS) == 0) { return new PILS(); } // ok
		else if (solverStr.compare(VNS_BILYK) == 0) { return new VNSBilyk(); } // ok
		else if (solverStr.compare(ITERATED_GREEDY) == 0) { return new IteratedGreedySolver(); } // ok
		else if (solverStr.compare(SIMULATED_ANNEALING) == 0) { return new SimulatedAnnealing(); } // ok
		else { cout << "[ERROR] Solver parameter not valid" << endl; exit(0); }
}
