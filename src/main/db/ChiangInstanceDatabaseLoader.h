#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include "DatabaseManager.h"
#include "ProblemInstanceRepository.h"

using namespace std;
using std::filesystem::directory_iterator;

inline void loadInstancesToDatabase() {

	for (auto const &directoryEntry : directory_iterator("./chiang-instances")) {
		string filePath = directoryEntry.path();

		if (filePath.compare("./chiang-instances/@readme.txt") == 0) {
			continue;
		}

		string fileName = filePath.substr(19, filePath.length() - 19);

		// clean filename
		filePath = filePath.replace(filePath.begin(), filePath.begin() + 25, "");

		// read parameters
		vector<string> tokens = split(filePath, '-');
		string numFamiliesStr = tokens[0].substr(1, tokens[0].length() - 1);
		string numMachinesStr = tokens[1].substr(1, tokens[1].length() - 1);
		string numJobsPerFamily = tokens[2].substr(2, tokens[2].length() - 1);
		string maxBatchesStr = tokens[3].substr(1, tokens[3].length() - 1);
		string alphaStr = tokens[4];
		string betaStr = tokens[5];
		string instanceIdStr = tokens[6].substr(0, tokens[6].find("."));
		string numJobsStr = to_string(stoi(numFamiliesStr) * stoi(numJobsPerFamily));

		// check data
		string validationName = "F" + numFamiliesStr + "-M" + numMachinesStr + "-JF" + numJobsPerFamily + "-B" + maxBatchesStr + "-" + alphaStr + "-" + betaStr
				+ "-" + instanceIdStr + ".txt";

		if (filePath.compare(validationName) != 0) {
			cout << "invalid data" << endl;
			exit(0);
		}

		saveProblemInstance(fileName, "chiang", numFamiliesStr, numMachinesStr, numJobsStr, numJobsPerFamily, maxBatchesStr, alphaStr, betaStr, instanceIdStr);
	}
}
