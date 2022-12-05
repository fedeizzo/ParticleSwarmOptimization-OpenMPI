# Introduction
*Particle Swarm Optimization* focuses on main definitions: the notion of *particle* and the one of *particle perception*.
A particle can be seen as an entity which is characterized by:

- a position $x$ depicting the *candidate solution* for our optimization problem;
- a velocity component $v$, which is used in order to *perturb* the particle;
- a performance measure $f(x)$, also called *fitness* value, which quantify the quality of the candidate solution.

The entire set of particles is referred as *swarm*.

Each particle needs to perceive the positions along with the associated performance measures of the *neighboring particles*. In this way, each particle remembers the position $z$ associated to the best performance of all the particles within the neighborhood, as well as its own position where it obtained the best performance so far $y$.

This project implements a version of PSO considering *distance-based* neighborhood in a nearest neighbor fashion. In details, each particle has a fixed number of neighbors, which depend dynamically on the particle position on the landscape. Moreover, the program offers the user the possibility to modify the number of particles to consider within a particle neighborhood.

## Parametrization
PSO requires the following parameters to be set:

- *Swarm size*: typically 20 particles for problems with dimensionality 2-200;
- *Neighborhood size*: typically 3 to 5, otherwise global neighborhood;
- *Velocity update factors*.

## Continuous Optimization
A swarm of particles is initialized with random positions and velocity.

At each step, each particle updates first its velocity: 
$$v' = w \cdot v + \phi_1 U_1 \cdot (y-x) + \phi_2 U_2 \cdot (z-x)$$
where:

- $x$ and $v$ are the particle current position and velocity, respectively;
- $y$ and $z$ are the personal and social/global best position, respectively;
- $w$ is the inertia (weighs the current velocity);
- $\phi_1$, $\phi_2$ are acceleration coefficients/learning rates (cognitive and social, respectively);
- $U_1$ and $U_2$ are uniform random numbers in $[0,1]$.

Finally, each particle updates its position: $x' = x+v'$; and in case of improvement, update $y$ (and eventually $z$).

The pseudocode of the algorithm is shown below:
\begin{algorithm}[H]
\caption{Particle Swarm Optimization (Nearest Neighbors)}
\begin{algorithmic}[1]
\Function{PSO}{$\mathcal{S}$, $\mathcal{D}$,  $MAX\_IT$, $n$, $f$, $v$, $x$, $x_{min}$, $x_{max}$, $v_{max}$}

\State \Call{Initialize}{$\mathcal{S}$, $\mathcal{D}$, $f$, $v$, $x$, $x_{min}$, $x_{max}$, $v_{max}$} 

\State $it = 0$
\Repeat
\ForAll {particle $i \in \mathcal{S}$}
\If{$f(x_{i}) < f(pb_{i})$}
    \State $pb_{i} \gets x_{i}$ 
