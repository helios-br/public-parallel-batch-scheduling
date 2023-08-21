#pragma once

#include <set>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Instance.h"
#include "InstanceJob.h"
#include "InstanceMachine.h"
#include "InstanceFamily.h"
#include "../util/StringUtil.h"

using namespace std;

inline Instance* loadChiangInstance(char *instanceName);
inline string normalizeChiangFileName(char *instanceName);

inline static ifstream STREAM;

/**
 * @brief Creates new instance from file
 *
 * @param instanceName Instance name on file system
 * @return Instance
 */
inline Instance* loadChiangInstance(char *instanceName) {
	// Load file
	string fileName = normalizeChiangFileName(instanceName);
	//cout << "Loading instance " << fileName << "..." << endl;

	if (!STREAM.is_open()) {
		STREAM.open(&fileName[0]);
	}

	// Multiplier applied on time related data
	int multiplier = 100;

	// Load data from file
	string lineFromfile;

	// Read first line
	getline(STREAM, lineFromfile);
	//cout << "lineFromfile: " << lineFromfile << endl;
	vector<string> headerTokens = split(removeBlankSpaces(lineFromfile), ' ');

	int numMachines = stoi(headerTokens[0]);
	int numJobsPerFamily = stoi(headerTokens[1]);
	int maxBatchSize = stoi(headerTokens[2]);

	// Read second line
	getline(STREAM, lineFromfile);
	//cout << "lineFromfile: " << lineFromfile << endl;
	vector<string> familyTokens = split(removeBlankSpaces(lineFromfile), ' ');
	int numFamilies = stoi(familyTokens[0]);
	int numJobs = numFamilies * numJobsPerFamily;

	vector<InstanceFamily*> allFamilies(numFamilies, &DUMMY_INSTANCE_FAMILY);
	//cout << "allFamilies size: " << allFamilies.size() << endl;
	//cout << "families: ";
	for (int f = 0; f < numFamilies; f++) {
		int processingTime = stoi(familyTokens[f + 1]);
		allFamilies[f] = new InstanceFamily(f, processingTime, numJobsPerFamily);
	}

	// Read third line (blank)
	getline(STREAM, lineFromfile);

	// Read jobs
	vector<InstanceJob*> allJobs;

	int familyIndex = 0;
	int jobId = 0;

	for (int j = 0; j < numJobs + numFamilies - 1; j++) {
		getline(STREAM, lineFromfile);
		if (removeBlankSpaces(lineFromfile) == "" || removeBlankSpaces(lineFromfile) == " ") {
			familyIndex++;
			continue;
		}

		vector<string> jobTokens = split(removeBlankSpaces(lineFromfile), ' ');

		int arrivalTime = stoi(jobTokens[0]);
		int dueDate = stoi(jobTokens[1]);

		// Due date value transformation
		string weightStr = replaceAll(jobTokens[2], ".", "");
		int weight = (stoi(weightStr));

		InstanceFamily *instanceFamily = allFamilies[familyIndex];
		InstanceJob *instanceJob =
				new InstanceJob(jobId++, instanceFamily, 1, dueDate, instanceFamily->processingTime, weight, arrivalTime);
		allJobs.push_back(instanceJob);
	}

	STREAM.clear();
	STREAM.seekg(0, STREAM.beg);

	// Prepare machines
	vector<InstanceMachine*> machines;
	for (int i = 0; i < numMachines; i++) {
		InstanceMachine *instanceMachine = new InstanceMachine(i, maxBatchSize);
		machines.push_back(instanceMachine);
	}

	Instance *instanceData = new Instance { replaceAll(fileName, "./chiang-instances/", ""), allJobs, allFamilies, machines, maxBatchSize, multiplier };
	//cout << "Instance created [OK]" << endl;
	return instanceData;
}

/**
 * @brief Appends file extension if necessary
 *
 * @param instanceName
 * @return string
 */
inline string normalizeChiangFileName(char *instanceName) {
	string fileName = instanceName;
	fileName = replaceAll(fileName, "./output/", "./");
	size_t foundDot = fileName.find(".txt");
	if (foundDot != string::npos) {
		return fileName;
	}
	return fileName + ".txt";
}
