-----[Paper]-----

Tsung-Che Chiang, Hsueh-Chien Cheng, and Li-Chen Fu, A memetic algorithm for minimizing total weighted tardiness on parallel batch machines with incompatible job families and dynamic job arrival, Computers & Operations Research, vol. 37, no. 12, pp. 2257 - 2269, 2010.



-----[Filename & Parameter setting]-----

COR10-F-M-JF-B-Alpha-Beta-InstanceId.txt

F: Number of families
M: Number of machines
JF: Number of jobs in each family
B: Maximum batch size

There are 10 instances for each combination of F, M, JF, B, Alpha, and Beta.

There are 3x3x3x2x3x3 = 486 combinations and thus 4860 instances in total.

(Please see Table 2 in the paper.)



-----[File format]-----

First line: [M] [JF] [B]
Second line: [F] followed by processing times of [F] families

From the fourth line, there are [F] groups of [JF] lines, separated by an empty line.
Each line in a group represents data of a job, in which the three values are arrival time, due date, and weight, respectively.




-----[Contact information]-----

If there is any problem when using these problem instances, please feel free to contact the first author through tcchiang@ieee.org.