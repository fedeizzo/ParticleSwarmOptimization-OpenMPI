# Introduction
PSO focuses on two main definitions: the notion of *particle* and the one of *particle perception*.

A particle can be seen as an entity which is characterized by:

- a position $x$ depicting the *candidate solution* for our optimization problem;
- a velocity component $v$, which is used in order to *perturb* the particle;
- a performance measure $f(x)$, also called *fitness* value, which quantify the quality of the candidate solution.

The entire set of particles is referred as *swarm*.

Each particle needs to perceive the positions along with the associated performance measures of the *neighboring particles*. In this way, each agent remembers the position $z$ associated to the best performance of all the particles within the neighborhood, as well as its own best performance so far $y$.

This project implements a version of PSO considering *distance-based* neighborhood in a nearest neighbor fashion. In details, each particle has a fixed number of neighbors, which depend dynamically on the particle position on the landscape. Moreover, the program offers the user the possibility to modify the number of particles to consider within a particle neighborhood.

## Parametrization
PSO requires the following parameters to be set:

- *Swarm size*: typically 20 particles for problems with dimensionality 2-200;
- *Neighborhood size*: typically 3 to 5, otherwise global neighborhood;
- *Velocity update factors*.

## Continuous Optimization
A swarm of particles is initialized with random positions and velocity.

At each step, each particle updates first its velocity (equation {@eq:pso-update-velocity}): 

$$v' = w \cdot v + \phi_1 U_1 \cdot (y-x) + \phi_2 U_2 \cdot (z-x)$${#eq:pso-update-velocity}

where:

- $x$ and $v$ are the particle current position and velocity, respectively;
- $y$ and $z$ are the personal and social/global best position, respectively;
- $w$ is the inertia (weighs the current velocity);
- $\phi_1$, $\phi_2$ are acceleration coefficients/learning rates (cognitive and social, respectively);
- $U_1$ and $U_2$ are uniform random numbers in $[0,1]$.

Finally, each particle updates its position (equation {@eq:pso-update-position}): 

