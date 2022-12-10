\newpage
# Benchmarking {#sec:benchmarking}
After implementing the algorithm, we wanted to understand how our parallelization impacts the performance of the proposed solution.

Hence, we devised a full analysis of our algorithm performance changing the number of threads, the number of processes and the PBS process allocation pattern, in order to understand how the running time would have been affected.

## Problem configuration
We devised a configuration file which is the same for every run, so as to have a common baseline.

The configuration is listed below:

```ini
[global]
problemDimension = 50
particlesNumber = 5000
iterationsNumber = 500
neighborhoodPopulation = 5000

[velocity]
w = 0.8
phi_1 = 0.3 
phi_2 = 0.3

[randomBounds]
initMaxPosition = 500.0
initMinPosition = -500.0
initMaxVelocity = 100.0
initMinVelocity = -100.0

[functions]
fitness = sphere
distance = euclidean
fitnessGoal = minimum
```

The amount of particles and the neighboord population are unreasonable for any known problem but they were chosen to show clearly the advantages brought by a multi process solution.


## Cluster jobs
In order to have high-quality and trustworthy results to examine, as indicated in the repository structure, we created a script that allowed us to send thousands of tasks to the University's HPC cluster over the course of many days. The script keeps a limit on the user current submitted job in order to do not monopolize the cluster and comply with policies. Precisely, we have set a $15$ process limit and every $10$ seconds the script checks whether the user has $15$ or more running jobs in the cluster. If it is the case, then the script waits, otherwise it submits a new job to the scheduler. 

The number of tests we have ran in total is around 1280, in particular we tried every possible combination of different parameters:

* processes: chosen between `[1 2 4 8 16 32 64]`;
* threads: chosen between `[1 2 4 8 16 32 64]`;
* select: chosen between `[1 2 3 4 5]`;
* places: chosen between `[pack scatter pack:excl scatter:excl]`;

Just to provide a common baseline, a PBS select option looks like this:

```
select=1:ncpus=5:mem=1mb
```

Where *select* refer to the number of chunks requested to the PBS. A chunk can be seen as group of sockets, which are not necessarily on the same machine. 
*ncpus* instead are the number of cores per chunk the user wants to have.

As for the select option, here we list the different meanings for the PBS places.
The option used to specify the PBS place is the following:

```
place=$place
``` 

Where `$place` can take one of the following values:

- `pack`: all the chunks are allocated within the same nodes;
- `scatter`: the chunks are located on different nodes; 
- `pack:excl`: the scenario is the same as `pack`, but the chunks are reserved entirely for the user application;
- `scatter:excl`: the scenario is the same as `scatter`, but the chunks are reserved entirely fo the user application.

Since it takes 50 minutes on average for each run, we had to stretch out the submission of jobs across many days (three weeks).

## Results
During the execution of the benchmarking phase two problems occurred:

* one student has saturated the available space in the the cluster home. For this reason, some of our runs were not able to store the results on disk at the end of the execution, hence, some computations got lost;
* we decided to submit all the jobs to the short job queue of the cluster. However, due to the problem difficulty and to the thread scheduling issues, some jobs failed as a result of *time exceed* or missing resources.

All the job configurations were tested by both members of the group in order to validate and reduce possible noise of the results. 

Figure {@fig:number_failed_per_procces} shows the amount of jobs we have run and the associated time exceeded rate.

