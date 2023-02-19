#!/bin/bash

arg=$1

case $arg in
zmq-proxy)
    cp ../bin/zmq-proxy zmq-proxy/
    cd zmq-proxy || exit
    docker build -t zmq-proxy:latest -t fir.love.io:3005/zmq-proxy:latest .
    docker push fir.love.io:3005/zmq-proxy:latest
    cd ..
    ;;
zmq-stack)
    cp ../bin/zmq-stack zmq-stack/
    cd zmq-stack || exit
    docker build -t zmq-stack:latest -t fir.love.io:3005/zmq-stack:latest .
    docker push fir.love.io:3005/zmq-stack:latest
    cd ..
    ;;
conv-stack)
    cp ../bin/conv-stack conv-stack/
    cd conv-stack || exit
    docker build -t conv-stack:latest -t fir.love.io:3005/conv-stack:latest .
    docker push fir.love.io:3005/conv-stack:latest
    cd ..
    ;;
net-stack)
    cp ../bin/net-stack net-stack/
    cd net-stack || exit
    docker build -t net-stack:latest -t fir.love.io:3005/net-stack:latest .
    docker push fir.love.io:3005/net-stack:latest
    cd ..
    ;;
esac

