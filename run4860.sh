# ./run4860.sh num_machine_instances instance_index algorithm num_executions
# num_machine_instances = 1, 2, 3, 4, 5, 6, 9, 10, 12, 15, 18, 20, 27, 30
# instance_index = [0..num_machine_instances-1]

# example: ./run4860.sh 1 0 pils 10 (runs pils algorithm for all 4860 problem instances 10 times on the (unique) machine)

# example: ./run4860.sh 2 0 pils 10 (runs pils algorithm for the first 2430 problem instances 10 times on the first machine)
# example: ./run4860.sh 2 1 pils 10 (runs pils algorithm for the first 2430 problem instances 10 times on the second machine)

clear

numInstances=$1
numInstanceIndex=$2
solver=$3
numExecutions=$4

directory="./chiang-instances/"
fullpath=""

batchSize=$((4860 / numInstances))
firstElement=$(((batchSize * numInstanceIndex) + 1))
lastElement=$((firstElement + batchSize - 1))
index=0

echo -e 'Running application...'
echo

echo "Number of instances (machines): " $numInstances
echo "Number of problem instances: " $batchSize
echo "First element (index): " $firstElement
echo "Las element (index): " $lastElement
echo "Solver: " $solver
echo "Number of executions: " $numExecutions

cd output

for FILE in `ls ./chiang-instances/`; do
	if [[ "$FILE" != *readme.txt ]]; then
	
		index=$((index + 1))
		if (($index < firstElement || $index > lastElement)); then
			continue
		fi
	
		fullpath=$directory$FILE
		
		for iter in $(seq 1 $numExecutions); do
			./app $fullpath $solver $(hostname)
		done
	fi			
done

echo
echo -e '# Done!'
echo

sudo shutdown -h now