$$x' = x+v'$${#eq:pso-update-position}

and in case of improvement, updates $y$ (and eventually $z$).

## State-of-the-art analysis {#sec:sota-analysis}
As a first approach to the problem, we have surfed the web in order to look for pre-existing PSO implementations.

Based on what we have found, the approaches can be divided into three main categories:

1. those ones which aim to change the behavior of the algorithm introducing new features;
2. those ones which aim to solve a real world problem using PSO as main algorithm;
3. those ones which aim to optimize the runtime execution speed.

In our study we have decided to exclude the second category of PSO algorithms since these solutions are strictly problem dependent. Thus, a comparison would produce meaningless results. 

On the other hand, all those approaches which belong to first category of problems can be employed as case studies for our benchmarking analysis. However, it is strictly required to change some implementation aspects by modifying directly the code. In some cases, this requires a deep understanding of others' code, most of the time a though job due to the absence of documentation. 

The third category is our perfect competitor, since they share our same objective.
However, there are several cases in which different PSO version have been implemented. Hence, some hands on is still required.

In the following table we list some of the implementations we have decided to consider during the benchmarking phase.

| **Ref.**           | **Year** | **Type**      | **Code** | **Note** |
|--------------------|----------|---------------|----------|----------|
| [@KennedyEberhart] | 1995     | Serial        | No       | -        |
| [@toddguant]       | 2019     | Serial        | Yes      | 1        |
| [@sousouho]        | 2019     | Serial        | Yes      | 1        |
| [@kkentzo]         | 2020     | Serial        | Yes      | 1        |
| [@fisherling]      | 2020     | Serial        | Yes      | 1        |
| [@MoraesMitre]     | 2014     | MPI           | No       | -        |
| [@NedJahMoraes]    | 2017     | MPI/MP        | No       | -        |
| [@abhi4578]        | 2019     | MPI/MP,CUDA   | Yes      | 1        |
| [@LaSEEB]          | 2020     | OpenMP        | Yes      | 2        |
| [@pg443]           | 2021     | Serial,OpenMP | Yes      | 1        |

The indexes in the notes refer to:

1. provides only global neighborhood implementation. Thus, the comparison would be untruthful as those implementations have a clear advantage in the execution time due to a favorable topology;
2. provides PSO with different neighborhood versions but without a distance based approach. Hence, the implication are the same as for the point 1.

According to the previous statements, we claim that we have implemented a PSO version which differ from the ones we have decided to consider since it has a different notion of neighborhood which makes it harder to parallelize.

# Main step towards parallelization
In this section the report provides a detailed description of the major contribution we have provided to the serial parallelization in order to move towards an efficient hybrid OpenMP-MPI solution.

## Serial version of the algorithm
The main steps of the algorithm are:

1. initialize the particles in the swarm according to the problem dimensionality;
2. exchange particles' positions among within the swarm;
3. sort the particles according to a distance measure (euclidean distance) in ascending order;
4. update the particles position and velocity (eq. {@eq:pso-update-velocity} and {@eq:pso-update-position}).

As a first approach, we have tried to use OpenMP directives in order to generate a thread for each loop iteration whenever it was possible.

However, OpenMP *fork-join* model requires a non negligible overhead so as to spawn multiple threads which are eventually joined into the master at the end of the OpenMP block. For relatively small problems, this operation was a time-consuming procedure which leads to a significant rise in execution time with respect to the single thread model. Moreover, during the experiments we have not been able to observe the threads advantage we were hoping for. We assume that the main reason behind this non-tangible advantage are the optimization provided by \texttt{gcc} during at compile time and the non-optimal thread allocation patterns performed on the cluster. 

In the final version of the application, we have included the OpenMP directives only in the portion of the code where we thought it was needed, even if the advantage in terms of time were not satisfactory compared to the single threaded application.

For the neighborhood sort, the program relies on *quicksort* (figure \ref{fig:quicksort-algorithm}). The main reason behind this choice is the amount of parallelization this algorithm can provide.

\begin{figure}
    \centering
    \includegraphics[width=0.75\linewidth]{./images/quicksort.jpeg}
    \caption{Quicksort}
    \label{fig:quicksort}
\end{figure}
 
## Parallel version of the algorithm
We have distributed the workload among $N$ different processes in the cluster using the *MPI* library and we have exploited multiprocessing via OpenMP for a couple of different shared-memory tasks.

### Architecture
In order to subdivide the work and to carry out the final computation, the architecture proposed by the report focuses on the *all-to-all* parallel computational pattern (figure {@fig:communication-schema}).

*All-to-all* parallel pattern implemented using \texttt{MPI\_Allgather} is characterized by the exchange of individual messages from every process to any other process. 

![Communication schema.](./images/communication_schema.png){#fig:communication-schema}

### Message
To send a message between different processes we created a custom MPI data type called \texttt{broadcastMessage\_t}. Its purpose is to inform the receiver process about the particles' position and fitness of the sender. The structure is composed by a timestamp, which is needed for logging purposes on the sqlite, the current iteration of the algorithm, the identifier of the particle, the sender rank, and the current solution.

### Communication pattern
The communication between the different processes is synchronous.

Firstly, each process takes charge of a given number of particles.
In details, let $N$ be the number of particles the user has requested to program to manage and let $p$ be the number of processes available to *MPI*. Without the need of synchronization nor of message exchange, each process creates $N / p$ particles and the remaining $N \% p$ ones are split among the remaining processes. 

To carry out this operation, each process embeds its own particles in an array of \texttt{define\_datatype\_broadcast\_message}. Then, the particle information exchange happens with an \texttt{MPI\_Allgather} communication primitive.

As presented in figure {@fig:communication-schema}, \texttt{MPI\_Allgather} is suitable for the problem since it is an *all-to-all* communication channel and since it allows to reunite all the particles of each process into a single vector, which, at the end of the communication, will be equal for each process.

Once each process knows everything about the others, the application needs to consider the neighbor contributions in order to update the process particles' position and velocity.

At this point, each process can sort all the particles, whose position is known thanks to the \texttt{MPI\_Allgather} communication, with respect to all particles proper to the process, according to the euclidean distance. In this way, for each process particle is possible to identify the $k$-th nearest neighbors.

Finally, by applying the position and velocity update equations {@eq:pso-update-velocity} and {@eq:pso-update-position} it is possible to evolve the algorithm and approach the target function optima.

# Benchmarking
We devised a full analysis of our algorithm performance changing the number of threads, the number of processes and the PBS process allocation pattern, in order to understand how the running time would have been affected.

## Problem configuration
We devised a configuration file which is the same for every run, so as to have a common baseline.

The configuration is listed below:

* problemDimension = 50
* particlesNumber = 5000
* iterationsNumber = 500
* neighborhoodPopulation = 5000
* weights: w = 0.8, phi_1 = 0.3, phi_2 = 0.3
* functions: fitness = sphere, distance = euclidean, fitnessGoal = minimum

The amount of particles and the neighboord population are unreasonable for any known problem but they were chosen to show clearly the advantages brought by a multi process solution.


## Cluster jobs
In order to have high-quality and trustworthy results to examine, as indicated in the repository structure, we created a script that allowed us to send thousands of tasks to the University's HPC cluster over the course of many days.

The number of tests we have ran in total is around 980(TODO write correct number), in particular we tried every possible combination of different parameters:

* processes: chosen between \texttt{[1 2 4 8 16 32 64]};
* threads: chosen between \texttt{[1 2 4 8 16 32 64]};
* select: chosen between \texttt{[1 2 3 4 5]};
* places: chosen between \texttt{[pack scatter pack:excl scatter:excl]};

*Select* refer to the number of chunks requested to the PBS. A *chunk* can be seen as group of sockets, which are not necessarily on the same machine. 
*Place* can take one of the following values:

- \texttt{pack}: all the chunks are allocated within the same nodes;
- \texttt{scatter}: the chunks are located on different nodes; 
- \texttt{pack:excl}: the scenario is the same as \texttt{pack}, but the chunks are reserved entirely for the user application;
- \texttt{scatter:excl}: the scenario is the same as \texttt{scatter}, but the chunks are reserved entirely fo the user application.

## Results
All the job configurations were tested by both members of the group in order to validate and reduce possible noise of the results. 

Table \ref{tbl:job-results} shows the amount of jobs we have run and the associated fail rate (we have decided to show only the amount of processes and not the threads one for presentation purposes).

\begin{table}[h]
\centering
\begin{tabular}{llll}
\toprule
\textbf{Processes number} & \textbf{Total} & \textbf{Failed} & \textbf{Fail rate} \\
\midrule
1                & 110   & 18     & 16.36     \\
2                & 109   & 51     & 46.79     \\
4                & 104   & 52     & 50.00     \\
8                & 105   & 59     & 56.19     \\
16               & 105   & 64     & 60.95     \\
32               & 91    & 63     & 69.23     \\
64               & 56    & 43     & 76.79     \\
\bottomrule
\end{tabular}
\vspace{0.33cm}
\caption{Job results.}
\label{tbl:job-results}
\end{table}

The table highlights a correlation between the failure rate and the number of processes. 

As a result of a first analysis, none of the failed runs were caused by an errore in the code, most of them were failed duet to the time exceed error. Thus, we have tried to investigate the main reason behind this weird behavior. 

To begin with, we have kept constant the number of processes and we have increased the number of chunks for our jobs. 

Figure \ref{fig:time-thread-correlation} shows the number of failed runs associated with the corresponding number of threads. As a matter of fact, the more the requested chunks, the more the cores for the job are. Hence, since the number of MPI processes is always the same, unused cores can host threads, which could be a reasonable explanation for the low amount of failed jobs in higher chunks requests.

This proof of concept highlights how the overhead paid for a continuos context switch introduced by OpenMP is dramatically higher than the performance gain due to the parallelization. Therefore, we came to the conclusion that since several optimizations are already included within modern compilers such as [GCC](https://gcc.gnu.org/), OpenMP introduces only an unwanted overhead for the problem that we are facing. Hence, the optimal scenario is represented by the single threaded multi-process case.

\begin{figure}
    \centering
    \includegraphics[width=1\linewidth]{./images/time_threads_correlation.pdf}
    \caption{Threads and time exceed runs.}
    \label{fig:time-thread-correlation}
\end{figure}

The previously described phenomena is also observable from figure \ref{fig:threads-performance}. There, it is possible to see that the execution time increases when the number of threads increases. Specifically, the dots in the plot represent the executed jobs while the size of the dots expresses the number of correctly terminated runs used for to compute the average.

\begin{figure}
    \centering
    \includegraphics[width=1\linewidth]{./images/threads_performance.pdf}
    \caption{Thread and time exeuction runs.}
    \label{fig:threads-performance}
\end{figure}

A part from the efficiency issues in the the multi-threaded scenario, we have deepen our benchmark analysis by considering single threaded jobs. Figure \ref{fig:processes-performance} shows the execution time difference between various configuration places, where *excl* means exclusive. 

\begin{figure}
    \centering
    \includegraphics[width=1\linewidth]{./images/processes_performance_short_report.pdf}
    \caption{Processes and time execution runs.}
    \label{fig:processes-performance}
\end{figure}

The plot suggests that the difference in terms of execution time between exclusive chunks and shared ones is marginal with respect to the entire time needed for the job execution. This inevitably implies that the computation time of the job is markedly higher then the scheduling time between other users' processes. For the same reason, we can claim that the overall computation time does not suffer from the network overhead. The last statement can be appreciated from the small differences in term of execution time between pack and scatter jobs executions.

Furthermore, we have highlighted an elbow point in figure \ref{fig:processes-performance-elbow}, visible in the right plot. This spot identifies the best visual tradeoff between number of processes and the execution time of the parallel solution. Indeed, with more then 16 processes the gain does not motivate the expenses associated to the PBS request.

\begin{figure}
    \centering
    \includegraphics[width=1\linewidth]{./images/processes_performance_elbow_short_report.pdf}
    \caption{Processes and time execution runs elbow point.}
    \label{fig:processes-performance-elbow}
\end{figure}

To validate this hypothesis we have created speedup and efficiency plots with respect to SOTA serial solution presented in section {@sec:sota-analysis}.

# Final discussion

\newpage
# References
