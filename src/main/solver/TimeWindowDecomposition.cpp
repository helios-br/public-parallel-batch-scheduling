#include "TimeWindowDecomposition.h"

#include "../schedule/Schedule.h"
#include "../schedule/Batch.h"
#include "../schedule/Machine.h"
#include  "../schedule/ScheduleCostProcessor.h"
#include "TWDUtil.h"

using namespace std;

/**
 * Time window decomposition heuristic
 * Andrew Bilyk, Lars Monch, and Christian Almeder
 * Scheduling jobs with ready times and precedence constraints on parallel batch machines using metaheuristics
 * Computers & Industrial Engineering, 78:175–185, 2014.
 */
Schedule* TimeWindowDecomposition::solveInstance(Instance *instance) {
	//cout << endl << "[twd-solve] // {start} // @ creating schedule // # machines: " << instance->instanceMachines.size() << endl;
	Schedule *schedule = new Schedule(instance, this->thres, this->lookAheadParam);

	// Define set M(f, t, delta-t) of unscheduled jobs of family f with ready time less or equal than (t + delta-t)

	int machineIndex = 0;
	int numberOfMachines = schedule->machines.size();

	int averageProcessingTimeRemainingJobs = 0;
	int totalProcessingTimeRemainingJobs = 0;
	int remainingJobs = schedule->jobs.size();

	int initialTime = 0;
	int delta = 0;
	int timeLimit = 0;
	double divisor = 0;
	vector<Job*> allJobs(schedule->jobs);

	for (Job *job : allJobs) {
		job->weightPriority = job->instanceJob->priorityWeight / (double) job->instanceJob->processingTime;
		totalProcessingTimeRemainingJobs += job->instanceJob->processingTime;
	}

	while (schedule->hasUnassignedJobs()) {

		//cout << endl << "@ new iteration" << endl;
		if (machineIndex == numberOfMachines) {
			machineIndex = 0;
		}

		Machine *machine = schedule->machines[machineIndex];

		// Average processing time of the remaining jobs p'
		averageProcessingTimeRemainingJobs = totalProcessingTimeRemainingJobs / remainingJobs;

		// Bylik, Monch, Almeder, 2014: delta_t = p'/2
		initialTime = machine->numBatches > 0 ? machine->batches[machine->numBatches - 1]->completionTime : 0;
		delta = averageProcessingTimeRemainingJobs / 2;
		timeLimit = timeLimit + delta;
		divisor = this->lookAhead * averageProcessingTimeRemainingJobs;

		//cout << endl << "[solve-" << machineIndex << "] // # averageProcessingTimeRemainingJobs: " << averageProcessingTimeRemainingJobs << endl;
		//cout << "[solve-" << machineIndex << "] // # initialTime: " << initialTime << ", timeLimit: " << timeLimit << endl;
		//cout << "[solve-" << machineIndex << "] // # divisor: " << divisor << endl;

		// Sort the jobs of each set M in non-increasing order by the ATC index
		//cout << "computeJobsPriority..." << endl;
		computeJobsPriority(allJobs, initialTime, timeLimit, divisor);
		//cout << "computeJobsPriority [ok]" << endl;

		// Derive the set M'(f, t, delta-t, thres) by choosing not more than the first thres jobs of each family from M
		// Consider all possible batch combinations from jobs of M' and assess each potential batch using the BATC-II index
		// Schedule the batch with the largest BATC-II index among the different job families

		//cout << "findBestBatch..." << endl;
		Batch *batch = findBestBatch(allJobs, instance->maxBatchSize, instance->numberFamilies, initialTime, timeLimit, divisor, this->thres);
		//cout << "findBestBatch [ok]" << endl;
		if (batch == NULL) {
			machineIndex++;
			continue;
		}
		machine->insertAfterLastBatch(batch);
		//machine->insertAtFirstAvailablePosition(batch);

		remainingJobs = remainingJobs - batch->currentSize;
		for (Job *job: batch->jobs) {
			if (!job->isDummy()) {
				totalProcessingTimeRemainingJobs = totalProcessingTimeRemainingJobs - job->instanceJob->processingTime;
			}
		}

		// Advance the ready time of the machine m to max(t, r_bj) + p_b, where p_b is the processing time of batch b
		// Define a new time window and go to Step 1 if there are still unscheduled jobs // @suppress("Ambiguous problem")
		// Parameters
		// Bylik, Monch, Almeder, 2014: delta_t = p'/2 and thres = 15 are used in all computational experiments since we know from (Mönch et al., 2005) that these settings are favorable

		machineIndex++;
	}

	// Apply costs
	calculateScheduleCostStateful(schedule);
	return schedule;
}
