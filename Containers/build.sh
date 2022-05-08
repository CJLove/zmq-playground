#!/bin/bash

arg=$1

case $arg in
zmq_proxy)
    cp ../bin/zmq_proxy zmq_proxy/
    cd zmq_proxy
    docker build -t zmq_proxy:latest -t fir.love.io:3005/zmq_proxy:latest .
    docker push fir.love.io:3005/zmq_proxy:latest
    cd ..
    ;;
zmq_producer)
    cp ../bin/zmq_producer zmq_producer/
    cd zmq_producer
    docker build -t zmq_producer:latest -t fir.love.io:3005/zmq_producer:latest .
    docker push fir.love.io:3005/zmq_producer:latest
    cd ..
    ;;
zmq_subscriber)
    cp ../bin/zmq_subscriber zmq_subscriber/
    cd zmq_subscriber
    docker build -t zmq_subscriber:latest -t fir.love.io:3005/zmq_subscriber:latest .
    docker push fir.love.io:3005/zmq_subscriber:latest
    cd ..
    ;;
zmq_pub)
    cp ../bin/zmq_pub zmq_pub/
    cd zmq_pub
    docker build -t zmq_pub:latest -t fir.love.io:3005/zmq_pub:latest .
    docker push fir.love.io:3005/zmq_pub:latest
    cd ..
    ;;
zmq_sub)
    cp ../bin/zmq_sub zmq_sub/
    cd zmq_sub
    docker build -t zmq_sub:latest -t fir.love.io:3005/zmq_sub:latest .
    docker push fir.love.io:3005/zmq_sub:latest
    cd ..
    ;;
zmq-proxy)
    cp ../bin/zmq-proxy zmq-proxy/
    cd zmq-proxy
    docker build -t zmq-proxy:latest -t fir.love.io:3005/zmq-proxy:latest .
    docker push fir.love.io:3005/zmq-proxy:latest
    cd ..
    ;;
zmq-stack)
    cp ../bin/zmq-stack zmq-stack/
    cd zmq-stack
    docker build -t zmq-stack:latest -t fir.love.io:3005/zmq-stack:latest .
    docker push fir.love.io:3005/zmq-stack:latest
    cd ..
    ;;
zmq-conv)
    cp ../bin/conv-stack zmq-conv-stack/
    cd zmq-conv-stack
    docker build -t zmq-conv-stack:latest -t fir.love.io:3005/zmq-conv-stack:latest .
    docker push fir.love.io:3005/zmq-conv-stack:latest
    cd ..
    ;;
esac

