#!/usr/bin/env bash
helpMessage="usage $0: [container-tag]"

TAG="pso"
[ ! -z $1 ] && TAG=$1
docker build -t $TAG .
