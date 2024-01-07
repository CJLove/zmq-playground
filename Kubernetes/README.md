# Sandbox for Kubernetes
- Deploy to `zmq-test` namespace
- Deploy zmq-proxy in `StatefulSet` for reliability (3 replicas)



## Proxy
```bash
$ kubectl apply -f zmq-proxy-ss.yaml
```
Note: there is a service spanning the 3 proxies for subscription and services for each proxy instance for publishing

## ZMQ Stack
Deply the `zmq-stack` stateful set:
```bash
$ kubectl apply -f zmq-stack-ss.yaml
```

## Converter
Deploy the `conv-stack` container with a ConfigMap as "converter":
```bash
$ kubectl apply -f conv-stack.yaml
```

## Net Stack
Deply the `net-stack` stateful set:
```bash
$ kubectl apply -f net-stack-ss.yaml
```