# Sandbox for Kubernetes
- Deploy to `zmq-test` namespace
- Deploy zmq-proxy in `StatefulSet` for reliability (3 replicas)



## Proxy
```bash
$ kubectl apply -f zmq-proxy-ss.yaml
```
Note: there is a service spanning the 3 proxies for 
## Stack A
Deploy the `zmq-stack` container with a ConfigMap as "stackA":
```bash
$ kubectl apply -f zmq-stackA.yaml
```

## Stack B
Deploy the `zmq-stack` container with a ConfigMap as "stackB":
```bash
$ kubectl apply -f zmq-stackB.yaml
```

## Converter
Deploy the `conv-stack` container with a ConfigMap as "converter":
```bash
$ kubectl apply -f conv-stack.yaml