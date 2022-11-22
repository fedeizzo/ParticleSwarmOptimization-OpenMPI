#!/usr/bin/env bash
# in order to delete all started jobs
# qstat -u $USER | awk -F' ' '{print $1}' | awk -F '.' '{print $1}' | xargs qdel {}
PROCESSES="1 2 4 8 16 32 64"
THREADS="1 2 4 8 16 32 64"
SELECT="1 2 3 4 5"
PLACES="pack scatter pack:excl scatter:excl"
mkdir -p log
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
		qsub \
		    -N pso-${p}-${t}-${place//:/_}-${node} \
		    -o log/out-${p}-${t}-${place//:/_}-${node} \
		    -e log/err-${p}-${t}-${place//:/_}-${node} \
		    -l select=$node:ncpus=$p:mem=2gb \
		    -l place=$place \
		    -v PROCESS_NUMBER=$p,NUMBER_OF_THREADS=$t \
		    ./run.sh
		COUNTER=$((COUNTER + 1))
		echo "Job $COUNTER/$TOTAL"
		if [[ $(echo "$COUNTER%40" | bc) == 0 ]]; then
		    echo $COUNTER
		    qsub -hold_jid "pso-*"
		fi
	    done
	done
    done
done

