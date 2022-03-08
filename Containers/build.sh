#!/bin/bash

cp ../bin/zmq_proxy zmq_proxy/
cp ../bin/zmq_producer zmq_producer/
cp ../bin/zmq_subscriber zmq_subscriber/
cp ../bin/zmq_pub zmq_pub/
cp ../bin/zmq_sub zmq_sub/

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

cd zmq_pub
docker build -t zmq_pub:latest -t fir.love.io:3005/zmq_pub:latest .
docker push fir.love.io:3005/zmq_pub:latest
cd ..

cd zmq_sub
docker build -t zmq_sub:latest -t fir.love.io:3005/zmq_sub:latest .
docker push fir.love.io:3005/zmq_sub:latest
cd ..