![Number of time exceeded runs per process.](./images/number_of_failed_runs_per_process.pdf){#fig:number_failed_per_procces}

The presented figure highlights a correlation between the failure rate and the number of processes. Thus, we have tried to investigate the main reason behind this weird behavior. 

To begin with, we have kept constant the number of processes and we have increased the number of chunks for our jobs. 

Figure {@fig:time-thread-correlation} shows the number of failed runs associated with the corresponding number of threads. As a matter of fact, the more the requested chunks, the more the cores for the job are. Hence, since the number of MPI processes is always the same, unused cores can host threads, which could be a reasonable explanation for the low amount of failed jobs in higher chunks requests.

This proof of concept highlights how the overhead paid for a continuos context switch introduced by OpenMP is higher than the performance gain due to the parallelization. Therefore, we came to the conclusion that since several optimizations are already included within modern compilers such as [gcc](https://gcc.gnu.org/), OpenMP introduces only an unwanted overhead for the problem that we are facing. Hence, the optimal scenario is represented by the single threaded multi-process case.

![Thread and time exceed failures correlation](./images/time_threads_correlation.pdf){ height=150px }{#fig:time-thread-correlation}

The previously described phenomena is also observable from figure {@fig:threads-performance}. From there, it is possible to see that the execution time increases when the number of threads increases. Specifically, the dots in the plot represent the executed jobs while the size of the dots expresses the number of correctly terminated runs used to compute the average.

![Thread and time exceed failures correlation](./images/threads_performance.pdf){ width=250px }{#fig:threads-performance}

A part from the efficiency issues in the the multi-threaded scenario, we have deepen our benchmark analysis by considering single threaded jobs. Figure {@fig:processes-performance} shows the execution time difference between various configuration places, where *excl* means exclusive. 

The plot suggests that the difference in terms of execution time between exclusive chunks and shared ones is marginal with respect to the entire time needed for the job execution. This implies that the computation time of the job is markedly higher then the scheduling time between other users' processes. For the same reason, we can claim that the overall computation time does not suffer from the network overhead. The last statement can be appreciated from the small differences in term of execution time between pack and scatter jobs executions.

![Thread and time exceed failures correlation](./images/processes_performance.pdf){ width=250px }{#fig:processes-performance}

Furthermore, we have highlighted an elbow point in figure {@fig:processes-performance}. This spot identifies the best visual tradeoff between number of processes and the execution time of the parallel solution. Indeed, for the problem configuration used for our tests, more than 16 processes do not bring an enough gain in order to motivate the expenses associated to the PBS request.

As mentioned in section {@sec:sota-analysis}, the analysis of the available similar works has required to directly manage others' code.
Unfortunately, since the intricate problem was resolved in a matter of seconds, we claim that the findings are deceptive. We believe that the main reasons are the way some PSO instances perform the iterations since some of them stop whenever the solution is below an error threshold, hence performing fewer iterations than those required; as well as our difficulty in comprehending the actual behavior of other people's code, which has resulted in the development of an inconsistent algorithm. 

Based on the prior results, we have chosen to take into account only the multi-process solution, and we have deepened our investigation by examining the parallel performance improvement using speedup and efficiency graphs.
To begin with, the notion of scalability cannot be directly analyzed considering the problem we are optimizing. The reason for that regards the notion of *problem size*, which cannot be trivially defined.
Naively, one would conclude that the problem size is doubled when the problem dimension is doubled, however, the parallelization influence is limited only in the time for the position and velocity update.
On the other hand, we cannot argue that the problem size is doubled when the number of particles is doubled, since the problem persists, but holds in the opposite direction, namely only some portions of the code benefit from the parallelization.
Therefore, we claim that the concept of problem size is represented by a tight coupling between the problem dimension and the number of particles. Due to this non-trivial correlation, we have decided to focus only on one hard problem configuration and support our results with hundreds of runs.

![Speedup](./images/speedup.pdf){ width=150px }{#fig:speedup}

From figure {@fig:speedup} we can see that the speedup is very limited. This can be seen as a consequence of the time needed for communication between multiple processors, as at the end of each iteration all the processes must be synchronized and the number of exchanged messages is considerably high. Therefore, we claim that the overhead time we pay for the parallelization plays a relevant role, however, parallelization is still capable of providing a massive improvement in terms of time. This inevitably implies that the perfect parallelization and the ideal speedup cannot be achieved, as highlighted in the plot.

![Efficiency](./images/efficiency.pdf){ width=150px }{#fig:efficiency}

Likewise figure {@fig:speedup}, figure {@fig:efficiency} shows the efficiency curve considering only the jobs run with MPI, which is the reason why the efficiency does not start with $1$.
As a first consideration, the single process MPI job requires additional time compared to the serial version of the program to complete the execution, showing that the MPI, if not used properly introduces a non-negligible overhead.
The efficiency plot, on the other hand, shows that the best trade-off between the number of processes employed and the speedup gained has two peaks, respectively around $3$ and $12$ processes. Furthermore, we can see that the more the number of processes the less the efficiency is, which means that despite taking less time to execute, it is not convenient to employ a huge number of processes.
Moreover, figure {@fig:efficiency} highlights an irregular curve, we believe that this is a consequence of an inhomogeneous cluster, hence, there are nodes which are slower compared to others.

To conclude, table \ref{tbl:speedup-and-efficiency} provides a complete overview of the program analysis.

\begin{table}[h]
\centering
\begin{tabular}{crrrrc}
\toprule
\textbf{\# Pr.} & \textbf{Seconds} & \textbf{Diff} & \textbf{Speedup} & \textbf{Efficiency} & \textbf{Type} \\
\midrule
1     & 2099 & 0     & 1       & 1          & Serial  \\
1     & 4997 & -2898 & 0.41    & 0.41       & OpenMPI \\
2     & 2392 & -293  & 0.87    & 0.43       & OpenMPI \\
3     & 1564 & 535   & 1.34    & 0.44       & OpenMPI \\
4     & 1227 & 872   & 1.70    & 0.42       & OpenMPI \\
5     & 1128 & 971   & 1.86    & 0.37       & OpenMPI \\
8     & 629  & 1470  & 3.33    & 0.41       & OpenMPI \\
12    & 396  & 1703  & 5.29    & 0.44       & OpenMPI \\
16    & 307  & 1792  & 6.82    & 0.42       & OpenMPI \\
20    & 284  & 1815  & 7.38    & 0.36       & OpenMPI \\
24    & 218  & 1881  & 9.62    & 0.40       & OpenMPI \\
32    & 159  & 1940  & 13.18   & 0.41       & OpenMPI \\
40    & 155  & 1944  & 13.50   & 0.33       & OpenMPI \\
48    & 114  & 1985  & 18.32   & 0.38       & OpenMPI \\
64    & 93   & 2006  & 22.37   & 0.34       & OpenMPI \\
80    & 75   & 2024  & 27.95   & 0.34       & OpenMPI \\
96    & 77   & 2022  & 27.20   & 0.28       & OpenMPI \\
\bottomrule
\end{tabular}
\vspace{0.33cm}
\caption{Speedup and efficiency table.}
\label{tbl:speedup-and-efficiency}
\end{table}
