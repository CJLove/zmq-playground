# Pub-Sub With XPub/XSub Proxy

Example of ZeroMQ Pub-Sub using an XPub/XSub Proxy.

## Running

```bash
$ ./zmq_proxy

$ ./zmq_subscriber

$ ./zmq_producer
```

## Command-line configurability
- The `zmq_proxy` can be configured to use different ports for XPUB and XSUB

- The `zmq_subscriber` and `zmq_producer` can be configured as far as the XPUB and XSUB endpoints and the request/response topics.