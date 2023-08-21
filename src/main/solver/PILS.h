#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class PILS: public Solver {

	int minimumPopulationSize = 5; // μ = 25 (Vidal et al (2012))
	int generationSize = 10; // λ = 40, 70 or 100, depending on the type of problem (Vidal et al (2012))
	int maxPopulationSize = minimumPopulationSize + generationSize;
	float proportionOfEliteIndividuals = 0.4; // el = 0.4 (Vidal et al (2012))
	int numberEliteIndividuals = proportionOfEliteIndividuals * minimumPopulationSize; // nbElit = el * μ (Vidal et al (2012))
	float proportionOfCloseIndividuals = 0.2; // nc = 0.2 (Vidal et al (2012))
	int numberCloseIndividuals = ceil(proportionOfCloseIndividuals * minimumPopulationSize); // nclose = nc * μ (Vidal et al (2012))
	int maxIterationsWithoutImprovements = 250;

public:
	int getSolverInstanceId() {
		return 26;
	}

private:
	Schedule* solveInstance(Instance *instance);
};
