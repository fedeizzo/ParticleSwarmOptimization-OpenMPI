\newpage
# Conclusion

Up until this point, we produced a hybrid OpenMP-MPI algorithm to solve complex continuous optimization problems, equipped with an efficient and reproducible DevOps pipeline.

We have realized that thread parallelization does not fit well all the problems.
Indeed, due to the high overhead implied by the thread generation, we have observed that using OpenMP worsen the result, not providing the much-wanted speed benefit.

Benchmarking in the case of thread parallelization is a task which is far from trivial. Every system may perform differently in the presence or absence of threads.  Moreover, it is hard to decide whether to parallelize or not some piece of code based on general assumptions. As an effective parallelization, we started our project by parallelizing each for loop in the code. This has resulted in a waste of resources and a worsening of performances for small-size problems. Unfortunately, the same has happened even in the case when the threads acted on the most time-consuming region of the code.

To conclude, the program we provided is suitable for single-threaded process parallelization and, as shown in the efficiency and speedup plots, it provides the best result when the number of processes is limited, as even if the computational time decreases, the more the processes the more the overhead required for the MPI communication to take place is.

## Future Work
As a further work, it would be interesting to complement the already present architecture with different type of neighborhood and analyze which configuration brought the best results in presence of parallelization, and in terms of quality of the provided solutions.
However, the scope of our project was to implement the above described parallel algorithm, which already posed significant challenges, especially because we could not base our implementation on pre-existing works.

\newpage
# References
