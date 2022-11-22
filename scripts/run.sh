#!/bin/sh

# Directives to PBS

#PBS -l select=1:ncpus=5:mem=1mb
#PBS -l walltime=2:00:00
#PBS -q short_cpuQ
#PBS -o run_output.txt
#PBS -e run_error.txt

# SCRIPT: run.sh
# AUTHOR: Samuele Bortolotti <samuele@studenti.unitn.it>
# AUTHOR: Federico Izzo <federico.izzo@studenti.unitn.it>
# DATE:   2022-10-09
#
# PURPOSE: Runs the MPI program on the cluster
# PROCESS_NUMBER: number of provessed to employ, default 2

# If number of processes is not specified, then declare it
if [ -z $PROCESS_NUMBER ]; then
  PROCESS_NUMBER=5
fi
if [ -z $CONFIG_PATH ]; then
  CONFIG_PATH="$HOME/ParticleSwarmOptimization-OpenMPI/pso-data.ini"
fi
if [ -z $NUMBER_OF_THREADS ]; then
  NUMBER_OF_THREADS=1
fi

echo $CONFIG_PATH $PROCESS_NUMBER $NUMBER_OF_THREADS
exit 1

colorPrint() {
  echo -e "$(tput setaf 6)$1$(tput sgr0)"
}

errorPrint() {
  echo -e "$(tput setaf 1)$1$(tput sgr0)"
}

usage() {
  test $# = 0 || echo "$@"
  echo "Usage: $0 PROCESS_NUMBER [DEFAULT=$PROCESS_NUMBER]"
  echo
  echo Runs the MPI program on the cluster
  echo Options:
  echo "  -h, --help                    Print this help"
  echo
  exit 1
}

args=
while [ $# != 0 ]; do
  case $1 in
    -h|--help) usage ;;
    -?*) usage "Unknown option: $1" ;;
    *) args="$args \"$1\"" ;;
  esac
  shift
done

# Get args
eval "set -- $args"

# If the command line variable is specified and it is an integer, then override the environment variable
if [ $# -eq 1 ] && [[ $1 =~ ^[[:digit:]]+$ ]]; then
  PROCESS_NUMBER=$1
fi

# Run the MPI program
echo $HOSTNAME
time mpiexec -n $PROCESS_NUMBER udocker run \
       -v $CONFIG_PATH:/src/bin/pso-data.ini \
       --hostenv --hostauth --user=$(id -u) --nobanner \
       pso \
       ./particle-swarm-optimization \
       -u \
       --number-of-threads=$NUMBER_OF_THREADS \
       pso-data.ini
