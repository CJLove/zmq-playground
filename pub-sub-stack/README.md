# Stack executables

Stacks are comprized of ZMQ PUB and SUB sockets and can be configured to subscribe to one or more topics and publish to one or more topics


## zmq-proxy 
- XPUB/XSUB proxy for use with standalone stack apps

## zmq-stack 
- executable with a single ZmqStack instance and interactive loop for accepting messages to be pushed to specific topics.  Uses 'TCP' transport to an XPUB/XSUB proxy.

## conv-stack 
- executable with a single ConvStack instance configured to convert from a message received on one topic to publish the message to one or more topics. Uses 'TCP' transport to an XPUB/XSUB proxy.

## inproc-stack 
- executable with multiple ZmqStack instances using 'inproc' transport.  Supports interactive loop for accepting messages to be published by 1 stack to specific topics

