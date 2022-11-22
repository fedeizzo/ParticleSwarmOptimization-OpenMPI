#!/usr/bin/env bash
helpMessage=

usage() {
  echo "usage $0: [--cluster] [container-tag]"
  echo
  echo Options:
  echo "  -c, --cluster                 Update cluster udocker container"
  echo "  -h, --help                    Print this help"
  echo
  exit 1
}

update_cluster_container() {
    colorPrint "Pulling latest image"
    udocker pull fedeizzo/pso:latest
    colorPrint "Creating container from latest image, this operation may take a while"
    udocker create --name=pso --force fedeizzo/pso:latest
    colorPrint "Done"
    udocker setup --execmode=F4 pso
}

if [[ $1 == "-h" || $1 == "--help" || $1 == "help" ]]; then
    echo $helpMessage
    exit 0
fi

if [[ $1 == "--cluster" || $1 == "-c" ]]; then
    update_cluster_container
else
    TAG="pso"
    [ ! -z $1 ] && TAG=$1
    docker build -t $TAG .
fi
