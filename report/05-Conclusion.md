\newpage
# Conclusion

## Is parallelization always a good choice?
During our benchmarking analysis we have surprisingly realized that the thread parallelization is not always a good choice.

Due to the high overhead implied by the thread generation, we have observed that using OpenMP does not always result in a guaranteed speed benefit.

In cases in which the parallel region took little time to execute, it would be preferable to avoid parallelization and proceed with the straightforward execution of the code in a serial manner. 

Benchmarking in the case of thread parallelization is a task which if far from being trivial. In fact, every system may perform differently in presence or in absence of threads. Moreover, it is hard to decide whether to parallelize or not sime piece of code based on general assumptions.

As an effective parallelization, we have started our project by parallelizing each for loop in the code. This has resulted in a waste of resource and a worsening of performances for small size problems. On the other hand, employing parallelization implies a significant performance boost for big dimensionality problems. 

## Thread allocation pattern
