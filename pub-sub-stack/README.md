# Stack executables

Stacks are comprized of ZMQ PUB and SUB sockets and can be configured to subscribe to one or more topics and publish to one or more topics

## zmq-proxy 
- XPUB/XSUB proxy for use with standalone stack apps

## zmq-stack 
- executable with a single ZmqStack instance
- Supports interactive loop for accepting messages to be pushed to specific topics.  
- Uses 'TCP' transport to an XPUB/XSUB proxy.

## conv-stack 
- executable with a single ConvStack instance configured to convert from a message received on one topic to publish the message to one or more topics. 
- Uses 'TCP' transport to an XPUB/XSUB proxy.

## inproc-stack 
- executable with:
   - 4 ZmqStack instances all using 'inproc' transport,
   - 2 NetStack instances listening on UDP ports 6000 and 6001, sending to ports 7000 and 7001
   - a ConvStack routing messages between the NetStack instances. 
- Supports interactive loop for accepting messages to be published by 1 stack to specific topics

# Control messages
- Each `Subscriber` instance subscribes to the `CTRL` topic and messages received on this topic are passed to the `onCtrlMessage()` callback handled by `ZmqStack`, `ConvStack` or other subclass.

## Control message format
<name> [cmd1] ... [cmdn]

Where <name> is the name of the stack which should process the message
    [cmdX] - command

Commands handled by `ZmqStack`
- `S:<topic>` - stack should subscribe to a topic
- `U:<topic>` - stack should unsubscribe to a topic

Commands handled by `ConvStack`
- `P:<subtopic>:<pubtopic>`