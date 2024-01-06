#!/bin/bash
# Script to do multiarch container builds using Docker buildx

function version()
{
    year=$(date +%-y)
    month=$(date +%-m)
    day=$(date +%-d)
    hour=$(date +%-k)
    min=$(date +%-M)
    daymin=$((hour * 60 + min))
    buildnumber="${year}.${month}.${day}.${daymin}"

    echo "$buildnumber"
}


arg=$1

builddir=$PWD

[ ! -f /etc/buildkitd.toml ] && { echo "Docker buildx not configured for private registry"; exit 1; }

if ! docker buildx create --use --bootstrap --name zmqbuilder --driver docker-container --config /etc/buildkitd.toml ; then
    echo "Error creating docker buildx builder"
    exit 1
fi

newTag=$(version)

case $arg in
zmq-proxy)
    cd ../zmq-proxy || exit
    docker buildx build -f "$builddir/Dockerfile.zmq-proxy" --platform linux/amd64,linux/arm64 --push -t fir.love.io:3005/zmq-proxy:latest -t "fir.love.io:3005/zmq-proxy:${newTag}" .
    cd "$builddir" || exit
    ;;
zmq-stack)
    cd ../zmq-stack || exit
    docker buildx build -f "$builddir/Dockerfile.zmq-stack" --platform linux/amd64,linux/arm64 --push -t fir.love.io:3005/zmq-stack:latest -t "fir.love.io:3005/zmq-stack:${newTag}" .
    cd "$builddir" || exit
    ;;
conv-stack)
    cd ../conv-stack || exit
    docker buildx build -f "$builddir/Dockerfile.conv-stack" --platform linux/amd64,linux/arm64 --push -t fir.love.io:3005/conv-stack:latest -t "fir.love.io:3005/conv-stack:${newTag}" .
    cd "$builddir" || exit
    ;;
net-stack)
    cd ../net-stack || exit
    docker buildx build -f "$builddir/Dockerfile.net-stack" --platform linux/amd64,linux/arm64 --push -t fir.love.io:3005/net-stack:latest -t "fir.love.io:3005/net-stack:${newTag}" .
    cd "$builddir" || exit
    ;;
esac

docker buildx rm zmqbuilder