\EndIf
\EndFor
\State $\mathcal{S'} = $ \Call{Copy}{$\mathcal{S}$}
\ForAll {particle $i \in \mathcal S$}
\State $\mathcal{S'}$ = \Call{Sort}{S', i}
\ForAll {particle $j \in \mathcal{S'}$}
\If{$f(x_j) < f(gb_{i})$}
    \State $gb_i \gets x_j$
\EndIf
\EndFor
\EndFor
\ForAll {particle $i \in \mathcal S$}
\ForAll {dimension $d \in \mathcal D$}
    \State $v_{i, d} = v_{i, d} + C_1 \cdot Rnd(0, 1) \cdot [pb_{i, d} - x_{i, d}] + C_2 \cdot Rnd(0, 1) \cdot [gb_{d} - x_{i, d}]$
    \State $x_{i, d} = x_{i, d} + v_{i, d}$
\EndFor
\EndFor
\State $it \gets it + 1$
\Until{it $<$ MAX\_ITERATIONS}
\State \Return x
\EndFunction
\end{algorithmic}
\end{algorithm}

## State-of-the-art analysis
As a first approach to the problem, we have surfed the web in order to look for pre-existing PSO implementation.

Based on what we have found, the approaches can be divided into three main categories:

1. those ones which aim to change the behavior of the algorithm introducing new features;
2. those ones which aim to solve a real world problem using PSO as main algorithm.
3. those ones which aim to optimize the runtime execution speed.

In our study we have decided to exclude the second category of PSO algorithms since these solutions are strictly problem dependent. Thus, a comparison would produce meaningless results. 

On the other hand, all those approaches which belong to first category of problems can be employed as case studies for our benchmarking analysis. However, it is strictly required to change some implementation aspects by modifying directly the code. In some cases, this requires a deep understanding of others' code, most of the time a though job due to the absence of documentation. 

The third category is our perfect competitor, since they share our same objective.
However, there are several cases in which different PSO version have been implemented. Hence, some hands on is still required.

In the following table we list some of the implementations we have decided to consider during the benchmarking phase.

\begin{table}[]
\centering
\resizebox{\textwidth}{!}{%
\begin{tabular}{|l|l|l|l|l|}
\hline
\textbf{Authors}                                             & \textbf{Year} & \textbf{Type}       & \textbf{Source code}                                & \textbf{Notes} \\ \hline
Kennedy, Eberhart {[}@KennedyEberhart{]}                     & 1995          & Serial              & No                                                  &                \\ \hline
toddguant {[}@toddguant{]}                                   & 2019          & Serial              & {[}Yes{]}(https://github.com/toddgaunt/cpso)        & 1              \\ \hline
sousouho {[}@sousouho{]}                                     & 2019          & Serial              & {[}Yes{]}(https://github.com/sousouhou/succinctPSO) & 1              \\ \hline
kkentzo {[}@kkentzo{]}                                       & 2020          & Serial              & {[}Yes{]}(https://github.com/kkentzo/pso)           & 1              \\ \hline
fisherling {[}@fisherling{]}                                 & 2020          & Serial              & {[}Yes{]}(https://github.com/fischerling/pso)       & 1              \\ \hline
Moraes, Mitre {[}@MoraesMitre{]}                             & 2014          & Parallel OpenMPI    & No                                                  &                \\ \hline
Nedja, Moraes, Rogerio, Marcedo Mourelle {[}@NedJahMoraes{]} & 2017          & Parallel OpenMPI/MP & No                                                  &                \\ \hline
abhi4578 {[}@abhi4578{]} & 2019 & Parallel OpenMPI/MP, CUDA & {[}Yes{]}(https://github.com/abhi4578/Parallelization-of-PSO)       & 1 (da verificare) \\ \hline
LaSEEB {[}@LaSEEB{]}                                         & 2020          & Parallel OpenMP     & {[}Yes{]}(https://github.com/LaSEEB/openpso)        & 2              \\ \hline
pg443 {[}@pg443{]}       & 2021 & Serial, Parallel OpenMP   & {[}Yes{]}(https://github.com/pg443/Particle-Swarm-Optimizer-OpenMP) & 1                 \\ \hline
\end{tabular}%
}
\end{table}

Table: SOTA works.
The indexes in the notes refer to:

1. provides only global neighborhood implementation. Thus, the comparison would be untruthful as those implementations have a clear advantage in the execution time due to a favorable topology;
2. provides PSO with different neighborhood versions but without a distance based approach. Hence, the implication are the same as for the point 1.

According to the previous statements, we claim that we have implemented a PSO version which differ from the ones we have decided to consider since it has a different notion of neighborhood which makes it harder to parallelize.

# Main step towards parallelization
In this section the report provides a detailed description of the major contribution we have provided to the serial parallelization in order to move towards an efficient hybrid OpenMP-MPI paralellization.

## Serial version of the algorithm
As can be seen from the PSO pseudocode shown in the introduction, the main steps the algorithm has to face are:

1. initialize the particles in the swarm according to the problem dimensionality;
2. exchange particles' positions among within the swarm;
3. sort the particles according to a distance measure (euclidean distance) in ascending order;
4. update the particles position and velocity.

As a first approach, we have tried to use OpenMP directives in order to generate a thread for each loop iteration whenever it was possible.

However, OpenMP *fork-join* model requires a non negligible overhead so as to spawn multiple threads which are eventually joined into the master at the end of the OpenMP block. For relatively small problems, this operation was a time-consuming procedure which leads to a significant rise in execution time with respect to the single thread model. Moreover, during the experiments we have not been able to observe the threads advantage we were hoping for. We assume that the main reason behind this non-tangible advantage are the optimization provided by `gcc` during at compile time and the non-optimal thread allocation patterns performed on the cluster. 

In the final version of the application, we have included the OpenMP directives only in the portion of the code where we thought it was needed, even if the advantage in terms of time were not satisfactory compared to the single threaded application.

### Sorting algorithm
Concerning the sorting algorithm, the program relies on *quicksort* (figure {@fig:quicksort-algorithm}). The main reason behind this choice is the amount of parallelization this algorithm can provide. Indeed, merge sort has a better worst-case performance $\mathcal{O}(n \log n)$ with respect to quicksort having $\mathcal{O}(n^2)$ but requires synchronization in order to merge the partial solutions which is not required in quicksort. Moreover, its average performance is $\mathcal{O}(n \log n)$ as for merge sort.

Initially, one thread selects a pivot and moves the elements of the array which are smaller than the pivot value to the left and the elements which are bigger than the pivot value to the right.
The resulting two portions of the array wait until there is a free thread ready to process it. 
 
## Parallel version of the algorithm
We have distributed the workload among $N$ different processes in the cluster using the *MPI* library and we have exploited multiprocessing via OpenMP for a couple of different shared-memory tasks.

### Architecture
In order to subdivide the work and to carry out the final computation, the architecture proposed by the report focuses on the *all-to-all* parallel computational pattern (figure {@fig:communication-schema}).

*All-to-all* parallel pattern implemented using `MPI_Allgather` is characterized by the exchange of individual messages from every process to any other process. 

![Communication schema.](./images/communication_schema.png){#fig:communication-schema}

### Message
To send a message between different processes we created a custom MPI data type called `broadcastMessage_t`. Its purpose is to inform the receiver process about the particles' position and fitness of the sender. The structure is composed by a timestamp, which is needed for logging purposes on the sqlite, the current iteration of the algorithm, the identifier of the particle, the sender rank and the current solution.

### Communication pattern
The communication between the different processes is synchronous.

Firstly, each process takes charge of a given number of particles.
In details, let $N$ be the number of particles the user has requested to program to manage and let $p$ be the number of processes available to *MPI*. Without the need of synchronization nor of message exchange, each process creates $N / p$ particles and the remaining $N \% p$ ones are split among the remaining processes. 

The most interesting part in the algorithm parallelization is the program segment related to the message exchange among multiple processes.

To carry out this operation, each process embeds its own particles in an array of `define_datatype_broadcast_message`. Then, the particle information exchange happens with an `MPI_Allgather` communication primitive.

`MPI_Allgather` (figure {@fig:allgather}) primitive is suitable for the problem since it is an *all-to-all* communication primitive and since it allow to reunite all the particles of each process into a single vector, which, at the end of the communication, will be equal for each process.

Once each process knows everything about the others, the application needs to consider the neighbor contributions in order to update the process particles' position and velocity.

At this point, each process can sort all the particles, whose position is known thanks to the `MPI_Allgather` communication, with respect to all particles proper to the process, according to the euclidean distance. In this way, for each process particle is possible to identify the $k$-th nearest neighbors.

Finally, by applying the position and velocity update formulas listed in the PSO pseudocode it was possible to evolve the algorithm and approach the target function optima.

We have observed that the workload split implied by the algorithm enhance the PSO performances. A first, and trivial way to observe the advantage offered by *MPI* and optimization is to give a look at some of the expensive computation the serial algorithm has to carry out, in particular, let $n$ be the number of particles and let $m$ be the problem dimensionality. Moreover, during this reasoning process, we consider the average performance case and the single threaded scenario.

1. the distance computation is quadratic in the number of particles, while the euclidean distance is liner in the vectors' number of dimensions, hence the complexity is $\mathcal{O}(mn^2)$. In the application scenario, the distance needs to be computed a $\mathcal{O}(n^2)$ number of times, as it is required to know the relative position of each particle with respect to all the others. Hence, the complexity grows to $\mathcal{O}(mn^4)$;
2. the sorting algorithm, in the average performance scenario has a complexity which is given by $\mathcal{O}(n \log n)$, where $n$ is the number of particles. This operation has to be performed with respect to all the particle in the swarm, increasing the complexity to $\mathcal{O}(n^2 \log n)$;
3. finally, the particle's update is linear in the dimensionality of the problem, hence the complexity is $\mathcal{O}(m)$, which has to be performed for all the particle in the swarm, bringing the complexity to $\mathcal{O}(nm)$.

All those operations, are quite expensive in terms of time-complexity. Thanks to the workload sharing we have set up, the number each process has to manage is drastically reduced, and decreases the more processes MPI has at its disposal. Let $p$ be the number of processes. On average, each process has to manage $n/p$ particles, while the problem dimensionality remains untouched.

The complexity then decreases as follows:

1. $\mathcal{O}(m * (n/p)^4)$ for the distance computation;
2. $\mathcal{O}((n/q)^2 \log (n/q))$ for the sorting operation;
2. $\mathcal{O}((n/q)m)$ for the particles' variables update.

Despite this results being positive, we have to consider the time needed for each process to exchange their particle to each process. However, most HPC systems use *InfiniBand* interconnection, an high throughput, low latency connection among nodes in the cluster, therefore we claim that the advantage in terms of computational complexity remains legitimate since, in this scenario, the network has little impact on the application performance.

A visual proof of this statement is deeply discussed in the benchmarking section of the report.

# Benchmarking

# Final discussion

# References