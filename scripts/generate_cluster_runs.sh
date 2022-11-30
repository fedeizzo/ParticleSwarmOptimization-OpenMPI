#!/usr/bin/env bash
# in order to delete all started jobs
# qstat -u $USER | tail -n +6 | awk -F' ' '{print $1}' | awk -F '.' '{print $1}' | xargs qdel {}
PROCESSES="1 4 8 16 32 64"
THREADS="1 8 16 32"
SELECT="1 2 3 4 5"
PLACES="pack scatter pack:excl scatter:excl"
mkdir -p log_second_try
mkdir -p configs
COUNTER=0
TOTAL=0
for node in $SELECT; do
    for place in $PLACES; do
	for p in $PROCESSES; do
	    for t in $THREADS; do
		TOTAL=$((TOTAL + 1))
	    done
	done
    done
done
for node in $SELECT; do
    for place in $PLACES; do
	for p in $PROCESSES; do
	    for t in $THREADS; do
		total_cores=$(echo "$p*$node" | bc)
		queue="short_cpuQ"
		time="2:00:00"
		if [ $t -ge 9 ]; then
		    time="4:00:00"
		fi
		if [ $t -ge 17 ]; then
		    time="6:00:00"
		fi
		if [ $total_cores -ge 97 ]; then
		    queue="long_cpuQ"
		fi
		while [ $(qstat -u $USER | tail -n +6 | grep 'short' | wc -l) -ge 15 ]; do
		    sleep 10
		done
		qsub \
		    -N pso-${p}-${t}-${place//:/_}-${node} \
		    -o log_second_try/out-${p}-${t}-${place//:/_}-${node} \
		    -e log_second_try/err-${p}-${t}-${place//:/_}-${node} \
		    -l select=$node:ncpus=$p:mem=1gb \
		    -l place=$place \
		    -l walltime=$time \
		    -q $queue  \
		    -v PROCESS_NUMBER=$((p*node)),NUMBER_OF_THREADS=$t \
		    ./scripts/run.sh
		COUNTER=$((COUNTER + 1))
		echo "Job $COUNTER/$TOTAL"
	    done
	done
    done
done

