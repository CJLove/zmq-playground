# zmq-playground

Representative set of ZMQ-based microservices using the ZMQ XPUB/XSUB proxy pattern

## Code:
- common/ - common/utility code
- zmq-proxy/ - a ZMQ XPUB/XSUB proxy service
- zmq-stack/ - service which subscribes to and logs messages received on a configurable set of ZMQ topics and periodically publishes messages to a configurable set of ZMQ topics
- conv-stack/ - a service which receives messages on a configurable set of topics and converts them to a configurable set of ZMQ topics to which they are published
- net-stack/ - a service which sets up a TCP server to receive messages from outside client(s). Messages are published to a configurable set of ZMQ topics. Also subscribes to a configurable set of ZMQ topics and broadcasts those messages to all TCP clients
- inproc-stack/ a monolithic executable using the ZMQ inproc transport aggregating instances of all services
- net-driver/ - a TCP client application which will communication over TCP with a specific net-stack service instance

## Containers
Build script and Dockerfiles for all services

## Kubernetes
Manifests for deploying services as follows in the `zmq-test` namespace:
- zmq-proxy in a StatefulSet (3 replicas, not expected to scale up/down)
- conv-stack in a Deployment (1 replica)
- zmq-stack in a StatefulSet (2 replicas, scalable)
- net-stack in a StatefulSet (2 replicas, scalable)

## Helm
`zmq-playground` chart (in progress) for deploying these services

```bash
$ cd Helm
$ helm install -n <namespace> <deployment name> ./zmq-playground
```