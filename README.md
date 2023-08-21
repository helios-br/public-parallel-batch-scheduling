# Parallel Batch Scheduling

This work addresses a parallel batch machine scheduling problem subject to tardiness penalties, release dates, and incompatible job families. In this environment, jobs of the same family are partitioned into batches and each batch is assigned to a machine. The objective is to determine the sequence in which the batches will be processed on each machine with a view of minimizing the total weighted tardiness. To solve the problem, we propose a population-based iterated local search algorithm that makes use of multiple neighborhood structures and an efficient perturbation mechanism. The algorithm also incorporates the time window decomposition (TWD) heuristic to generate the initial population and employs population control strategies aiming to promote individuals with higher fitness by combining the total weighted tardiness with the contribution to the diversity of the population. Extensive computational experiments were conducted on 4860 benchmark instances and the results obtained compare very favorably with those found by the best existing algorithms.

### Configure Environment

	sudo apt update
	sudo apt upgrade

##### GCC/G++ installation

	sudo apt install build-essential
	gcc --version
	g++ --version
	
##### GIT installation

	sudo apt install git

### Repository

[https://github.com/helios-br/public-parallel-batch-scheduling](https://github.com/helios-br/public-parallel-batch-scheduling)

### Database configuration (MariaDB)

Skip! The code related to the database was commented out.
	

# How to execute application from AWS

	git clone 'repo_url' OR git pull
	make
	./runConsole.sh instance_name algorithm
	ex: ./runConsole.sh ./chiang-instances/COR10-F6-M3-JF50-B4-0.25-0.25-4 pils
