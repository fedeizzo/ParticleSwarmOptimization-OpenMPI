# Particle Swarm Optimization OpenMPI
*ParticleSwarmOptimization-openMPI* provides an basic implementation of [PSO](https://en.wikipedia.org/wiki/Particle_swarm_optimization) with support for cluster computation through [OpenMPI](https://www.open-mpi.org/), moreover it uses [OpenMP](https://www.openmp.org/) for thread parallelization.

<a href="https://fedeizzo.github.io/ParticleSwarmOptimization-OpenMPI/report.pdf" target="_blank"><b>PDF report can be found here</b></a>

## How to use
### Build
#### Binary
```bash
make
```

#### Docker
```bash
make docker-build
```

with optional argument `DOCKER_TAG`

```bash
make DOCKER_TAG=mytag docker-build
```

#### Udocker
```bash
make cluster-pull
```


### Run
#### Local
```bash
./bin/particle-swarm-optimization pso-data.ini
```

for optional arguments plese execute `./bin/particle-swarm-optimization --help`.

#### Cluster (through PBS)
Number of runs should be specified directly inside [this script](./scripts/generate_cluster_runs.sh)
```bash
make cluster-run
```

### Report
```bash
make report
```
