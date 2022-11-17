#!/usr/bin/env bash
helpMessage="usage $0: [container-tag]"

if [[ $1 == "-h" || $1 == "--help" || $1 == "help" ]]; then
    echo $helpMessage
    exit 0
fi

TAG="pso"
[ ! -z $1 ] && TAG=$1
docker build -t $TAG .
