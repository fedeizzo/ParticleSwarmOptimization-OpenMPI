\newpage
# Benchmarking {#sec:benchmarking}
After implementing the algorithm, we wanted to understand how our parallelization impacts the performance of the algorithm.

Hence, we devised a full analysis of our algorithm performance changing the number of threads, the number of processes and the pBS process allocation pattern, in order to understand how the running time would have been affected.

## Problem configuration
In order to understand how our parallelization improved the running time, we devised a configuration file which is the same for every run, so as to have a common baseline.

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
In order to have high-quality and trustworthy results to examine, as indicated in the repository structure, we created a script that allowed us to send thousands of tasks to the University's HPC cluster over the course of many days. The script keeps a limit on the user current submitted job in order to do not monopolize the cluster and comply with the cluster policies. Precisely, we have set a $15$ process limit and every $10$ seconds the script checks whether the user has $15$ or more running processes in the cluster. If it is the case, then the script waits, otherwise it submits a new job to the scheduler. 

The total number of tests we have ran in total is around around 960, in particular we tried every possible combinations of different parameters:

* processes: chosen between `[1 2 4 8 16 32 64]`
* threads: chosen between `[1 2 4 8 16 32 64]`
* select: chosen between `[1 2 3 4 5]`
* places: chosen between `[pack scatter pack:excl scatter:excl]`
TODO: spiegare select and places

select equivale al numero di chunk (un chunk un insieme core/socket non necessariamente sulla stessa macchina).
ncpus e' il numero di core che voglio per ogni chunk.
pack mette tutti i chunk sulla stessa macchina, scatter li divide su macchine diverse SEMPRE mentre le versioni excl riservano gli interi ?nodi/chunk? alla tua creazione

We decided to increase the processes and threads number by power of 2 because the most complex algorithm (following the asymptotic notation) should be $\mathcal{O}(nlogn)$ (TODO verificare questa complessita')

Since it takes 50 minutes on average for each run, we had to stretch out the submission of jobs across many days (two weeks).

## Results
During the execution of the bencmarking phase two problems occured:

* one student saturates the available space in the home of the cluster and for this reason some of our runs were not able to write results on disk at the end of the execution;
* we decided to sumbit all jobs to the short job queue of the cluster, and some jobs failed for *time exceed* error or missing resources.

All configurations were launched by both of them in order to validate results and reduce possible noise. In Table 1 it is presented the amount of jobs and the associated fail rate (it is shown only the amount of cores for presentation reasons)

| Processes number | Total | Failed | Fail rate |
|------------------|-------|--------|-----------|
| 1                | 110   | 18     | 16.36     |
| 2                | 109   | 51     | 46.79     |
| 4                | 104   | 52     | 50.00     |
| 8                | 105   | 59     | 56.19     |
| 16               | 105   | 64     | 60.95     |
| 32               | 91    | 63     | 69.23     |
| 64               | 56    | 43     | 76.79     |

From the table it is possible to notice that it seems to be a correlation between the failure rate and the number of processes. Given the fact that most of the fails are a consequence of the time exceed problem we tried to investigate the reason for this behavior. With a constant number of processes we tried to increase the number of chunks required for our jobs. In figure {@fig:time-thread-correlation} it is possible to notice the number of failed runs and the number of threads. With an increased number of chunk the cluster allocates more pyshical cores to the job, but since the number of processes passed to OpenMPI is always the same those cores can be used by the threads. This is an evidence that explains how the overhead paid for continuos thread context switch introduced by OpenMP is greater than the performance gain, several optimizations are already made by compilers and the usage of OpenMP only introdues another layer of overhead.

![Thread and time exceed failures correlation](./images/time_threads_correlation.pdf){ height=150px }{#fig:time-thread-correlation}

This phenomena is observable also in figure {@fig:threads-performance} where it is possible to notice that the execution time increases when the number of threads is increased. Dots in the plot represent the executed jobs and the size of the dot expresses the number of correclty terminated runs.

![Thread and time exceed failures correlation](./images/threads_performance.pdf){ width=250px }{#fig:threads-performance}

Given the problem associated with threads we made further analysis only considering jobs with one thread. In figure {@fig:processes-performance} it is possible to notice that the execution measured execution time difference between an exclusive (excl suffix) and a shared node is marginal with respect to the all time required for the job. This similarity is a consequence of the fact that the time required for the internal job of a core is higher then the time spent to wait the core (questa e' da verificare se e' vera). For the same reason the overall computation time is not influenced by the network overhead, this can be understood by the similarity time between pack and scatter jobs.

![Thread and time exceed failures correlation](./images/processes_performance.pdf){ width=250px }{#fig:processes-performance}

Alwayse in figure {@fig:processes-performance} we identified an elbow point highlighted in the right plot. That specific spot should identifies the maximum efficency of the parallel solution, with more then 16 processes the gain does not motivate the expenses associated to the PBS request.
To validate this hypotesis we have created speedup and efficency plots with respect to SOTA serial solution presented in section {@sec:sota-analysis}
