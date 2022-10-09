#!/bin/sh

# Query
#PBS -l select=1:ncpus=1:mem=1mb
# Max execution time
#PBS -l walltime=0:05:00
# Execution Queue
#PBS -q short_cpuQ

# SCRIPT: run.sh
# AUTHOR: Samuele Bortolotti <samuele@studenti.unitn.it>
# AUTHOR: Federico Izzo <federico.izzo@studenti.unitn.it>
# DATE:   2022-10-09
#
# PURPOSE: Run MPI program on the cluster
# Input: file name

# Number of processes
if [ -z PROCESS_NUMBER ]; then
  PROCESS_NUMBER=2
fi

usage() {
  test $# = 0 || echo "$@"
  echo "Usage: $0 PROCESS_NUMBER [DEFAULT = ${PROCESS_NUMER}]"
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

eval "set -- $args"

if [ $# -eq 1 ]; then
  PROCESS_NUMBER=$1
fi

module load mpich-3.2
mpirun.actual -n $PROCESS_NUMBER ~/hpc-2022/bin/hpc
