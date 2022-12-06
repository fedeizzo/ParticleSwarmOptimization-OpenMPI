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

The number of tests we have ran in total is around 980 (TODO write correct number), in particular we tried every possible combination of different parameters:

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

Table 1 shows the amount of jobs we have run and the associated fail rate (we have decided to show only the amount of processes and not the threads one for presentation purposes).

| Processes number | Total | Failed | Fail rate |
|------------------|-------|--------|-----------|
| 1                | 110   | 18     | 16.36     |
| 2                | 109   | 51     | 46.79     |
| 4                | 104   | 52     | 50.00     |
| 8                | 105   | 59     | 56.19     |
| 16               | 105   | 64     | 60.95     |
| 32               | 91    | 63     | 69.23     |
| 64               | 56    | 43     | 76.79     |

The table highlights a correlation between the failure rate and the number of processes. 

As a result of a first analysis, none of the failed runs were caused by an errore in the code, most of them were failed duet to the time exceed error. Thus, we have tried to investigate the main reason behind this weird behavior. 

To begin with, we have kept constant the number of processes and we have increased the number of chunks for our jobs. 

Figure {@fig:time-thread-correlation} shows the number of failed runs associated with the corresponding number of threads. As a matter of fact, the more the requested chunks, the more the cores for the job are. Hence, since the number of MPI processes is always the same, unused cores can host threads, which could be a reasonable explanation for the low amount of failed jobs in higher chunks requests.

This proof of concept highlights how the overhead paid for a continuos context switch introduced by OpenMP is dramatically higher than the performance gain due to the parallelization. Therefore, we came to the conclusion that since several optimizations are already included within modern compilers such as [GCC](https://gcc.gnu.org/), OpenMP introduces only an unwanted overhead for the problem that we are facing. Hence, the optimal scenario is represented by the single threaded multi-process case.


![Thread and time exceed failures correlation](./images/time_threads_correlation.pdf){ height=150px }{#fig:time-thread-correlation}

The previously described phenomena is also observable from figure {@fig:threads-performance}. There, it is possible to see that the execution time increases when the number of threads increases. Specifically, the dots in the plot represent the executed jobs while the size of the dots expresses the number of correctly terminated runs used for to compute the average.

![Thread and time exceed failures correlation](./images/threads_performance.pdf){ width=250px }{#fig:threads-performance}

A part from the efficiency issues in the the multi-threaded scenario, we have deepen our benchmark analysis by considering single threaded jobs. Figure {@fig:processes-performance} shows the execution time difference between various configuration places, where *excl* means exclusive. 

The plot suggests that the difference in terms of execution time between exclusive chunks and shared ones is marginal with respect to the entire time needed for the job execution. This inevitably implies that the computation time of the job is markedly higher then the scheduling time between other users' processes. For the same reason, we can claim that the overall computation time does not suffer from the network overhead. The last statement can be appreciated from the small differences in term of execution time between pack and scatter jobs executions.

![Thread and time exceed failures correlation](./images/processes_performance.pdf){ width=250px }{#fig:processes-performance}

Furthermore, we have highlighted an elbow point in figure {@fig:processes-performance}, visible in the right plot. This spot identifies the best visual tradeoff between number of processes and the execution time of the parallel solution. Indeed, with more then 16 processes the gain does not motivate the expenses associated to the PBS request.

To validate this hypothesis we have created speedup and efficiency plots with respect to SOTA serial solution presented in section {@sec:sota-analysis}.