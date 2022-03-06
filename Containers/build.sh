#!/bin/bash

cp ../bin/zmq_proxy zmq_proxy/
cp ../bin/zmq_producer zmq_producer/
cp ../bin/zmq_subscriber zmq_subscriber/

cd zmq_proxy
docker build -t zmq_proxy:latest -t fir.love.io:3005/zmq_proxy:latest .
docker push fir.love.io:3005/zmq_proxy:latest
cd ..

cd zmq_producer
docker build -t zmq_producer:latest -t fir.love.io:3005/zmq_producer:latest .
docker push fir.love.io:3005/zmq_producer:latest
cd ..

cd zmq_subscriber
docker build -t zmq_subscriber:latest -t fir.love.io:3005/zmq_subscriber:latest .
docker push fir.love.io:3005/zmq_subscriber:latest
cd ..